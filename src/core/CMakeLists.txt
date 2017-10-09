set(PUBLIC_HEADERS
        bitfield.hpp
        core.hpp
        enumfield.hpp
        meta.hpp
        pod.hpp
        )

add_library(core INTERFACE)
target_include_directories(core INTERFACE ${PUBLIC_HEADERS})

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/core/${dir})
endforeach()
