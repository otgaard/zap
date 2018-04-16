set(PUBLIC_HEADERS
        log.hpp
        os.hpp
        string.hpp
        threadpool.hpp
        )

add_library(tools INTERFACE)
target_include_directories(tools INTERFACE ${PUBLIC_HEADERS})

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/tools/${dir})
endforeach()
