set(PUBLIC_HEADERS
        camera.hpp
        light.hpp
        material.hpp
        render_args.hpp
        render_context.hpp
        renderer.hpp
        renderer_fwd.hpp
        shader_builder.hpp
        style.hpp)

set(SOURCE_FILES
        camera.cpp
        render_context.cpp
        renderer.cpp
        shader_builder.cpp
        style.cpp)

if(DYNAMIC_LINKAGE)
    add_library(zapRenderer-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapRenderer-shared
            PRIVATE core
            PRIVATE zapMaths-shared
            PRIVATE zapEngine-shared
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapRenderer-shared PROPERTIES OUTPUT_NAME "zapRendererD")
    else()
        set_target_properties(zapRenderer-shared PROPERTIES OUTPUT_NAME "zapRenderer")
    endif()

    target_link_libraries(zapRenderer-shared zapEngine-shared zapMaths-shared ${OPENGL} ${GLEW_LIBRARY})

    install(TARGETS zapRenderer-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

if(STATIC_LINKAGE)
    add_library(zapRenderer-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapRenderer-static
            PRIVATE core
            PRIVATE zapMaths-shared
            PRIVATE zapEngine-shared
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapRenderer-static PROPERTIES OUTPUT_NAME "zapRendererD")
    else()
        set_target_properties(zapRenderer-static PROPERTIES OUTPUT_NAME "zapRenderer")
    endif()

    set_target_properties(zapRenderer-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapRenderer-static zapEngine-static zapMaths-static ${OPENGL} ${GLEW_LIBRARY})

    install(TARGETS zapRenderer-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/renderer/${dir})
endforeach()
