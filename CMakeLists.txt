cmake_minimum_required(VERSION 3.5)
project(zap)

#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_STATIC_BUILD FALSE)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/third_party/include)
link_directories(${CMAKE_SOURCE_DIR}/third_party/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)

if(APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions(-Wall -Werror -DGLEW_STATIC)
elseif(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")
add_definitions(-Wall -Werror)
elseif(WIN32)
set(CMAKE_CXX_FLAGS )
endif(APPLE)

set(ZAP_MATHS_FILES
        maths/vec3.hpp
        maths/maths.hpp
        maths/vec2.hpp
        maths/vec4.hpp
        maths/mat2.hpp
        maths/mat3.hpp
        maths/mat4.hpp
        maths/functions.hpp
        maths/geometry/ray.hpp
        maths/io.cpp
        maths/io.hpp
        maths/geometry/disc.hpp
        maths/algebra.hpp
        maths/rand_lcg.hpp
        maths/curves/curves.hpp
        maths/transform.hpp
        maths/curves/hermite.hpp
        maths/matrix.hpp
        maths/vector.hpp
        maths/transform.cpp
        maths/geometry/plane.hpp
        maths/geometry/sphere.hpp maths/geometry/hull.hpp maths/geometry/rect.hpp)

set(ZAP_ENGINE_FILES
        tools/log.hpp
        core/core.hpp
        engine/shader.hpp
        engine/engine.hpp
        engine/program.hpp
        engine/shader.cpp
        engine/gl_api.cpp
        engine/gl_api.hpp
        engine/program.cpp
        engine/buffer.cpp
        engine/buffer.hpp
        engine/vertex_format.hpp
        engine/vertex_buffer.hpp
        engine/index_buffer.hpp
        engine/pixel_buffer.hpp
        engine/vertex_buffer.cpp
        engine/vertex_attribute.hpp
        engine/mesh.cpp
        engine/mesh.hpp
        engine/export.hpp
        engine/pixel_format.hpp
        core/pod.hpp
        core/bitfield.hpp
        core/meta.hpp
        engine/texture.cpp
        engine/texture.hpp
        engine/uniform_buffer.hpp
        engine/uniform_block.hpp
        generators/textures/spectral.hpp
        renderer/camera.cpp
        renderer/camera.hpp
        engine/framebuffer.cpp
        engine/framebuffer.hpp
        generators/geometry/surface.cpp
        generators/geometry/surface.hpp
        generators/geometry/geometry2.hpp
        generators/textures/planar.hpp
        generators/geometry/geometry3.hpp
        renderer/colour.hpp
        renderer/colour.cpp
        generators/particles/particle_engine.hpp
        generators/particles/particle_engine.cpp
        generators/noise/value_noise.hpp
        generators/noise/noise.cpp
        generators/textures/convolution.hpp
        generators/noise/perlin.hpp
        generators/noise/noise.hpp
        graphics2/plotter2.hpp
        graphics2/plotter2.cpp
        graphics2/curve_input.cpp
        graphics2/curve_input.hpp
        core/enumfield.hpp
        graphics2/quad.cpp
        graphics2/quad.hpp
        graphics2/graphics2.hpp
        renderer/renderer.cpp
        renderer/renderer.hpp
        engine/pixmap.cpp
        engine/pixmap.hpp
        graphics2/quad.cpp
        graphics2/quad.hpp)

set(ZAP_RAYTRACER_FILES
        raytracer/raytracer.cpp
        raytracer/raytracer.hpp)

set(ZAP_RASTERISER_FILES
        rasteriser/canvas.cpp rasteriser/canvas.hpp)

set(STB_ARCHIVE
        third_party/stb/stb.c)

set(ZAP_EXAMPLE_FILES
        apps/zap_example/zap_example.cpp
        apps/application.cpp
        apps/application.hpp
        apps/graphic_types.hpp
        apps/zap_example/simple_shdr.cpp
        apps/zap_example/simple_shdr.hpp)

set(ZAP_COMMAND_LINE
        zap_command.cpp
)

set(ZAP_TESTS_MATHS_FILES
        tests/maths/vector_tests.cpp
        tests/maths/maths_tests.cpp
        tests/maths/matrix_tests.cpp
        tests/maths/mat2_tests.cpp
        tests/maths/mat3_tests.cpp
        tests/maths/mat4_tests.cpp)

set(ZAP_TESTS_ENGINE_FILES
        tests/engine/buffer_format_tests.cpp)

set(SYNESTHESIA_FILES
        apps/synesthesia/fft.cpp
        apps/synesthesia/fft.hpp
        apps/synesthesia/shader_src.hpp
        apps/application.cpp
        apps/application.hpp
        apps/synesthesia/synesthesia.cpp
        apps/synesthesia/modules/module.hpp
        apps/synesthesia/modules/bars.cpp
        apps/synesthesia/modules/bars.hpp
        apps/graphic_types.hpp
        apps/synesthesia/modules/metaballs.cpp
        apps/synesthesia/modules/metaballs.hpp
        apps/synesthesia/spectral_analyser.hpp
        apps/synesthesia/spectral_analyser.cpp
        apps/synesthesia/modules/graphs.cpp
        apps/synesthesia/modules/graphs.hpp
        apps/synesthesia/modules/particles.cpp
        apps/synesthesia/modules/particles.hpp)

set(TRACER_FILES
        apps/application.cpp
        apps/application.hpp
        apps/tracer/tracer.cpp)

set(RASTERISER_FILES
        apps/application.cpp
        apps/application.hpp
        apps/raster/raster.cpp)

if(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(CARBON Carbon)
    find_library(COREVIDEO CoreVideo)
    find_library(QUARTZ QuartzCore)
    find_library(OPENGL OpenGL)
    set(GLFW_LIBS ${COCOA} ${IOKIT} ${CARBON} ${COREVIDEO} ${QUARTZ} ${OPENGL})
    set(GTEST_LIBS )
    set(LOADER_LIBS GLEW)
    set(PORTAUDIO_LIBS )
elseif(UNIX)
    find_library(OPENGL GL)
    find_library(M m)
    find_library(DYNLINK dl)
    find_library(XINERAMA Xinerama)
    find_library(XRANDR Xrandr)
    find_library(XI Xi)
    find_library(XCURSOR Xcursor)
    find_library(X11 X11)
    find_library(XXF86VM Xxf86vm)
    find_library(PTHREAD pthread)
    set(GLFW_LIBS ${OPENGL} ${M} ${DYNLINK} ${XINERAMA} ${XRANDR} ${XI} ${XCURSOR} ${X11} ${XXF86VM} ${PTHREAD})
    set(GTEST_LIBS ${PTHREAD})
    set(LOADER_LIBS libGLEW.a)
elseif(WIN32)
    set(GLFW_LIBS opengl32.lib ${CMAKE_SOURCE_DIR}/third_party/lib/Release/Win32/glew32s.lib)
    set(GTEST_LIBS )
    set(LOADER_LIBS )
endif(APPLE)

if(CMAKE_STATIC_BUILD)
    add_library(ZapMaths STATIC ${ZAP_MATHS_FILES})
    add_library(ZapEngine STATIC ${ZAP_ENGINE_FILES})
    add_library(ZapRaytracer STATIC ${ZAP_RAYTRACER_FILES})
    add_library(ZapRasteriser STATIC ${ZAP_RASTERISER_FILES})
else(CMAKE_STATIC_BUILD)
    add_library(ZapMaths SHARED ${ZAP_MATHS_FILES})
    add_library(ZapEngine SHARED ${ZAP_ENGINE_FILES})
    add_library(ZapRaytracer SHARED ${ZAP_RAYTRACER_FILES})
    add_library(ZapRasteriser SHARED ${ZAP_RASTERISER_FILES})
    target_link_libraries(ZapEngine ${LOADER_LIBS} ${GLFW_LIBS})
    target_link_libraries(ZapRasteriser ZapEngine ${LOADER_LIBS} ${GLFW_LIBS})
endif(CMAKE_STATIC_BUILD)

add_library(stb STATIC ${STB_ARCHIVE})

add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example glfw3 ZapRasteriser ZapEngine ZapMaths stb ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(zap_cmd ${ZAP_COMMAND_LINE})
#target_link_libraries(zap_cmd)

add_executable(zap_maths_tests ${ZAP_TESTS_MATHS_FILES})
target_link_libraries(zap_maths_tests gtest ${GTEST_LIBS})

add_executable(zap_engine_tests ${ZAP_TESTS_ENGINE_FILES})
target_link_libraries(zap_engine_tests gtest ${GTEST_LIBS})

add_executable(synesthesia ${SYNESTHESIA_FILES})
target_include_directories(synesthesia PUBLIC ${CMAKE_SOURCE_DIR}/third_party/simple_mp3)
target_link_libraries(synesthesia glfw3 portaudio ZapEngine mp3lame stb ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(tracer ${TRACER_FILES})
target_link_libraries(tracer glfw3 ZapRaytracer ZapEngine ZapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(raster ${RASTERISER_FILES})
target_link_libraries(raster glfw3 ZapRasteriser ZapEngine ZapMaths ${GLFW_LIBS} ${LOADER_LIBS})
