set(PUBLIC_HEADERS
        hit_record.hpp
        raytracer.hpp)

set(SOURCE_FILES
        raytracer.cpp)

if(DYNAMIC_LINKAGE)
    add_library(zapTracer-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapTracer-shared
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapTracer-shared PROPERTIES OUTPUT_NAME "zapTracerD")
    else()
        set_target_properties(zapTracer-shared PROPERTIES OUTPUT_NAME "zapTracer")
    endif()

    add_dependencies(zapTracer-shared zapGraphics-shared zapEngine-shared zapMaths-shared core)
    target_link_libraries(zapTracer-shared zapGraphics-shared zapEngine-shared zapMaths-shared)

    install(TARGETS zapTracer-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
    if(WIN32)
        set_target_properties(zapTracer-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
        GENERATE_EXPORT_HEADER(zapTracer-shared
                BASE_NAME zapTracer
                EXPORT_MACRO_NAME ZAPTRACER_EXPORT
                EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/tracer_exports.h
                STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
        target_compile_definitions(zapTracer-shared PUBLIC -DTRACER_EXPORT="tracer_exports.h")
        install(FILES "${CMAKE_BINARY_DIR}/exports/tracer_exports.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/raytracer/${dir})
    endif()

endif()

if(STATIC_LINKAGE)
    add_library(zapTracer-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapTracer-static
            PRIVATE core
            PRIVATE ${CMAKE_BINARY_DIR}/exports)

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapTracer-static PROPERTIES OUTPUT_NAME "zapTracerD")
    else()
        set_target_properties(zapTracer-static PROPERTIES OUTPUT_NAME "zapTracer")
    endif()

    add_dependencies(zapTracer-static zapGraphics-static zapEngine-static zapMaths-static core)

    set_target_properties(zapTracer-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapTracer-static zapGraphics-static zapEngine-static zapMaths-static)

    install(TARGETS zapTracer-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)

    if(WIN32)
        set_target_properties(zapTracer-static PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
        GENERATE_EXPORT_HEADER(zapTracer-static
                BASE_NAME zapTracer
                EXPORT_MACRO_NAME ZAPTRACER_EXPORT
                EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/tracer_exports_s.h
                STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
        target_compile_definitions(zapTracer-static PUBLIC -DTRACER_EXPORT="tracer_exports_s.h")
        install(FILES "${CMAKE_BINARY_DIR}/exports/tracer_exports_s.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/raytracer/${dir})
    endif()

endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/raytracer/${dir})
endforeach()
