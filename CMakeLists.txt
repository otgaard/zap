cmake_minimum_required(VERSION 3.4)
project(zap)

include(CMakeUtils.cmake)

set(ZAP_MAJOR_VERSION 0)
set(ZAP_MINOR_VERSION 0)
set(ZAP_PATCH_VERSION 0)
set(ZAP_VERSION ${ZAP_MAJOR_VERSION}.${ZAP_MINOR_VERSION}.${ZAP_PATCH_VERSION})

#[[
PROJECT CONFIG
#]]

option(BUILD_MATHS "Build the Maths library" ON)
option(BUILD_ENGINE "Build the OpenGL API Engine" ON)
option(BUILD_GLFW_HOST "Build the GLFW Host library" ON)
option(BUILD_QT_HOST "Build the Qt Host library" ON)
option(BUILD_GRAPHICS "Build the Graphics library" ON)
option(BUILD_RENDERER "Build the Renderering library" ON)
option(BUILD_RASTERISER "Build the Rasteriser library" ON)
option(BUILD_RAYTRACER "Build the Raytracer library" OFF)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TEST "Build tests" OFF)
option(BUILD_DOCUMENTATION "Build doxygen documentation" OFF)

option(VERBOSE_LOGGING "Verbose logging output" ON)

#[[
GLOBAL CONFIG
#]]

set(CMAKE_CXX_STANDARD 14)

#[[
PLATFORM CONFIG
#]]

if(APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_definitions(-Wall -Werror)
elseif(WIN32)
    include(GenerateExportHeader)
    add_definitions(/W3 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    get_windows_version()
elseif(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_definitions(-Wall -Werror)
endif()

#[[
BUILD CONFIG
#]]

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR CMake)
else()
    set(DEF_INSTALL_CMAKE_DIR lib/CMake/zap)
endif()

set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

foreach(p LIB BIN INCLUDE CMAKE)
    set(var INSTALL_${p}_DIR)
    if(NOT IS_ABSOLUTE "${${var}}")
        set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
    endif()
endforeach()

include_directories("${PROJECT_SOURCE_DIR}/src" "${PROJECT_BINARY_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/third_party/lib/cmake)

add_subdirectory(src/core)

# Setup dependencies (results in BUILD options set to ON or OFF based on found packages)
if(BUILD_MATHS)
    message(STATUS "Configuring zapMaths")
    add_subdirectory(src/maths)
endif()

if(BUILD_ENGINE)
    message(STATUS "Configuring zapEngine")
    add_subdirectory(src/engine)
endif()

if(BUILD_GLFW_HOST)
    message(STATUS "Configuring zapHostGLFW")
    add_subdirectory(src/host/GLFW)
endif()

if(BUILD_QT_HOST)
    message(STATUS "Configuring zapHostQt")
endif()

if(BUILD_GRAPHICS)
    message(STATUS "Configuring zapGraphics")
    add_subdirectory(src/graphics)
endif()

if(BUILD_RENDERER)
    message(STATUS "Configuring zapRenderer")
    add_subdirectory(src/renderer)
endif()

if(BUILD_EXAMPLES)
    message(STATUS "Configuring Examples")
    add_subdirectory(src/examples)
endif()

if(BUILD_TEST)
    message(STATUS "Configuring Tests")
endif()

export(TARGETS core zapMaths-static zapMaths-shared
        FILE "${PROJECT_BINARY_DIR}/zapTargets.cmake")

export(PACKAGE zap)

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(zapConfig.cmake.in "${PROJECT_BINARY_DIR}/zapConfig.cmake" @ONLY)
set(CONF_INCLUDE_DIRS "\${ZAP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(zapConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/zapConfig.cmake" @ONLY)
configure_file(zapConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/zapConfigVersion.cmake" @ONLY)

install(FILES
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/zapConfig.cmake"
        "${PROJECT_BINARY_DIR}/zapConfigVersion.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(EXPORT zapTargets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)







#[[
include_directories(
	${CMAKE_SOURCE_DIR}/src
	${CMAKE_SOURCE_DIR}/third_party/include
	${CMAKE_SOURCE_DIR}/third_party/glew/include)

if(WIN32)
    include_directories(${CMAKE_CURRENT_BINARY_DIR} 	# For the Import/Export header generation
            ${CMAKE_SOURCE_DIR}/third_party/asio/include)
endif(WIN32)

link_directories(
	${CMAKE_SOURCE_DIR}/third_party/lib)

if(APPLE)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_definitions(-Wall -Werror -Wno-unused-local-typedef -DGLEW_STATIC -DASIO_STANDALONE)

    # Could not get FreeType2 to import from third_party via FREETYPE_DIR or PATHS using find_package
    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/lib/libfreetype.a")
        add_definitions(-DFOUND_FREETYPE)
        set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/include/freetype2)
        set(FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/lib/libfreetype.dylib)
        message(${FREETYPE_INCLUDE_DIRS})
        message(${FREETYPE_LIBRARIES})
        message("Freetype Exists!")
    else()
        find_package(Freetype)
    endif()

    if(${FOUND_FREETYPE})
        message("Freetype successfully configured")
    endif()
elseif(UNIX)
    set(CMAKE_CXX_STANDARD 14)
    add_definitions(-Wall -Werror -Wno-unused-variable -DGLEW_STATIC -DASIO_STANDALONE)
    #add_compile_options(-msse4.1 -ffast-math)
    add_compile_options(-msse4.1)
    find_package(Freetype)
    if(${FREETYPE_FOUND})
        add_definitions(-DFOUND_FREETYPE)
        message("Freetype Found")
    endif()
elseif(WIN32)
    include(GenerateExportHeader)
    add_definitions(-DGLEW_STATIC -DASIO_STANDALONE -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_STANDARD 14)

    if(EXISTS "${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/x64/freetype28MT.lib")
        add_definitions(-DFOUND_FREETYPE)
        set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/freetype/include) # freetype2 doesn't support "installing" on windows
        set(FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/x64/freetype28MT.lib)
    elseif(EXISTS "${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/Win32/freetype28MT.lib")
        add_definitions(-DFOUND_FREETYPE)
        set(FREETYPE_INCLUDE_DIRS ${CMAKE_SOURCE_DIR}/third_party/freetype/include) # freetype2 doesn't support "installing" on windows
        set(FREETYPE_LIBRARIES ${CMAKE_SOURCE_DIR}/third_party/freetype/objs/vc2010/Win32/freetype28MT.lib)
    else()
        find_package(Freetype)
    endif()

    set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})
endif(APPLE)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions(-DLOGGING_ENABLED -DDEBUG)
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    #add_definitions(-DLOGGING_ENABLED)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

#########################################
# SOURCE PATHS

set(ZAP_ENGINE_PUBLIC_HEADERS
        src/tools/log.hpp
        src/core/core.hpp
        src/engine/shader.hpp
        src/engine/engine.hpp
        src/engine/program.hpp
        src/engine/buffer.hpp
        src/engine/vertex_format.hpp
        src/engine/vertex_buffer.hpp
        src/engine/index_buffer.hpp
        src/engine/pixel_buffer.hpp
        src/engine/vertex_attribute.hpp
        src/engine/mesh.hpp
        src/engine/export.hpp
        src/engine/pixel_format.hpp
        src/core/pod.hpp
        src/core/bitfield.hpp
        src/core/meta.hpp
        src/engine/uniform_buffer.hpp
        src/engine/texture.hpp
        src/engine/uniform_block.hpp
        src/generators/textures/spectral.hpp
        src/engine/framebuffer.hpp
        src/generators/geometry/surface.hpp
        src/generators/geometry/geometry2.hpp
        src/generators/textures/planar.hpp
        src/generators/geometry/geometry3.hpp
        src/renderer/colour.hpp
        src/generators/noise/value_noise.hpp
        src/generators/textures/convolution.hpp
        src/generators/noise/perlin.hpp
        src/generators/noise/noise.hpp
        src/core/enumfield.hpp
        src/renderer/renderer.hpp
        src/engine/pixmap.hpp
        src/renderer/camera.hpp
        src/renderer/material.hpp
        src/renderer/light.hpp
        src/scene_graph/spatial.hpp
        src/scene_graph/node.hpp
        src/scene_graph/visual.hpp
        src/scene_graph/bound.hpp
        src/renderer/style.hpp
        src/engine/render_state.hpp
        src/tools/os.hpp
        src/renderer/renderer_fwd.hpp
        src/engine/sampler.hpp
        src/tools/threadpool.hpp
        src/renderer/render_context.hpp
        src/tools/string.hpp
        src/renderer/render_args.hpp
        src/engine/state_stack.hpp
        src/renderer/shader_builder.hpp
        src/generators/geometry/geometry_traits.hpp)

set(ZAP_ENGINE_FILES
        src/engine/shader.cpp
        src/engine/gl_api.cpp
        src/engine/gl_api.hpp
        src/engine/program.cpp
        src/engine/buffer.cpp
        src/engine/vertex_buffer.cpp
        src/engine/mesh.cpp
        src/engine/texture.cpp
        src/renderer/camera.cpp
        src/engine/framebuffer.cpp
        src/generators/geometry/surface.cpp
        src/renderer/colour.cpp
        src/generators/noise/noise.cpp
        src/renderer/renderer.cpp
        src/tools/os.cpp
        src/engine/sampler.cpp
        src/renderer/render_context.cpp
        src/engine/state_stack.cpp
        src/renderer/shader_builder.cpp)

set(ZAP_GRAPHICS_PUBLIC_HEADERS
        src/graphics2/quad.hpp
        src/graphics2/g2_types.hpp
        src/graphics2/plotter/plotter.hpp
        src/graphics2/curve_input.hpp
        src/effects/particle_engine.hpp
        src/graphics2/plotter/plot_sampler.hpp
        src/graphics3/g3_types.hpp
        src/generators/generator.hpp
        src/generators/pixel_conversion.hpp
        src/graphics2/text/text_batcher.hpp
        src/graphics2/text/text.hpp)

set(ZAP_GRAPHICS_FILES
        src/graphics2/curve_input.cpp
        src/graphics2/quad.cpp
        src/effects/particle_engine.cpp
        src/graphics2/plotter/plotter.cpp
        src/graphics3/g3_types.cpp
        src/graphics2/g2_types.cpp
        src/generators/generator.cpp
        src/generators/pixel_conversion.cpp
        src/graphics2/text/text_batcher.cpp
        src/graphics2/text/text.cpp
        src/graphics2/text/font_manager.cpp
        src/graphics2/text/font_manager.hpp)

set(ZAP_HOST_PUBLIC_HEADERS
        src/host/GLFW/application.hpp)

set(ZAP_HOST_FILES
        src/host/GLFW/application.cpp)

set(ZAP_LOADER_PUBLIC_HEADERS
        src/loader/loader.hpp)

set(ZAP_LOADER_FILES
        src/loader/loader.cpp)

set(ZAP_RAYTRACER_FILES
        src/raytracer/raytracer.cpp
        src/raytracer/raytracer.hpp
        src/raytracer/hit_record.hpp)

set(ZAP_RASTERISER_FILES
        src/rasteriser/canvas.cpp
        src/rasteriser/canvas.hpp)

set(STB_ARCHIVE
        third_party/stb/stb.c)

set(SQLITE3_ARCHIVE
        third_party/sqlite3/sqlite3.c
        third_party/sqlite3/sqlite3.h)

set(ZAP_EXAMPLE_FILES
        src/examples/zap_example/zap_example.cpp
        src/examples/graphic_types.hpp)

set(TRACER_FILES
        src/examples/tracer/tracer.cpp)

set(RASTERISER_FILES
        src/examples/raster/raster.cpp)

set(PART_ENGINE_FILES
        src/examples/particle_engine/particle_engine.cpp)

set(PLOTTER_FILES
        src/examples/plotter/plotter.cpp)

set(PROCGEN1_FILES
        src/examples/procgen1/procgen1.cpp)

set(MODELS_FILES
        src/examples/models/models.cpp)

#########################################
# DEPENDENCIES

# GLEW
if(APPLE)
    set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/libGLEW.a)
elseif(UNIX)
    set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/libGLEW.a)
elseif(WIN32)
    if(CMAKE_CL_64)
        set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/Release/x64/glew32s.lib)
    else()
        set(GLEW_LIB ${CMAKE_SOURCE_DIR}/third_party/glew/lib/Release/Win32/glew32s.lib)
    endif()
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
    add_library(zapGraphics STATIC ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS} ${FREETYPE_INCLUDE_DIRS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer STATIC ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser STATIC ${ZAP_RASTERISER_FILES})
    #add_library(zapLoader STATIC ${ZAP_LOADER_FILES} ${ZAP_LOADER_PUBLIC_HEADERS})
    target_include_directories(zapGraphics PRIVATE ${FREETYPE_INCLUDE_DIRS})
else(CMAKE_STATIC_BUILD)
if(APPLE OR UNIX)
    add_library(zapMaths SHARED ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS})
    add_library(zapEngine SHARED ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS})
    add_library(zapGraphics SHARED ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer SHARED ${ZAP_RAYTRACER_FILES})
    add_library(zapRasteriser SHARED ${ZAP_RASTERISER_FILES})
    add_library(zapLoader SHARED ${ZAP_LOADER_FILES} ${ZAP_LOADER_PUBLIC_HEADERS})
    target_include_directories(zapGraphics PRIVATE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(zapEngine zapMaths ${LOADER_LIBS})
    target_link_libraries(zapHost ${LOADER_LIBS} ${GLFW_LIBS})
    target_link_libraries(zapGraphics zapEngine zapMaths ${LOADER_LIBS} ${FREETYPE_LIBRARIES})
    target_link_libraries(zapRasteriser zapEngine zapMaths ${LOADER_LIBS})
    target_link_libraries(zapLoader zapEngine zapMaths ${LOADER_LIBS} assimp)
else(WIN32)
    add_library(zapMaths SHARED ${ZAP_MATHS_FILES} ${ZAP_MATHS_PUBLIC_HEADERS} maths_exports.h)
    add_library(zapEngine SHARED ${ZAP_ENGINE_FILES} ${ZAP_ENGINE_PUBLIC_HEADERS} engine_exports.h)
    add_library(zapGraphics SHARED ${ZAP_GRAPHICS_FILES} ${ZAP_GRAPHICS_PUBLIC_HEADERS})
    add_library(zapHost STATIC ${ZAP_HOST_FILES} ${ZAP_HOST_PUBLIC_HEADERS})
    add_library(zapRaytracer SHARED ${ZAP_RAYTRACER_FILES} raytracer_exports.h)
    add_library(zapRasteriser SHARED ${ZAP_RASTERISER_FILES} rasteriser_exports.h)
    add_library(zapLoader SHARED ${ZAP_LOADER_FILES} ${ZAP_LOADER_PUBLIC_HEADERS})

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
    target_link_libraries(zapLoader zapEngine zapMaths ${LOADER_LIBS})
endif(APPLE OR UNIX)
endif(CMAKE_STATIC_BUILD)

add_library(stb STATIC ${STB_ARCHIVE})
add_library(sqlite3 STATIC ${SQLITE3_ARCHIVE})

add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example ${FREETYPE_LIBRARIES} zapGraphics zapHost zapEngine zapMaths stb ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(tracer ${TRACER_FILES})
target_link_libraries(tracer zapRaytracer zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(raster ${RASTERISER_FILES})
target_link_libraries(raster zapRasteriser zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(particle_engine ${PART_ENGINE_FILES})
target_link_libraries(particle_engine zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(plotter ${PLOTTER_FILES})
target_link_libraries(plotter zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(frustum src/examples/frustum.cpp)
target_link_libraries(frustum zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(noises src/examples/noises.cpp)
target_link_libraries(noises zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(scene_graph src/examples/scene_graph2.cpp)
target_link_libraries(scene_graph zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(template src/examples/template.cpp)
target_link_libraries(template zapGraphics zapEngine zapMaths zapHost stb ${GLFW_LIBS} ${LOADER_LIBS})

if(APPLE)
    add_executable(procgen1 ${PROCGEN1_FILES})
    target_link_libraries(procgen1 zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

    add_executable(models ${MODELS_FILES})
    target_link_libraries(models zapLoader zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS} assimp)

    add_executable(memory_access_tests src/tests/simd/memory_access_tests.cpp src/tests/simd/functions.hpp)
    target_compile_options(memory_access_tests PUBLIC -O3 -ffast-math -save-temps -mllvm --x86-asm-syntax=intel -fverbose-asm)
elseif(UNIX)
    add_executable(procgen1 ${PROCGEN1_FILES})
    target_link_libraries(procgen1 zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS})

    add_executable(models ${MODELS_FILES})
    target_link_libraries(models zapLoader zapGraphics zapHost zapEngine zapMaths ${GLFW_LIBS} ${LOADER_LIBS} assimp)

    add_executable(memory_access_tests src/tests/simd/memory_access_tests.cpp src/tests/simd/functions.hpp)
    #target_compile_options(memory_access_tests PUBLIC -O3 -ffast-math -save-temps -fverbose-asm)
    target_compile_options(memory_access_tests PUBLIC -save-temps)
    #target_compile_options(memory_access_tests PUBLIC -save-temps -mllvm --x86-asm-syntax=intel -fverbose-asm)
elseif(WIN32)
    add_executable(memory_access_tests src/tests/simd/memory_access_tests.cpp src/tests/simd/functions.hpp)
    target_compile_options(memory_access_tests PUBLIC /FAs)
endif(APPLE)

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
    #install(TARGETS procgen1 DESTINATION bin)
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
#]]