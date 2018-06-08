set(PUBLIC_HEADERS
        generators/geometry/geometry2.hpp
        generators/geometry/geometry3.hpp
        generators/geometry/geometry_traits.hpp
        generators/geometry/surface.hpp
        generators/noise/noise.hpp
        generators/noise/perlin.hpp
        generators/noise/value_noise.hpp
        generators/textures/convolution.hpp
        generators/textures/planar.hpp
        generators/textures/spectral.hpp
        generators/generator.hpp
        graphics2/text/font_manager.hpp
        graphics2/text/text.hpp
        graphics2/text/text_batcher.hpp
        graphics2/curve_input.hpp
        graphics2/g2_types.hpp
        graphics2/plotter2.hpp
        graphics2/quad.hpp
        graphics3/g3_types.hpp
        particle_engine/particle_engine.hpp
        loader/image_loader.hpp
        colour.hpp
        graphics.hpp
        loader/obj_loader.hpp
        shadermap/shadermap.hpp
        loader/image_writer.hpp
        graphics3/line_batch.hpp)

set(SOURCE_FILES
        generators/geometry/surface.cpp
        generators/noise/noise.cpp
        generators/generator.cpp
        graphics2/plotter/plot_sampler.hpp
        graphics2/plotter/plotter.cpp
        graphics2/plotter/plotter.hpp
        graphics2/text/font_manager.cpp
        graphics2/text/text.cpp
        graphics2/text/text_batcher.cpp
        #graphics2/curve_input.cpp
        graphics2/g2_types.cpp
        graphics2/plotter2.cpp
        graphics2/quad.cpp
        graphics3/g3_types.cpp
        particle_engine/particle_engine.cpp
        colour.cpp
        loader/obj_loader.cpp
        #shadermap/shadermap.cpp
        graphics3/line_batch.cpp)

if(APPLE OR UNIX)
    find_package(PkgConfig)
    pkg_search_module(FREETYPE REQUIRED freetype2)
    set(ASIO_INCLUDE ${CMAKE_SOURCE_DIR}/third_party/include/asio)
elseif(WIN32)
    set(ASIO_INCLUDE ${CMAKE_SOURCE_DIR}/third_party/asio/include)
    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/x64/freetype28MT.lib")
        set(FREETYPE_FOUND TRUE)
        set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/freetype/include) # freetype2 doesn't support "installing" on windows
        set(FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/x64/freetype28MT.lib)
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/Win32/freetype28MT.lib")
        set(FREETYPE_FOUND TRUE)
        set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/freetype/include) # freetype2 doesn't support "installing" on windows
        set(FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/Win32/freetype28MT.lib)
    else()
        find_package(Freetype)
    endif()
endif()

if(FREETYPE_FOUND OR ${FREETYPE_FOUND})
    message(STATUS "FreeType Found")
    add_definitions(-DFOUND_FREETYPE)
endif()

if(DYNAMIC_LINKAGE)
    add_library(zapGraphics-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapGraphics-shared
            PRIVATE core
            PRIVATE zapMaths-shared
            PRIVATE zapEngine-shared
            PRIVATE zapRenderer-shared
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${ASIO_INCLUDE}
            PRIVATE ${FREETYPE_INCLUDE_DIRS}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapGraphics-shared PROPERTIES OUTPUT_NAME "zapGraphicsD")
    else()
        set_target_properties(zapGraphics-shared PROPERTIES OUTPUT_NAME "zapGraphics")
    endif()

    add_dependencies(zapGraphics-shared zapRenderer-shared zapEngine-shared zapMaths-shared core)
    target_link_libraries(zapGraphics-shared zapRenderer-shared zapHostGLFW-shared zapEngine-shared zapMaths-shared ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})

    target_compile_definitions(zapGraphics-shared PUBLIC -DASIO_STANDALONE)
    install(TARGETS zapGraphics-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)

if(WIN32)
    GENERATE_EXPORT_HEADER(zapGraphics-shared
            BASE_NAME zapGraphics
            EXPORT_MACRO_NAME ZAPGRAPHICS_EXPORT
            EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/graphics_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
    target_compile_definitions(zapGraphics-shared PUBLIC -DGRAPHICS_EXPORT="graphics_exports.h")
    set_target_properties(zapGraphics-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    install(FILES "${CMAKE_BINARY_DIR}/exports/graphics_exports.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/graphics/${dir})
endif()

endif()

if(STATIC_LINKAGE)
    add_library(zapGraphics-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapGraphics-static
            PRIVATE core
            PRIVATE zapMaths-static
            PRIVATE zapEngine-static
            PRIVATE zapRenderer-static
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${ASIO_INCLUDE}
            PRIVATE ${FREETYPE_INCLUDE_DIRS}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapGraphics-static PROPERTIES OUTPUT_NAME "zapGraphicsD")
    else()
        set_target_properties(zapGraphics-static PROPERTIES OUTPUT_NAME "zapGraphics")
    endif()

    add_dependencies(zapGraphics-static zapRenderer-static zapEngine-static zapMaths-static core)

    set_target_properties(zapGraphics-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapGraphics-static zapRenderer-static zapHostGLFW-static zapEngine-static zapMaths-static ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})
    target_compile_definitions(zapGraphics-static PUBLIC -DASIO_STANDALONE)

    install(TARGETS zapGraphics-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)

if(WIN32)
    GENERATE_EXPORT_HEADER(zapGraphics-static
            BASE_NAME zapGraphics
            EXPORT_MACRO_NAME ZAPGRAPHICS_EXPORT
            EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/graphics_exports_s.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
    target_compile_definitions(zapGraphics-static PUBLIC -DGRAPHICS_EXPORT="graphics_exports_s.h")
    install(FILES "${CMAKE_BINARY_DIR}/exports/graphics_exports_s.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/graphics/${dir})
endif()

endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/graphics/${dir})
endforeach()
