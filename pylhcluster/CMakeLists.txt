cmake_minimum_required( VERSION 3.10 )

project( pylhcluster )

set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11" )

##############################################################
# native extension module (library)
##############################################################

# pull in python
# TODO - FindPython3 is a bit off, it fails for cases where there
#        are multiple python installations but only some of them
#        have the development headers and libraries. Specifying
#        the exact version prevents this but is limiting.
find_package( Python3 3.6 COMPONENTS Interpreter Development EXACT REQUIRED )
if( NOT Python3_FOUND )
    message( FATAL_ERROR "python 3.6 could not be found" )
endif( NOT Python3_FOUND )

# pull in boost
find_package( Boost 1.69 COMPONENTS python36 REQUIRED )
if( NOT Boost_FOUND )
    message( FATAL_ERROR "boost python36 could not be found" )
endif( NOT Boost_FOUND )

set( LIB_SRC_DIR "libpylhclusterext/src" )

set( LIB_INC_DIR "libpylhclusterext/include" )

add_library( pylhclusterext SHARED
    "${LIB_SRC_DIR}/pybroker.cxx"
    "${LIB_SRC_DIR}/pybrokerfactory.cxx"
    "${LIB_SRC_DIR}/pybrokerparameters.cxx"
    "${LIB_SRC_DIR}/pybrokerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pyclient.cxx"
    "${LIB_SRC_DIR}/pyclientfactory.cxx"
    "${LIB_SRC_DIR}/pyclientparameters.cxx"
    "${LIB_SRC_DIR}/pyclientparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pyclientproxy.cxx"
    "${LIB_SRC_DIR}/pyclientproxyfactory.cxx"
    "${LIB_SRC_DIR}/pyclientproxyparameters.cxx"
    "${LIB_SRC_DIR}/pyclientproxyparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pycluster.cxx"
    "${LIB_SRC_DIR}/pyendpoint.cxx"
    "${LIB_SRC_DIR}/pyendpointbuilder.cxx"
    "${LIB_SRC_DIR}/pyexport.cxx"
    "${LIB_SRC_DIR}/pylogging.cxx"
    "${LIB_SRC_DIR}/pynotifybroker.cxx"
    "${LIB_SRC_DIR}/pynotifybrokerfactory.cxx"
    "${LIB_SRC_DIR}/pynotifybrokerparameters.cxx"
    "${LIB_SRC_DIR}/pynotifybrokerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pynotifypublisher.cxx"
    "${LIB_SRC_DIR}/pynotifypublisherfactory.cxx"
    "${LIB_SRC_DIR}/pynotifypublisherparameters.cxx"
    "${LIB_SRC_DIR}/pynotifypublisherparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pynotifysubscriber.cxx"
    "${LIB_SRC_DIR}/pynotifysubscriberfactory.cxx"
    "${LIB_SRC_DIR}/pynotifysubscriberparameters.cxx"
    "${LIB_SRC_DIR}/pynotifysubscriberparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pyrequestresponse.cxx"
    "${LIB_SRC_DIR}/pyworker.cxx"
    "${LIB_SRC_DIR}/pyworkerfactory.cxx"
    "${LIB_SRC_DIR}/pyworkerparameters.cxx"
    "${LIB_SRC_DIR}/pyworkerparametersbuilder.cxx"
    "${LIB_SRC_DIR}/pyzmqmessage.cxx" )

target_link_libraries( pylhclusterext
    PUBLIC
        "${Boost_LIBRARIES}"
    PRIVATE
        "${Python3_LIBRARIES}"
        lhcluster )

target_include_directories( pylhclusterext
    PRIVATE
        "${Boost_INCLUDE_DIR}"
        "${Python3_INCLUDE_DIRS}"
        "${LIB_INC_DIR}/pylhcluster" )

##############################################################
# python packaging (python sdist with library)
##############################################################

# ALL will cause target to be built by default
add_custom_target( pylhcluster ALL )
add_dependencies( pylhcluster pylhclusterext )
add_custom_command( TARGET pylhcluster POST_BUILD
    # Copy pylhcluster/, setup.py and tox.ini to the build dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/pylhcluster"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/setup.py"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/tox.ini"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/test"
            "${CMAKE_CURRENT_BINARY_DIR}/test"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/pylhcluster_test"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster_test"
    # Files used by setup.py/tox
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/MANIFEST.in"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
            "${CMAKE_CURRENT_BINARY_DIR}"
    # Copy pylhclusterext to plhc_ext
    COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:pylhclusterext>"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster/plhc_ext/$<TARGET_FILE_NAME:pylhclusterext>"
    COMMAND tox --notest )

##############################################################
# python unit tests
##############################################################

enable_testing()

add_test( NAME pylhcluster_basic COMMAND tox -e ALL -- test_basic.py )
add_test( NAME pylhcluster_config COMMAND tox -e ALL -- test_config.py )
add_test( NAME pylhcluster_notify COMMAND tox -e ALL -- test_notify.py )

##############################################################
# python installation
##############################################################

# Extract workdir from tox, find all files beneath its dist subdir
# and pip install them
set( AND_ECHO_IFDBG "" )
if( DEBUG_INSTALL_PYLHCLUSTER_COMMAND )
    set( AND_ECHO_IFDBG "| tee /dev/tty" )
endif( DEBUG_INSTALL_PYLHCLUSTER_COMMAND )

# PYTHON PACKAGING SAGA PART 1
# Would like to do something like this with tox
# Because I prefer to think of tox as the thing that "builds" packages
# But I stopped trying to use tox like this because its not tox's 
# purpose and I could not get # it to generate multiple eggs BUT 
# it turns out that this was a deadend anyway because setup controls
# the number of eggs generated AND eggs are deprecated anyway ??? so ..
# set( INSTALL_PYLHCLUSTER_COMMAND
#      "tox --showconfig ${AND_ECHO_IFDBG} | \
#       grep workdir ${AND_ECHO_IFDBG} | \
#       cut -d ':' -f 2 ${AND_ECHO_IFDBG} | \
#       xargs -I{} find {}/dist -maxdepth 1 -type f ${AND_ECHO_IFDBG} | \
#       grep -v -i '^test' ${AND_ECHO_IFDBG} | \
#       xargs -I{} pip3.6 install {} ${AND_ECHO_IFDBG}" )

# PYTHON PACKAGING SAGA PART 2
# Apparently pip no longer works with eggs so need to do something else
# set( INSTALL_PYLHCLUSTER_COMMAND 
#      "python3.6 setup.py bdist_egg ${AND_ECHO_IFDBG} && \
#       find ./dist -maxdepth 1 -type f ${AND_ECHO_IFDBG} | \
#       xargs -I{} pip3.6 install {} -t ${Python3_SITEARCH} ${ ${AND_ECHO_IFDBG}" )

# PYTHON PACKAGING SAGA PART 3
# So we are going to use easy_install instead BUT because of my
# unorthodox (?) way of building the native extension using cmake
# instead of setup.py Extension's, installing only the egg is 
# insufficient. The "cache" directory containing extracted files
# must always be installed otherwise the extension module cannot be
# imported. Apparently, python and eggs should take care of this 
# automatically if the library is listed in native_libs.txt (and it is)
# but this was not my observation SO we are going to pass -Z to 
# easy_install so that it always installs the extracted egg
set( INSTALL_PYLHCLUSTER_COMMAND 
     "python3.6 setup.py bdist_egg ${AND_ECHO_IFDBG} && \
      find ./dist -maxdepth 1 -type f ${AND_ECHO_IFDBG} | \
      xargs -I{} python3.6 -m easy_install -Z {} ${AND_ECHO_IFDBG}" )

# package
install( CODE
            "execute_process( \
                COMMAND bash -c \"${INSTALL_PYLHCLUSTER_COMMAND}\" )"
         COMPONENT pylhcluster )

ADD_CUSTOM_TARGET( install-pylhcluster
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=pylhcluster"
                   -P "cmake_install.cmake" )
