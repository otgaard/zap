add_definitions(-DFOUND_FREETYPE)

if(DYNAMIC_LINKAGE)
add_executable(zap_example-shared zap_example/zap_example.cpp)
target_include_directories(zap_example-shared
        PRIVATE core
        PRIVATE ${CMAKE_BINARY_DIR}/exports)
target_link_libraries(zap_example-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)
set_target_properties(zap_example-shared PROPERTIES OUTPUT_NAME "zap_example")
set_target_properties(zap_example-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
install(TARGETS zap_example-shared
        EXPORT zapTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        COMPONENT bin)
endif()

if(STATIC_LINKAGE)
add_executable(zap_example-static zap_example/zap_example.cpp)
target_include_directories(zap_example-static
        PRIVATE core
        PRIVATE ${CMAKE_BINARY_DIR}/exports)

target_link_libraries(zap_example-static zapHostGLFW-static zapRenderer-static zapGraphics-static zapEngine-static zapMaths-static)
set_target_properties(zap_example-static PROPERTIES OUTPUT_NAME "zap_example_s")
set_target_properties(zap_example-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
install(TARGETS zap_example-static
        EXPORT zapTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        COMPONENT bin)
endif()
