macro(lh_toplevel_add_subdirectories)
    foreach(subdirectory ${LH_SUBDIRECTORIES})
        add_subdirectory( ${subdirectory} )
    endforeach()
endmacro()

macro(lh_toplevel_add_cpack_components_from_subdirectories)
    foreach(subdirectory ${LH_SUBDIRECTORIES})
        foreach(component_desc ${"${subdirectory}_components"})
            string(FIND "${component_desc}" ":" pos)
            if (pos LESS 1)
                message(WARNING "Skipping malformed component (expected format <component>:<desc>): ${component_desc}")
            else ()
                string(SUBSTRING "${component_desc}" 0 "${pos}" component)
                math(EXPR pos "${pos} + 1")  # Skip the separator
                string(SUBSTRING "${component_desc}" "${pos}" -1 desc)
                cpack_add_component(${component}
                    DISPLAY_NAME  ${desc}
                    DESCRIPTION   ${desc}
                    GROUP ${component})
                cpack_add_component_group(${component})
            endif ()
        endforeach()
    endforeach()
endmacro()

macro(lh_toplevel_add_cpack_package_components)
    set( CPACK_COMPONENTS_GROUPING ONE_PER_GROUP )
    set( CPACK_GENERATOR "RPM" )
    set( CPACK_RPM_PACKAGE_RELOCATABLE ON)
    set( CPACK_RPM_COMPONENT_INSTALL ON )
    include( CPack )
    lh_toplevel_add_cpack_components_from_subdirectories(${LH_SUBDIRECTORIES})
endmacro()

macro(lh_add_shared_library)
    # libs
    add_library( ${LH_LIB_NAME} SHARED ${LH_LIB_SRC_FILES} )
    add_library( ${LH_LIB_NAME}_static STATIC ${LH_LIB_SRC_FILES} )
    # lib deps
    target_link_libraries( ${LH_LIB_NAME}
                        PUBLIC "${LH_LIB_PUBLIC_LINKLIBS}"
                        PRIVATE "${LH_LIB_PRIVATE_LINKLIBS}" )
    target_link_libraries( ${LH_LIB_NAME}_static
                        PUBLIC "${LH_LIB_PUBLIC_LINKLIBS}"
                        PRIVATE "${LH_LIB_PRIVATE_LINKLIBS}" )
    # header deps
    target_include_directories( ${LH_LIB_NAME}
                                PUBLIC "${LH_LIB_PUBLIC_INCLUDES}"
                                PRIVATE "${LH_LIB_PRIVATE_INCLUDES}" )
    target_include_directories( ${LH_LIB_NAME}_static
                                PUBLIC "${LH_LIB_PUBLIC_INCLUDES}"
                                PRIVATE "${LH_LIB_PRIVATE_INCLUDES}" )
    # properties
    set_target_properties( ${LH_LIB_NAME}
                        PROPERTIES 
                            LIBRARY_OUTPUT_DIRECTORY "${LH_INSTALL_LIBDIR}"
                            SOVERSION ${PROJECT_VERSION}
                            VERSION ${PROJECT_VERSION}
                            CLEAN_DIRECT_OUTPUT 1)
    set_target_properties( ${LH_LIB_NAME}_static 
                        PROPERTIES
                            ARCHIVE_OUTPUT_DIRECTORY "${LH_INSTALL_LIBDIR}"
                            OUTPUT_NAME ${LH_LIB_NAME} )
endmacro()

macro(lh_add_static_library)
    # libs
    add_library( ${LH_LIB_NAME}_static STATIC ${LH_LIB_SRC_FILES} )
    # lib deps
    target_link_libraries( ${LH_LIB_NAME}_static
                        PUBLIC "${LH_LIB_PUBLIC_LINKLIBS}"
                        PRIVATE "${LH_LIB_PRIVATE_LINKLIBS}" )
    # header deps
    target_include_directories( ${LH_LIB_NAME}_static
                                PUBLIC "${LH_LIB_PUBLIC_INCLUDES}"
                                PRIVATE "${LH_LIB_PRIVATE_INCLUDES}" )
    # properties
    set_target_properties( ${LH_LIB_NAME}_static 
                        PROPERTIES
                            ARCHIVE_OUTPUT_DIRECTORY "${LH_INSTALL_LIBDIR}"
                            OUTPUT_NAME ${LH_LIB_NAME} )
endmacro()

macro(lh_add_library)
    lh_add_shared_library()
    lh_add_static_library()
endmacro()

macro(lh_add_install_library)
    # install lib/bin
    install( TARGETS ${LH_LIB_NAME} ${LH_LIB_NAME}_static
            EXPORT ${LH_COMPONENT_NAME}-targets 
            COMPONENT ${LH_COMPONENT_NAME}
            RUNTIME DESTINATION "${LH_INSTALL_BINDIR}"
            COMPONENT ${LH_COMPONENT_NAME}
            LIBRARY DESTINATION "${LH_INSTALL_LIBDIR}"
            COMPONENT ${LH_COMPONENT_NAME}
            ARCHIVE DESTINATION "${LH_INSTALL_LIBDIR}"
            COMPONENT ${LH_COMPONENT_NAME}
            INCLUDES DESTINATION "${LH_LIB_PUBLIC_INCLUDES}" )
    # install header dirs
    foreach(incdir ${LH_LIB_PUBLIC_INCLUDE_DIRS})
        install( DIRECTORY ${incdir}
                DESTINATION "${LH_INSTALL_INCDIR}"
                COMPONENT ${LH_COMPONENT_NAME}
                FILES_MATCHING PATTERN "*.h" )
    endforeach
    # install export for things outside of build
    install( EXPORT ${LH_COMPONENT_NAME}-targets 
            FILE ${LH_COMPONENT_NAME}Targets.cmake 
            NAMESPACE ${LH_COMPONENT_NAME}:: 
            DESTINATION "${LH_INSTALL_LIBDIR}/cmake" 
            COMPONENT ${LH_COMPONENT_NAME} )
    # export export for things in build
    export( EXPORT ${LH_COMPONENT_NAME}-targets
        FILE ${CMAKE_BINARY_DIR}/cmake/${LH_COMPONENT_NAME}Targets.cmake 
        NAMESPACE ${LH_COMPONENT_NAME}:: )
    ADD_CUSTOM_TARGET( install-${LH_COMPONENT_NAME}
                    ${CMAKE_COMMAND}
                    -D "CMAKE_INSTALL_COMPONENT=${LH_COMPONENT_NAME}"
                    -P "cmake_install.cmake" )
endmacro()

macro(lh_add_pkgconfig)
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
endmacro()

macro(lh_add_install_cmake_config)
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
endmacro()

macro(lh_add_cpack_component)
    set(${LH_COMPONENT_NAME}_cpack_components "${LH_COMPONENT_NAME}:${LH_PACKAGE_DESC}" PARENT_SCOPE)
endmacro()