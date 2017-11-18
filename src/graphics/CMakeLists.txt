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
        colour.hpp
        )

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
        graphics2/curve_input.cpp
        graphics2/g2_types.cpp
        graphics2/plotter2.cpp
        graphics2/quad.cpp
        graphics3/g3_types.cpp
        particle_engine/particle_engine.cpp
        colour.cpp
        )

if(APPLE OR UNIX)
find_package(PkgConfig)
pkg_search_module(FREETYPE REQUIRED freetype2)
elseif(WIN32)
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

set(ASIO_INCLUDE ${CMAKE_SOURCE_DIR}/third_party/asio/include)

if(DYNAMIC_LINKAGE)
    add_library(zapGraphics-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapGraphics-shared
            PRIVATE core
            PRIVATE zapMaths-static
            PRIVATE zapEngine-static
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

    target_link_libraries(zapGraphics-shared zapHostGLFW-shared zapEngine-shared zapMaths-shared ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})
    target_compile_definitions(zapGraphics-shared PUBLIC -DASIO_STANDALONE)
    install(TARGETS zapGraphics-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

if(STATIC_LINKAGE)
    add_library(zapGraphics-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapGraphics-static
            PRIVATE core
            PRIVATE zapMaths-static
            PRIVATE zapEngine-static
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

    set_target_properties(zapGraphics-static PROPERTIES PREFIX "lib")
    target_link_libraries(zapGraphics-static zapHostGLFW-static zapEngine-static zapMaths-static ${GLEW_LIBRARY} ${FREETYPE_LIBRARIES})
    target_compile_definitions(zapGraphics-static PUBLIC -DASIO_STANDALONE)

    install(TARGETS zapGraphics-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/graphics/${dir})
endforeach()
