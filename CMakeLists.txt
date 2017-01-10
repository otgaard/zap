cmake_minimum_required(VERSION 3.4)
project(zap)

#########################################
# PROJECT CONFIG

# In the process of changing the windows build over to shared.

if(WIN32)
set(CMAKE_STATIC_BUILD TRUE)
else(WIN32)
set(CMAKE_STATIC_BUILD FALSE)
endif(WIN32)

# Default to third_party lib for dependencies

include_directories(
	${CMAKE_SOURCE_DIR} 
	${CMAKE_SOURCE_DIR}/third_party/include
	${CMAKE_SOURCE_DIR}/third_party/glew/include)

if(WIN32)
include_directories(${CMAKE_CURRENT_BINARY_DIR})	# For the Import/Export header generation
endif(WIN32)

link_directories(
	${CMAKE_SOURCE_DIR}/third_party/lib)

if(APPLE)
set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wall -Werror -DGLEW_STATIC)
elseif(UNIX)
set(CMAKE_CXX_STANDARD 14)
add_definitions(-Wall -Werror -DGLEW_STATIC)
elseif(WIN32)
include(GenerateExportHeader)
add_definitions(-DGLEW_STATIC)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS )
endif(APPLE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
add_definitions(-DLOGGING_ENABLED)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

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
        core/enumfield.hpp
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
        renderer/renderer.cpp
        engine/pixmap.cpp
        renderer/material.cpp
        renderer/light.cpp
        renderer/style.cpp
        renderer/render_state.cpp
        tools/os.cpp
        engine/sampler.cpp)

set(ZAP_GRAPHICS_PUBLIC_HEADERS
        graphics2/quad.hpp
        graphics2/graphics2_types.hpp
        graphics2/plotter/plotter.hpp
        graphics2/curve_input.hpp
        effects/particle_engine.hpp
        graphics2/plotter/plot_sampler.hpp)

set(ZAP_GRAPHICS_FILES
        graphics2/curve_input.cpp
        graphics2/quad.cpp
        effects/particle_engine.cpp
        graphics2/plotter/plotter.cpp)

set(ZAP_HOST_PUBLIC_HEADERS
        host/GLFW/application.hpp)

set(ZAP_HOST_FILES
        host/GLFW/application.cpp)

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
        examples/zap_example/zap_example.cpp
        examples/graphic_types.hpp)

set(TRACER_FILES
        examples/tracer/tracer.cpp)

set(RASTERISER_FILES
        examples/raster/raster.cpp)

set(PART_ENGINE_FILES
        examples/particle_engine/particle_engine.cpp)

set(PLOTTER_FILES
        examples/plotter/plotter.cpp)

set(PROCGEN1_FILES
        examples/procgen1/procgen1.cpp)

#########################################
# DEPENDENCIES

# GLEW
if(APPLE)
set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/libGLEW.a)
elseif(UNIX)
set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/libGLEW.a)
elseif(WIN32)
set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/Release/x64/glew32s.lib)
endif(APPLE)

if(APPLE)
    find_library(COCOA Cocoa REQUIRED)
    find_library(IOKIT IOKit)
    find_library(CARBON Carbon)
    find_library(COREVIDEO CoreVideo)
    find_library(QUARTZ QuartzCore)
    find_library(OPENGL OpenGL)
    set(GLFW_LIBS glfw3 ${COCOA} ${IOKIT} ${CARBON} ${COREVIDEO} ${QUARTZ} ${OPENGL})
    set(LOADER_LIBS ${GLEW_LIB} ${OPENGL})
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
    set(GLFW_LIBS glfw3 ${OPENGL} ${M} ${DYNLINK} ${XINERAMA} ${XRANDR} ${XI} ${XCURSOR} ${X11} ${XXF86VM} ${PTHREAD})
    set(LOADER_LIBS ${GLEW_LIB})
elseif(WIN32)
    set(GLFW_LIBS glfw3.lib opengl32.lib ${GLEW_LIB})
    set(LOADER_LIBS )
endif(APPLE)

#########################################
# TARGETS & LINKAGE

set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

if(CMAKE_STATIC_BUILD)
    add_library(zapMaths STATIC ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS})
    add_library(zapEngine STATIC ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS})
    add_library(zapGraphics STATIC ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer STATIC ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser STATIC ${ZAP_RASTERISER_FILES})
else(CMAKE_STATIC_BUILD)
if(APPLE OR UNIX)
    add_library(zapMaths SHARED ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS})
    add_library(zapEngine SHARED ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS})
    add_library(zapGraphics SHARED ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer SHARED ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser SHARED ${ZAP_RASTERISER_FILES})
    target_link_libraries(zapEngine zapMaths ${LOADER_LIBS})
    target_link_libraries(zapHost ${LOADER_LIBS} ${GLFW_LIBS})
    target_link_libraries(zapGraphics zapEngine zapMaths ${LOADER_LIBS})
    target_link_libraries(zapRasteriser zapEngine zapMaths ${LOADER_LIBS})
else(WIN32)
    add_library(zapMaths SHARED ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS} maths_exports.h)
    add_library(zapEngine SHARED ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS} engine_exports.h)
    add_library(zapGraphics SHARED ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer SHARED ${ZAP_RAYTRACER_FILES} raytracer_exports.h)
    add_library(zapRasteriser SHARED ${ZAP_RASTERISER_FILES} rasteriser_exports.h)

	GENERATE_EXPORT_HEADER(zapMaths
        BASE_NAME zapMaths
        EXPORT_MACRO_NAME ZAPMATHS_EXPORT
        EXPORT_FILE_NAME maths_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

	GENERATE_EXPORT_HEADER(zapEngine
        BASE_NAME zapEngine
        EXPORT_MACRO_NAME ZAPENGINE_EXPORT
        EXPORT_FILE_NAME engine_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

	GENERATE_EXPORT_HEADER(zapRaytracer
        BASE_NAME zapRaytracer
        EXPORT_MACRO_NAME ZAPRAYTRACER_EXPORT
        EXPORT_FILE_NAME raytracer_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

	GENERATE_EXPORT_HEADER(zapRasteriser
        BASE_NAME zapRasteriser
        EXPORT_MACRO_NAME ZAPRASTERISER_EXPORT
        EXPORT_FILE_NAME rasteriser_exports.h
        STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)

    target_link_libraries(zapEngine zapMaths ${LOADER_LIBS} ${GLFW_LIBS})
    target_link_libraries(zapGraphics zapEngine zapMaths ${LOADER_LIBS})
    target_link_libraries(zapRasteriser zapEngine zapMaths ${LOADER_LIBS} ${GLFW_LIBS})
endif(APPLE OR UNIX)
endif(CMAKE_STATIC_BUILD)

add_library(stb STATIC ${STB_ARCHIVE})

add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example zapHost zapEngine zapMaths stb ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(tracer ${TRACER_FILES})
target_link_libraries(tracer zapRaytracer zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(raster ${RASTERISER_FILES})
target_link_libraries(raster zapRasteriser zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(particle_engine ${PART_ENGINE_FILES})
target_link_libraries(particle_engine zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(plotter ${PLOTTER_FILES})
target_link_libraries(plotter zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(procgen1 ${PROCGEN1_FILES})
target_link_libraries(procgen1 zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

#########################################
# INSTALL

if(APPLE OR UNIX)
install(TARGETS zapMaths LIBRARY DESTINATION lib)
install(TARGETS zapEngine LIBRARY DESTINATION lib)
install(TARGETS zapGraphics LIBRARY DESTINATION lib)
install(TARGETS zapHost ARCHIVE DESTINATION lib)
install(TARGETS zapRaytracer LIBRARY DESTINATION lib)
install(TARGETS zapRasteriser LIBRARY DESTINATION lib)
install(TARGETS zap_example RUNTIME DESTINATION bin)
install(TARGETS tracer RUNTIME DESTINATION bin)
install(TARGETS raster RUNTIME DESTINATION bin)
install(TARGETS particle_engine RUNTIME DESTINATION bin)
install(TARGETS plotter RUNTIME DESTINATION bin)
install(TARGETS procgen1 RUNTIME DESTINATION bin)
elseif(WIN32)
install(TARGETS zapMaths DESTINATION lib)
install(TARGETS zapEngine DESTINATION lib)
install(TARGETS zapGraphics DESTINATION lib)
install(TARGETS zapHost DESTINATION lib)
install(TARGETS zapRaytracer DESTINATION lib)
install(TARGETS zapRasteriser DESTINATION lib)
install(TARGETS zap_example DESTINATION bin)
install(TARGETS tracer DESTINATION bin)
install(TARGETS raster DESTINATION bin)
install(TARGETS particle_engine DESTINATION bin)
install(TARGETS plotter DESTINATION bin)
install(TARGETS procgen1 DESTINATION bin)
endif(APPLE OR UNIX)

set(ZAP_PUBLIC_HEADERS
        ${ZAP_MATHS_PUBLIC_HEADERS}
        ${ZAP_HOST_PUBLIC_HEADERS}
        ${ZAP_ENGINE_PUBLIC_HEADERS}
        ${ZAP_GRAPHICS_PUBLIC_HEADERS})

foreach(file ${ZAP_PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION include/zap/${dir})
endforeach()
