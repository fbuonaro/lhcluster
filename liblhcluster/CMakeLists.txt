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
set( LH_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( LH_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/liblhcluster_${PROJECT_VERSION}" )
set( LH_INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/lhcluster_${PROJECT_VERSION}" )

# pull in boost log
find_package( Boost 1.69 COMPONENTS log REQUIRED )
# pull in protobuf
find_package( Protobuf 3.0 REQUIRED )
# TODO - zmq/czmq

# generate protobuf source and headers from prototype
PROTOBUF_GENERATE_CPP(
    PROTO_SRCS PROTO_HDRS "./proto/zmqframetype.proto" )

# library name
set( LH_LIB_NAME lhcluster )

# source files
set( LH_LIB_SRC_FILES 
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
set( LH_LIB_PUBLIC_LINKLIBS 
     czmq
     zmq
     "${Boost_LIBRARIES}" )
set( LH_LIB_PRIVATE_LINKLIBS pthread protobuf )
# header dependencies
set( LH_LIB_PUBLIC_INCLUDES 
     "${Boost_INCLUDE_DIR}"
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster>   # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                          # export
set( LH_LIB_PRIVATE_INCLUDES 
     "${CMAKE_CURRENT_BINARY_DIR}" 
     "${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster_impl"
     "${CMAKE_CURRENT_SOURCE_DIR}/include/lhcluster_stats" )

lh_add_library()

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
                                "${LH_LIB_PRIVATE_INCLUDES}"
                                "${CMAKE_CURRENT_BINARY_DIR}"
                                "${GTEST_INCLUDE_DIRS}" )

include( GoogleTest )
gtest_add_tests( TARGET testlhcluster )

##############################################################
# installation
##############################################################

# name of component to install
set(LH_COMPONENT_NAME liblhcluster )
# directories to install
set(LH_LIB_PUBLIC_INCLUDE_DIRS "include/lhcluster/lhcluster")

lh_add_install_library()

##############################################################
# pkg-config
##############################################################
set( PC_INCDIR "${LH_INSTALL_INCDIR}")
set( PC_LIBDIR "${LH_INSTALL_LIBDIR}")
set( PC_PUBREQS )
set( PC_PRIVREQS )
set( PC_CFLAGS "-DBOOST_LOG_DYN_LINK")
set( PC_PUBLIBS "-llhcluster" )
set( PC_PRIVLIBS )
set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )

lh_add_pkgconfig()

##############################################################
# cmake packaging
##############################################################

lh_add_install_cmake_config()

##############################################################
# rpm packaging
##############################################################
 
set( CPACK_RPM_LIBLHCLUSTER_PACKAGE_REQUIRES "czmq-devel >= 3.0.2" PARENT_SCOPE)

# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_LIBLHCLUSTER_PACKAGE_REQUIRES_POST "sed" PARENT_SCOPE) 
set( CPACK_RPM_LIBLHCLUSTER_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" PARENT_SCOPE)

set(LH_PACKAGE_DESC "liblhcluster libraries package")

lh_add_cpack_component()
