cmake_minimum_required( VERSION 3.10 )

project( libpylhclusterext 
         DESCRIPTION "pylhclusterext library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhcluster/libpylhclusterext"
         LANGUAGES CXX
         VERSION 1.0.0 )

##############################################################
# general
##############################################################

include( GNUInstallDirs )

set( CMAKE_CXX_STANDARD 11 )

add_compile_options( -Wall -Wextra -pedantic -Werror)

##############################################################
# library
##############################################################

# install directories
set( INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/libpylhclusterext_${PROJECT_VERSION}" )
set( INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/pylhclusterext_${PROJECT_VERSION}" )

# pull in python3
find_package( Python3 3.6 COMPONENTS Interpreter Development EXACT REQUIRED )
# pull in boost
find_package( Boost 1.69 COMPONENTS python36 REQUIRED )

# source files
set( LIB_SRC_FILES 
     "src/pybroker.cxx"
     "src/pybrokerfactory.cxx"
     "src/pybrokerparameters.cxx"
     "src/pybrokerparametersbuilder.cxx"
     "src/pyclient.cxx"
     "src/pyclientfactory.cxx"
     "src/pyclientparameters.cxx"
     "src/pyclientparametersbuilder.cxx"
     "src/pyclientproxy.cxx"
     "src/pyclientproxyfactory.cxx"
     "src/pyclientproxyparameters.cxx"
     "src/pyclientproxyparametersbuilder.cxx"
     "src/pycluster.cxx"
     "src/pyendpoint.cxx"
     "src/pyendpointbuilder.cxx"
     "src/pyexport.cxx"
     "src/pylogging.cxx"
     "src/pynotifybroker.cxx"
     "src/pynotifybrokerfactory.cxx"
     "src/pynotifybrokerparameters.cxx"
     "src/pynotifybrokerparametersbuilder.cxx"
     "src/pynotifypublisher.cxx"
     "src/pynotifypublisherfactory.cxx"
     "src/pynotifypublisherparameters.cxx"
     "src/pynotifypublisherparametersbuilder.cxx"
     "src/pynotifysubscriber.cxx"
     "src/pynotifysubscriberfactory.cxx"
     "src/pynotifysubscriberparameters.cxx"
     "src/pynotifysubscriberparametersbuilder.cxx"
     "src/pyrequestresponse.cxx"
     "src/pyworker.cxx"
     "src/pyworkerfactory.cxx"
     "src/pyworkerparameters.cxx"
     "src/pyworkerparametersbuilder.cxx"
     "src/pyzmqmessage.cxx" )

# library dependencies
set( LIB_PUBLIC_LINKLIBS "${Boost_LIBRARIES}")
set( LIB_PRIVATE_LINKLIBS "${Python3_LIBRARIES}" lhcluster )
# header dependencies
set( LIB_PUBLIC_INCLUDES 
     "${Boost_INCLUDE_DIR}"
     "${Python3_INCLUDE_DIRS}"
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pylhclusterext>   # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                          # export
set( LIB_PRIVATE_INCLUDES )

# boilerplate ..
# libs
add_library( pylhclusterext SHARED ${LIB_SRC_FILES} )
# NOTE - STATIC LIBRARY IS INAPPLICABLE TO PYTHON EXTENSIONS
# add_library( pylhclusterext_static STATIC ${LIB_SRC_FILES} )
# lib deps
target_link_libraries( pylhclusterext
                       PUBLIC "${LIB_PUBLIC_LINKLIBS}"
                       PRIVATE "${LIB_PRIVATE_LINKLIBS}" )
# target_link_libraries( pylhclusterext_static
#                        PUBLIC "${LIB_PUBLIC_LINKLIBS}"
#                        PRIVATE "${LIB_PRIVATE_LINKLIBS}" )
# header deps
target_include_directories( pylhclusterext
                            PUBLIC "${LIB_PUBLIC_INCLUDES}"
                            PRIVATE "${LIB_PRIVATE_INCLUDES}" )
# target_include_directories( pylhclusterext_static
#                             PUBLIC "${LIB_PUBLIC_INCLUDES}"
#                             PRIVATE "${LIB_PRIVATE_INCLUDES}" )
# properties
set_target_properties( pylhclusterext
                       PROPERTIES 
                           LIBRARY_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
                           SOVERSION ${PROJECT_VERSION}
                           VERSION ${PROJECT_VERSION}
                           CLEAN_DIRECT_OUTPUT 1)
# set_target_properties( pylhclusterext_static 
#                        PROPERTIES
#                            ARCHIVE_OUTPUT_DIRECTORY "${INSTALL_LIBDIR}"
#                            OUTPUT_NAME pylhclusterext )
# .. boilerplate

##############################################################
# unit tests
##############################################################

# NOTE - NO TESTS, TESTED BY PYLHCLUSTER

##############################################################
# installation
##############################################################

# NOTE - NOT INSTALLED, DISTRIBUTED BY PYLHCLUSTER

# boilerplate ..
# install lib/bin
# install( TARGETS pylhclusterext pylhclusterext_static
#          EXPORT libpylhclusterext-targets 
#          COMPONENT libpylhclusterext
#          RUNTIME DESTINATION "${INSTALL_BINDIR}"
#          COMPONENT libpylhclusterext
#          LIBRARY DESTINATION "${INSTALL_LIBDIR}"
#          COMPONENT libpylhclusterext
#          ARCHIVE DESTINATION "${INSTALL_LIBDIR}"
#          COMPONENT libpylhclusterext
#          INCLUDES DESTINATION "${LIB_PUBLIC_INCLUDES}" )
# # install header dir
# install( DIRECTORY "include/pylhclusterext/pylhclusterext"
#          DESTINATION "${INSTALL_INCDIR}"
#          COMPONENT libpylhclusterext
#          FILES_MATCHING PATTERN "*.h" )
# # install export for things outside of build
# install( EXPORT libpylhclusterext-targets 
#          FILE libpylhclusterextTargets.cmake 
#          NAMESPACE libpylhclusterext:: 
#          DESTINATION "${INSTALL_LIBDIR}/cmake" 
#          COMPONENT libpylhclusterext )
# export export for things in build
# export( EXPORT libpylhclusterext-targets
#     FILE ${CMAKE_BINARY_DIR}/cmake/libpylhclusterextTargets.cmake 
#     NAMESPACE libpylhclusterext:: )
# ADD_CUSTOM_TARGET( install-pylhclusterext
#                    ${CMAKE_COMMAND}
#                    -D "CMAKE_INSTALL_COMPONENT=libpylhclusterext"
#                    -P "cmake_install.cmake" )
# .. boilerplate

##############################################################
# pkg-config
##############################################################
# set( PC_INCDIR "${INSTALL_INCDIR}")
# set( PC_LIBDIR "${INSTALL_LIBDIR}")
# set( PC_PUBREQS )
# set( PC_PRIVREQS )
# set( PC_CFLAGS "-DBOOST_LOG_DYN_LINK")
# set( PC_PUBLIBS "-lpylhclusterext -llhmiscutil -llhsslutil" )
# set( PC_PRIVLIBS )
# set( PC_INSTALL_DIR "${CMAKE_INSTALL_DATAROOTDIR}/pkgconfig" )
# set( PC_INSTALL_FILENAME "${PROJECT_NAME}-${CMAKE_PROJECT_VERSION_MAJOR}.pc" )
# set( PC_INSTALL_FILE "${PC_INSTALL_DIR}/${PC_INSTALL_FILENAME}" )
# set( PC_RELOCATE_SCRIPT "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh" )
# 
# # boilerplate ..
# # configure pc file
# configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/component.pc.in"
#                 "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
#                 @ONLY )
# # install pc file
# install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PC_INSTALL_FILENAME}"
#          DESTINATION "${PC_INSTALL_DIR}"
#          COMPONENT libpylhclusterext )
# # configure pc relocation script
# configure_file( "${CMAKE_SOURCE_DIR}/modules/lhscriptutil/cmake/pkgconfig/rpm_relocate_pc.sh.in"
#                 "${CMAKE_CURRENT_BINARY_DIR}/rpm_relocate_pc.sh"
#                 @ONLY )
# .. boilerplate

##############################################################
# cmake packaging
##############################################################

# boilerplate ..
# include(CMakePackageConfigHelpers)
# 
# configure_package_config_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}Config.cmake.in"
#                                "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
#                                INSTALL_DESTINATION "${INSTALL_LIBDIR}/cmake" )
# 
# write_basic_package_version_file( "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
#                                   VERSION "${PROJECT_VERSION}"
#                                   COMPATIBILITY SameMajorVersion )
# 
# install( FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake" "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
#          DESTINATION "${INSTALL_LIBDIR}/cmake"
#          COMPONENT libpylhclusterext )
# .. boilerplate

##############################################################
# rpm packaging
##############################################################
 
# boilerplate ..
# set( CPACK_PROJECT_NAME ${PROJECT_NAME} )
# set( CPACK_PROJECT_VERSION ${PROJECT_VERSION} )
# set( CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
# set( CPACK_GENERATOR "RPM" )
# 
# # set( CPACK_GENERATOR "RPM" "TGZ" )
# # CPACK_RPM_BUILD_REQUIRES
# # set( CPACK_RPM_PACKAGE_REQUIRES "rapidjson-devel >= 1.1.0" )
# 
# set( CPACK_RPM_PACKAGE_RELOCATABLE ON)
# set( CPACK_RPM_COMPONENT_INSTALL ON )
# 
# # to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
# set( CPACK_RPM_PACKAGE_REQUIRES_POST "sed" ) 
# set( CPACK_RPM_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" )
# 
# include( CPack )
# 
# # component, group can be different; group will appended to package
# cpack_add_component(libpylhclusterext
#     DISPLAY_NAME  "pylhclusterext library"
#     DESCRIPTION   "pylhclusterext library"
#     GROUP libpylhclusterext)
# cpack_add_component_group(libpylhclusterext)
# .. boilerplate