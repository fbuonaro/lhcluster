cmake_minimum_required( VERSION 3.10 )

project( liblhcluster 
         DESCRIPTION "lhcluster library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhcluster/liblhcluster"
         LANGUAGES CXX
         VERSION 1.0.0 )

##############################################################
# general
##############################################################

include( GNUInstallDirs )

set( CMAKE_CXX_STANDARD 11 )

add_compile_options( -Wall -Wextra -pedantic -Werror -DBOOST_LOG_DYN_LINK) 

##############################################################
# library
##############################################################

# install directories
set( INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhcluster_${PROJECT_VERSION}" )
set( INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhcluster_${PROJECT_VERSION}" )

# pull in boost log
find_package( Boost 1.69 COMPONENTS log REQUIRED )
# pull in protobuf
find_package( Protobuf 3.0 REQUIRED )
# TODO - zmq/czmq

# generate protobuf source and headers from prototype
PROTOBUF_GENERATE_CPP(
    PROTO_SRCS PROTO_HDRS "./proto/zmqframetype.proto" )

# source files
set( LIB_SRC_FILES 
     "src/broker.cxx"
     "src/brokerrequesttypecontext.cxx"
     "src/client.cxx"
     "src/clientproxy.cxx"
     "src/clientrequestbuffer.cxx"
     "src/clientrequestcontext.cxx"
     "src/endpoint.cxx"
     "src/ibrokerfactory.cxx"
     "src/iclientfactory.cxx"
     "src/iclientproxyfactory.cxx"
     "src/inotifybrokerfactory.cxx"
     "src/inotifypublisherfactory.cxx"
     "src/inotifysubscriberfactory.cxx"
     "src/interfaces.cxx"
     "src/iworkerfactory.cxx"
     "src/logging.cxx"
     "src/notifybroker.cxx"
     "src/notifypublisher.cxx"
     "src/notifysubscriber.cxx"
     "src/requestprocessor.cxx"
     "src/requestresponse.cxx"
     "src/requeststate.cxx"
     "src/messagestats.cxx"
     "src/utils.cxx"
     "src/worker.cxx"
     "src/workerinfo.cxx"
     "src/workerinfolookup.cxx"
     "src/workerrequesttypecontext.cxx"
     "src/zmqframehandler.cxx"
     "src/endpointbuilder.cxx"
     "src/brokerparameters.cxx"
     "src/brokerparametersbuilder.cxx"
     "src/clientparameters.cxx"
     "src/clientparametersbuilder.cxx"
     "src/clientproxyparameters.cxx"
     "src/clientproxyparametersbuilder.cxx"
     "src/notifybrokerparameters.cxx"
     "src/notifybrokerparametersbuilder.cxx"
     "src/notifybrokerparameters.cxx"
     "src/notifybrokerparametersbuilder.cxx"
     "src/notifypublisherparameters.cxx"
     "src/notifypublisherparametersbuilder.cxx"
     "src/notifysubscriberparameters.cxx"
     "src/notifysubscriberparametersbuilder.cxx"
     "src/workerparameters.cxx"
     "src/workerparametersbuilder.cxx"
     ${PROTO_SRCS} )

# library dependencies
set( LIB_PUBLIC_LINKLIBS 
     czmq
     zmq
     "${Boost_LIBRARIES}" )
set( LIB_PRIVATE_LINKLIBS pthread protobuf )
# header dependencies
set( LIB_PUBLIC_INCLUDES 
     "${Boost_INCLUDE_DIR}"
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster>   # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                          # export
set( LIB_PRIVATE_INCLUDES 
     "${CMAKE_CURRENT_BINARY_DIR}" 
     "${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster_impl"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster_stats" )

# boilerplate ..
# libs
add_library( lhcluster SHARED ${LIB_SRC_FILES} )
add_library( lhcluster_static STATIC ${LIB_SRC_FILES} )
# lib deps
target_link_libraries( lhcluster
                       PUBLIC "${LIB_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIB_PRIVATE_LINKLIBS}" )
target_link_libraries( lhcluster_static
                       PUBLIC "${LIB_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIB_PRIVATE_LINKLIBS}" )
# header deps
target_include_directories( lhcluster
                            PUBLIC "${LIB_PUBLIC_INCLUDES}"
                            PRIVATE "${LIB_PRIVATE_INCLUDES}" )
target_include_directories( lhcluster_static
                            PUBLIC "${LIB_PUBLIC_INCLUDES}"
                            PRIVATE "${LIB_PRIVATE_INCLUDES}" )
# properties
set_target_properties( lhcluster
                       PROPERTIES 
                           LIBRARY_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           SOVERSION ${PROJECT_VERSION}
                           VERSION ${PROJECT_VERSION}
                           CLEAN_DIRECT_OUTPUT 1)
set_target_properties( lhcluster_static 
                       PROPERTIES
                           ARCHIVE_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           OUTPUT_NAME lhcluster )
# .. boilerplate

##############################################################
# unit tests
##############################################################

# pull in gtest
find_package( GTest REQUIRED )

add_executable( testlhcluster 
                "test/test.cxx"
                "test/testbroker.cxx"
                "test/testclient.cxx"
                "test/testclientproxy.cxx"
                "test/teste2enotifypubsub.cxx"
                "test/teste2eclientworker.cxx"
                "test/testlhcluster.cxx"
                "test/testnotifypublisher.cxx"
                "test/testnotifysubscriber.cxx"
                "test/testparameters.cxx"
                "test/testrequestprocessor.cxx"
                "test/testworker.cxx"
                "test/testzmqmessagefactoryutils.cxx"
                "test/testzmqsocketinterfaces.cxx"
                "test/testzmqsocketutils.cxx" )

target_link_libraries( testlhcluster
                       PRIVATE
                           "${GTEST_BOTH_LIBRARIES}"
                           lhcluster )

target_include_directories( testlhcluster
                            PRIVATE
                                "${LIB_PRIVATE_INCLUDES}"
                                "${CMAKE_CURRENT_BINARY_DIR}"
                                "${GTEST_INCLUDE_DIRS}" )

include( GoogleTest )
gtest_add_tests( TARGET testlhcluster )

##############################################################
# installation
##############################################################

# boilerplate ..
# install lib/bin
install( TARGETS lhcluster lhcluster_static
         EXPORT liblhcluster-targets 
         COMPONENT liblhcluster
         RUNTIME DESTINATION "${INSTALL_BINDIR}"
         COMPONENT liblhcluster
         LIBRARY DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhcluster
         ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
         COMPONENT liblhcluster
         INCLUDES DESTINATION "${LIB_PUBLIC_INCLUDES}" )
# install header dir
install( DIRECTORY "include/lhcluster/lhcluster"
         DESTINATION "${INSTALL_INCDIR}"
         COMPONENT liblhcluster
         FILES_MATCHING PATTERN "*.h" )
# install export for things outside of build
install( EXPORT liblhcluster-targets 
         FILE liblhclusterTargets.cmake 
         NAMESPACE liblhcluster:: 
         DESTINATION "${INSTALL_LIBDIR}/cmake" 
         COMPONENT liblhcluster )
# export export for things in build
export( EXPORT liblhcluster-targets
    FILE ${CMAKE_BINARY_DIR}/cmake/liblhclusterTargets.cmake 
    NAMESPACE liblhcluster:: )
ADD_CUSTOM_TARGET( install-lhcluster
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=liblhcluster"
                   -P "cmake_install.cmake" )
# .. boilerplate

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${INSTALL_INCDIR}")
set( PC_LIBDIR "${INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS "-DBOOST_LOG_DYN_LINK")
set( PC_PUBLIBS "-llhcluster" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

# boilerplate ..
# configure pc file
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/component.pc.in"
                "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
                @ONLY )
# install pc file
install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
         DESTINATION "${PC_INSTALL_DIR}"
         COMPONENT liblhcluster )
# configure pc relocation script
configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/rpm_relocate_pc.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh"
                @ONLY )
# .. boilerplate

##############################################################
# cmake packaging
##############################################################

# boilerplate ..
include(CMakePackageConfigHelpers)

configure_package_config_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
                               "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
                               INSTALL_DESTINATION "${INSTALL_LIBDIR}/cmake" )

write_basic_package_version_file( "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
                                  VERSION "${PROJECT_VERSION}"
                                  COMPATIBILITY SameMajorVersion )

install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
         DESTINATION "${INSTALL_LIBDIR}/cmake"
         COMPONENT liblhcluster )
# .. boilerplate

##############################################################
# rpm packaging
##############################################################
 
# boilerplate ..
set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
set( CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
set( CPACK_GENERATOR "RPM" )

# set( CPACK_GENERATOR "RPM" "TGZ" )
# CPACK_RPM_BUILD_REQUIRES
# set( CPACK_RPM_PACKAGE_REQUIRES "rapidjson-devel >= 1.1.0" )

set( CPACK_RPM_PACKAGE_RELOCATABLE ON)
set( CPACK_RPM_COMPONENT_INSTALL ON )

# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_PACKAGE_REQUIRES_POST "sed" ) 
set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" )

include( CPack )

# component, group can be different; group will appended to package
cpack_add_component(liblhcluster
    DISPLAY_NAME  "lhcluster library"
    DESCRIPTION   "lhcluster library"
    GROUP liblhcluster)
cpack_add_component_group(liblhcluster)
# .. boilerplate