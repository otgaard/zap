cmake_minimum_required(VERSION 3.4)
project(zap)

#########################################
# PROJECT CONFIG

#set(CMAKE_BUILD_TYPE "Release")
#set(CMAKE_BUILD_TYPE "Debug")

set(CMAKE_STATIC_BUILD FALSE)

# Default to third_party lib for dependencies
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/third_party/include)
link_directories(${CMAKE_SOURCE_DIR}/third_party/lib)

if(APPLE)
set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wall -Werror -DGLEW_STATIC)
elseif(UNIX)
set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wall -Werror)
elseif(WIN32)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS )
endif(APPLE)

#########################################
# SOURCE PATHS

set(ZAP_MATHS_PUBLIC_HEADERS
        maths/vec3.hpp
        maths/maths.hpp
        maths/vec2.hpp
        maths/vec4.hpp
        maths/mat2.hpp
        maths/mat3.hpp
        maths/mat4.hpp
        maths/functions.hpp
        maths/geometry/ray.hpp
        maths/io.hpp
        maths/geometry/disc.hpp
        maths/algebra.hpp
        maths/rand_lcg.hpp
        maths/curves/curves.hpp
        maths/transform.hpp
        maths/curves/hermite.hpp
        maths/matrix.hpp
        maths/vector.hpp
        maths/geometry/plane.hpp
        maths/geometry/sphere.hpp
        maths/geometry/hull.hpp
        maths/geometry/rect.hpp
        maths/geometry/line.hpp
        maths/geometry/segment.hpp
        maths/geometry/AABB.hpp)

set(ZAP_MATHS_FILES
        maths/io.cpp
        maths/transform.cpp)

set(ZAP_ENGINE_PUBLIC_HEADERS
        tools/log.hpp
        core/core.hpp
        engine/shader.hpp
        engine/engine.hpp
        engine/program.hpp
        engine/buffer.hpp
        engine/vertex_format.hpp
        engine/vertex_buffer.hpp
        engine/index_buffer.hpp
        engine/pixel_buffer.hpp
        engine/vertex_attribute.hpp
        engine/mesh.hpp
        engine/export.hpp
        engine/pixel_format.hpp
        core/pod.hpp
        core/bitfield.hpp
        core/meta.hpp
        engine/uniform_buffer.hpp
        engine/texture.hpp
        engine/uniform_block.hpp
        generators/textures/spectral.hpp
        engine/framebuffer.hpp
        generators/geometry/surface.hpp
        generators/geometry/geometry2.hpp
        generators/textures/planar.hpp
        generators/geometry/geometry3.hpp
        renderer/colour.hpp
        generators/noise/value_noise.hpp
        generators/textures/convolution.hpp
        generators/noise/perlin.hpp
        generators/noise/noise.hpp
        graphics2/plotter2.hpp
        graphics2/curve_input.hpp
        core/enumfield.hpp
        graphics2/quad.hpp
        graphics2/graphics2.hpp
        graphics2/quad.hpp
        renderer/renderer.hpp
        engine/pixmap.hpp
        renderer/camera.hpp
        renderer/material.hpp
        renderer/light.hpp
        scene_graph/spatial.hpp
        scene_graph/node.hpp
        scene_graph/visual.hpp
        scene_graph/bounds/bound.hpp
        renderer/style.hpp
        renderer/render_state.hpp
        tools/os.hpp
        renderer/renderer_fwd.hpp
        effects/part_engine.hpp
        engine/sampler.hpp)

set(ZAP_ENGINE_FILES
        engine/shader.cpp
        engine/gl_api.cpp
        engine/gl_api.hpp
        engine/program.cpp
        engine/buffer.cpp
        engine/vertex_buffer.cpp
        engine/mesh.cpp
        engine/texture.cpp
        renderer/camera.cpp
        engine/framebuffer.cpp
        generators/geometry/surface.cpp
        renderer/colour.cpp
        generators/noise/noise.cpp
        graphics2/plotter2.cpp
        graphics2/curve_input.cpp
        graphics2/quad.cpp
        renderer/renderer.cpp
        engine/pixmap.cpp
        graphics2/quad.cpp
        renderer/material.cpp
        renderer/light.cpp
        renderer/style.cpp
        renderer/render_state.cpp
        tools/os.cpp
        effects/particle_engine.cpp
        engine/sampler.cpp)

set(ZAP_RAYTRACER_FILES
        raytracer/raytracer.cpp
        raytracer/raytracer.hpp
        raytracer/hit_record.hpp)

set(ZAP_RASTERISER_FILES
        rasteriser/canvas.cpp
        rasteriser/canvas.hpp)

set(STB_ARCHIVE
        third_party/stb/stb.c)

set(ZAP_EXAMPLE_FILES
        apps/zap_example/zap_example.cpp
        apps/application.cpp
        apps/application.hpp
        apps/graphic_types.hpp
        apps/zap_example/simple_shdr.cpp
        apps/zap_example/simple_shdr.hpp)

set(TRACER_FILES
        apps/application.cpp
        apps/application.hpp
        apps/tracer/tracer.cpp)

set(RASTERISER_FILES
        apps/application.cpp
        apps/application.hpp
        apps/raster/raster.cpp)

set(PART_ENGINE_FILES
        apps/application.cpp
        apps/application.hpp
        apps/particle_engine/part_engine.cpp)

#########################################
# DEPENDENCIES

if(APPLE)
    find_library(COCOA Cocoa REQUIRED)
    find_library(IOKIT IOKit)
    find_library(CARBON Carbon)
    find_library(COREVIDEO CoreVideo)
    find_library(QUARTZ QuartzCore)
    find_library(OPENGL OpenGL)
    set(GLFW_LIBS ${COCOA} ${IOKIT} ${CARBON} ${COREVIDEO} ${QUARTZ} ${OPENGL})
    set(GTEST_LIBS )
    set(LOADER_LIBS GLEW)
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
    set(GLFW_LIBS opengl32.lib ${CMAKE_SOURCE_DIR}/third_party/lib/Release/x64/glew32s.lib)
    set(GTEST_LIBS )
    set(LOADER_LIBS )
endif(APPLE)

#########################################
# TARGETS & LINKAGE

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(CMAKE_STATIC_BUILD)
    add_library(zapMaths STATIC ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS})
    add_library(zapEngine STATIC ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS})
    add_library(zapRaytracer STATIC ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser STATIC ${ZAP_RASTERISER_FILES})
else(CMAKE_STATIC_BUILD)
    add_library(zapMaths SHARED ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS})
    add_library(zapEngine SHARED ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS})
    add_library(zapRaytracer SHARED ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser SHARED ${ZAP_RASTERISER_FILES})
    target_link_libraries(zapEngine zapMaths ${LOADER_LIBS} ${GLFW_LIBS})
    target_link_libraries(zapRasteriser zapEngine zapMaths ${LOADER_LIBS} ${GLFW_LIBS})
endif(CMAKE_STATIC_BUILD)

add_library(stb STATIC ${STB_ARCHIVE})

add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example glfw3 zapEngine zapMaths stb ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(tracer ${TRACER_FILES})
target_link_libraries(tracer glfw3 zapRaytracer zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(raster ${RASTERISER_FILES})
target_link_libraries(raster glfw3 zapRasteriser zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(particle_engine ${PART_ENGINE_FILES})
target_link_libraries(particle_engine glfw3 zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

#########################################
# INSTALL

install(TARGETS zapMaths LIBRARY DESTINATION lib)
install(TARGETS zapEngine LIBRARY DESTINATION lib)
install(TARGETS zapRaytracer LIBRARY DESTINATION lib)
install(TARGETS zapRasteriser LIBRARY DESTINATION lib)
install(TARGETS zap_example RUNTIME DESTINATION bin)
install(TARGETS tracer RUNTIME DESTINATION bin)
install(TARGETS raster RUNTIME DESTINATION bin)
install(TARGETS particle_engine RUNTIME DESTINATION bin)

foreach(file ${ZAP_MATHS_PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/zap/${dir})
endforeach()

foreach(file ${ZAP_ENGINE_PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/zap/${dir})
endforeach()
