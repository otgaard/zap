set(PUBLIC_HEADERS
        canvas.hpp)

set(SOURCE_FILES
        canvas.cpp)

if(DYNAMIC_LINKAGE)
    add_library(zapCanvas-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapCanvas-shared
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapCanvas-shared PROPERTIES OUTPUT_NAME "zapCanvasD")
    else()
        set_target_properties(zapCanvas-shared PROPERTIES OUTPUT_NAME "zapCanvas")
    endif()

    add_dependencies(zapCanvas-shared zapGraphics-shared zapEngine-shared zapMaths-shared core)
    target_link_libraries(zapCanvas-shared zapGraphics-shared zapEngine-shared zapMaths-shared)

    install(TARGETS zapCanvas-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
if(WIN32)
    set_target_properties(zapCanvas-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    GENERATE_EXPORT_HEADER(zapCanvas-shared
            BASE_NAME zapCanvas
            EXPORT_MACRO_NAME ZAPCANVAS_EXPORT
            EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/canvas_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
    target_compile_definitions(zapCanvas-shared PUBLIC -DCANVAS_EXPORT="canvas_exports.h")
    install(FILES "${CMAKE_BINARY_DIR}/exports/canvas_exports.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/rasteriser/${dir})
endif()

endif()

if(STATIC_LINKAGE)
    add_library(zapCanvas-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapCanvas-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapCanvas-static PROPERTIES OUTPUT_NAME "zapCanvasD")
    else()
        set_target_properties(zapCanvas-static PROPERTIES OUTPUT_NAME "zapCanvas")
    endif()

    add_dependencies(zapCanvas-static zapGraphics-static zapEngine-static zapMaths-static core)

    set_target_properties(zapCanvas-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapCanvas-static zapGraphics-static zapEngine-static zapMaths-static)

    install(TARGETS zapCanvas-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)

    if(WIN32)
        set_target_properties(zapCanvas-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
        GENERATE_EXPORT_HEADER(zapCanvas-static
                BASE_NAME zapCanvas
                EXPORT_MACRO_NAME ZAPCANVAS_EXPORT
                EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/canvas_exports_s.h
                STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
        target_compile_definitions(zapCanvas-static PUBLIC -DCANVAS_EXPORT="canvas_exports_s.h")
        install(FILES "${CMAKE_BINARY_DIR}/exports/canvas_exports_s.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/rasteriser/${dir})
    endif()

endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/rasteriser/${dir})
endforeach()
