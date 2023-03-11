cmake_minimum_required( VERSION 3.10 )

project( pylhcluster 
         DESCRIPTION "pylhclusterext library"
         HOMEPAGE_URL "https://github.com/fbuonaro/lhcluster/pylhcluster"
         VERSION 1.0.0 )

#############################################################
# python packaging (python wheel with library)
##############################################################

# TODO - figure out right platform/arch, boost.python3 + libpylhclusterext is limited to something, not sure what
# TODO - allow configure the path to right version of pip to install with
#        and pass to pip_install_wheel_to_local
#        hardcode 36 at the moment since nearly everything is 3.6 based

set( PY_WHL_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/dist" )
set( PY_WHL_TARGET_FILE_NAME "${PROJECT_NAME}-${PROJECT_VERSION}-cp36-none-linux_x86_64.whl" )
set( PY_WHL_TARGET_FILE "${CMAKE_CURRENT_BINARY_DIR}/${PY_WHL_TARGET_FILE_NAME}" )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/copy_poetry_lock.cmake.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake/copy_poetry_lock.cmake"
                @ONLY )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/rename_whl.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake/rename_whl.sh"
                @ONLY )

configure_file( "${CMAKE_CURRENT_SOURCE_DIR}/cmake/pip_install_wheel_to_local.sh.in"
                "${CMAKE_CURRENT_BINARY_DIR}/cmake/pip_install_wheel_to_local.sh"
                @ONLY )

# TODO - generalize by splitting out generic-setup from project-specific-setup 
#        i.e. generic = mypy.ini, specific = copy pylhcluster dir
add_custom_target( pylhcluster-setup )
add_dependencies( pylhcluster-setup pylhclusterext )
add_custom_command( TARGET pylhcluster-setup POST_BUILD
    # Copy generic files to the build dir
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/pyproject.toml"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/copy_poetry_lock.cmake"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/mypy.ini"
            "${CMAKE_CURRENT_BINARY_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/tox.ini"
            "${CMAKE_CURRENT_BINARY_DIR}"
    # Copy specific files to the build dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/pylhcluster"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/pylhcluster_test"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster_test"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CMAKE_CURRENT_SOURCE_DIR}/test"
            "${CMAKE_CURRENT_BINARY_DIR}/test"
    COMMAND ${CMAKE_COMMAND} -E copy
            "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
            "${CMAKE_CURRENT_BINARY_DIR}"
    # Copy very specific pylhclusterext to plhc_ext in build dir
    COMMAND ${CMAKE_COMMAND} -E copy
            "$<TARGET_FILE:pylhclusterext>"
            "${CMAKE_CURRENT_BINARY_DIR}/pylhcluster/plhc_ext/$<TARGET_LINKER_FILE_NAME:pylhclusterext>" )

# check syntax, build integrity
add_custom_target( pylhcluster-check )
add_dependencies( pylhcluster-check pylhcluster-setup )
add_custom_command( TARGET pylhcluster-check POST_BUILD
    COMMAND poetry check
    COMMAND tox -e mypy
    COMMAND tox -e build-check )

add_custom_target( pylhcluster-lock )
add_dependencies( pylhcluster-lock pylhcluster-check )
add_custom_command( TARGET pylhcluster-lock POST_BUILD
    COMMAND poetry lock )

# ALL will cause target to be built by default
add_custom_target( pylhcluster ALL )
add_dependencies( pylhcluster pylhcluster-check )
add_custom_command( TARGET pylhcluster POST_BUILD
    # build wheel
    COMMAND poetry build --format wheel 
    # rename wheel to a more appropriate name
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cmake/rename_whl.sh
    # install wheel locally to pick up all files to be installed by cmake/cpack
    COMMAND ${CMAKE_CURRENT_BINARY_DIR}/cmake/pip_install_wheel_to_local.sh )

##############################################################
# python unit tests
##############################################################

enable_testing()

add_test( NAME pylhcluster_basic COMMAND tox -e pytest -- test_basic.py )
add_test( NAME pylhcluster_config COMMAND tox -e pytest -- test_config.py )
add_test( NAME pylhcluster_notify COMMAND tox -e pytest -- test_notify.py )

##############################################################
# python installation
##############################################################

# name of component to install
set(LH_COMPONENT_NAME pylhcluster )

# package
 
# TODO - determine what the destination lib should be
install( DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pip_install/lib/python3.6"
         DESTINATION "lib64"
         COMPONENT ${LH_COMPONENT_NAME} )

ADD_CUSTOM_TARGET( install-${LH_COMPONENT_NAME}
                   ${CMAKE_COMMAND}
                   -D "CMAKE_INSTALL_COMPONENT=${LH_COMPONENT_NAME}"
                   -P "cmake_install.cmake" )

##############################################################
# packaging
##############################################################

# set( CPACK_GENERATOR "RPM" "TGZ" )
# CPACK_RPM_BUILD_REQUIRES
set( CPACK_RPM_PYLHCLUSTER_PACKAGE_REQUIRES "python3-devel >= 1.1.0, lhcluster-liblhcluster >= 1.0.0" PARENT_SCOPE)

# to relocate the pc file prefix to /somewhere if 'rpm --install --prefix /somewhere'
set( CPACK_RPM_PYLHCLUSTER_PACKAGE_REQUIRES_POST "sed" PARENT_SCOPE) 
set( CPACK_RPM_PYLHCLUSTER_POST_INSTALL_SCRIPT_FILE "${PC_RELOCATE_SCRIPT}" PARENT_SCOPE)

set( LH_PACKAGE_DESC "pylhcluster modules package")

lh_add_cpack_component()