add_definitions(-DFOUND_FREETYPE)

add_executable(zap_example-shared zap_example/zap_example.cpp)
target_include_directories(zap_example-shared PRIVATE core)
target_link_libraries(zap_example-shared zapHostGLFW-shared zapRenderer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)

install(TARGETS zap_example-shared
        EXPORT zapTargets
        RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
        COMPONENT bin)
