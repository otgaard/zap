set(PUBLIC_HEADERS
        buffer.hpp
        engine.hpp
        export.hpp
        framebuffer.hpp
        index_buffer.hpp
        mesh.hpp
        pixel_buffer.hpp
        pixel_format.hpp
        pixel_conversion.hpp
        pixmap.hpp
        program.hpp
        render_state.hpp
        sampler.hpp
        shader.hpp
        state_stack.hpp
        texture.hpp
        types.hpp
        uniform_block.hpp
        uniform_buffer.hpp
        vertex_attribute.hpp
        vertex_buffer.hpp
        vertex_format.hpp
        )

set(SOURCE_FILES
        buffer.cpp
        framebuffer.cpp
        gl_api.hpp
        gl_api.cpp
        mesh.cpp
        pixel_conversion.cpp
        program.cpp
        sampler.cpp
        shader.cpp
        state_stack.cpp
        texture.cpp
        vertex_buffer.cpp
        )

if(DYNAMIC_LINKAGE)
    add_library(zapEngine-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapEngine-shared
            PRIVATE core
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapEngine-shared PROPERTIES OUTPUT_NAME "zapEngineD")
    else()
        set_target_properties(zapEngine-shared PROPERTIES OUTPUT_NAME "zapEngine")
    endif()

    target_link_libraries(zapEngine-shared zapMaths-shared ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})
    target_compile_definitions(zapEngine-shared PRIVATE -DGLEW_STATIC)

    install(TARGETS zapEngine-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
if(WIN32)
    set_target_properties(zapEngine-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    GENERATE_EXPORT_HEADER(zapEngine-shared
            BASE_NAME zapEngine
            EXPORT_MACRO_NAME ZAPENGINE_EXPORT
            EXPORT_FILE_NAME engine_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
endif()

endif()

if(STATIC_LINKAGE)
    add_library(zapEngine-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapEngine-static
            PRIVATE core
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)
    target_compile_definitions(zapEngine-static PRIVATE -DGLEW_STATIC)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapEngine-static PROPERTIES OUTPUT_NAME "zapEngineD")
    else()
        set_target_properties(zapEngine-static PROPERTIES OUTPUT_NAME "zapEngine")
    endif()

    set_target_properties(zapEngine-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapEngine-static zapMaths-static ${OPENGL_gl_LIBRARY} ${GLEW_LIBRARY})

    install(TARGETS zapEngine-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)

if(WIN32)
    set_target_properties(zapEngine-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    GENERATE_EXPORT_HEADER(zapEngine-static
            BASE_NAME zapEngine
            EXPORT_MACRO_NAME ZAPENGINE_EXPORT
            EXPORT_FILE_NAME engine_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
endif()

endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/engine/${dir})
endforeach()
