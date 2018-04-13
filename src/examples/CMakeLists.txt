add_definitions(-DFOUND_FREETYPE)

if(DYNAMIC_LINKAGE)
    #
    add_executable(zap_example-shared zap_example.cpp)
    target_include_directories(zap_example-shared
            PRIVATE core
            PRIVATE zapMaths-shared
            PRIVATE zapEngine-shared
            PRIVATE zapRenderer-shared
            PRIVATE zapHostGLFW-shared
            PRIVATE ${CMAKE_BINARY_DIR}/exports)
    target_link_libraries(zap_example-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)
    set_target_properties(zap_example-shared PROPERTIES OUTPUT_NAME "zap_example")

    add_dependencies(zap_example-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared core)

    if(WIN32)
    set_target_properties(zap_example-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    endif()

    install(TARGETS zap_example-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
            COMPONENT bin)

    # Post-process Effects
    add_executable(blur_glow-shared blur_glow.cpp)
    target_include_directories(blur_glow-shared PRIVATE core PRIVATE ${CMAKE_BINARY_DIR}/exports)
    target_link_libraries(blur_glow-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)
    set_target_properties(blur_glow-shared PROPERTIES OUTPUT_NAME "blur_glow")

    if(WIN32)
        set_target_properties(blur_glow-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
        target_include_directories(blur_glow-shared PRIVATE ${CMAKE_SOURCE_DIR}/third_party/asio/include)
    endif()

    install(TARGETS blur_glow-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
            COMPONENT bin)

    # Scene Graph Tests & Construction
    add_executable(scene_graph-shared scene_graph.cpp)
    target_include_directories(scene_graph-shared PRIVATE core PRIVATE ${CMAKE_BINARY_DIR}/exports)
    target_link_libraries(scene_graph-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)
    set_target_properties(scene_graph-shared PROPERTIES OUTPUT_NAME "scene_graph")

    if(WIN32)
        set_target_properties(scene_graph-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
        target_include_directories(scene_graph-shared PRIVATE ${CMAKE_SOURCE_DIR}/third_party/asio/include)
    endif()

    install(TARGETS scene_graph-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
            COMPONENT bin)
endif() # Dynamic Linkage

if(STATIC_LINKAGE)
    add_executable(zap_example-static zap_example.cpp)
    target_include_directories(zap_example-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    target_link_libraries(zap_example-static zapHostGLFW-static zapRenderer-static zapGraphics-static zapEngine-static zapMaths-static)
    set_target_properties(zap_example-static PROPERTIES OUTPUT_NAME "zap_example_s")

    if(WIN32)
    set_target_properties(zap_example-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    endif()

    install(TARGETS zap_example-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
            COMPONENT bin)

    # Template
    add_executable(template-static template.cpp)
    target_include_directories(template-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    target_link_libraries(template-static zapHostGLFW-static zapRenderer-static zapGraphics-static zapEngine-static zapMaths-static stb)
    set_target_properties(template-static PROPERTIES OUTPUT_NAME "template_s")

    add_executable(models-static models.cpp)
    target_include_directories(models-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    target_link_libraries(models-static zapHostGLFW-static zapRenderer-static zapGraphics-static zapEngine-static zapMaths-static stb)
    set_target_properties(models-static PROPERTIES OUTPUT_NAME "models")

    add_executable(dynamic-static dynamic.cpp)
    target_include_directories(dynamic-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    target_link_libraries(dynamic-static zapHostGLFW-static zapRenderer-static zapGraphics-static zapEngine-static zapMaths-static stb)
    set_target_properties(dynamic-static PROPERTIES OUTPUT_NAME "dynamic_s")
endif()
