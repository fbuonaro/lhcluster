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
set( LH_INSTALL_BINDIR "${CMAKE_INSTALL_BINDIR}" )
set( LH_INSTALL_LIBDIR "${CMAKE_INSTALL_LIBDIR}/libpylhclusterext_${PROJECT_VERSION}" )
set( LH_INSTALL_INCDIR "${CMAKE_INSTALL_INCLUDEDIR}/pylhclusterext_${PROJECT_VERSION}" )

# pull in python3
find_package( Python3 3.6 COMPONENTS Interpreter Development EXACT REQUIRED )
# pull in boost
find_package( Boost 1.69 COMPONENTS python36 REQUIRED )

# library
set( LH_LIB_NAME pylhclusterext )
# source files
set( LH_LIB_SRC_FILES 
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
set( LH_LIB_PUBLIC_LINKLIBS "${Boost_LIBRARIES}")
set( LH_LIB_PRIVATE_LINKLIBS "${Python3_LIBRARIES}" lhcluster )
# header dependencies
set( LH_LIB_PUBLIC_INCLUDES 
     "${Boost_INCLUDE_DIR}"
     "${Python3_INCLUDE_DIRS}"
     $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/pylhclusterext>   # build
     $<INSTALL_INTERFACE:${INSTALL_INCDIR}> )                          # export
set( LH_LIB_PRIVATE_INCLUDES )

lh_add_shared_library()

##############################################################
# unit tests
##############################################################

# NOTE - NO UNIT TESTS, TESTED BY PYLHCLUSTER

##############################################################
# installation
##############################################################

# NOTE - NOT INSTALLED, DISTRIBUTED BY PYLHCLUSTER

##############################################################
# cmake packaging
##############################################################

# NOTE - NOT PACKAGED, DISTRIBUTED BY PYLHCLUSTER

##############################################################
# rpm packaging
##############################################################

# NOTE - NOT PACKAGED, DISTRIBUTED BY PYLHCLUSTER
