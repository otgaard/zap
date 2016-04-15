cmake_minimum_required(VERSION 2.8)
project(zap)

#set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE "Debug")

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/third_party/include)
link_directories(${CMAKE_SOURCE_DIR}/third_party/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)

if(APPLE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
add_definitions(-Wall -Werror)
elseif(UNIX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
add_definitions(-Wall -Werror)
elseif(WIN32)
set(CMAKE_CXX_FLAGS )
endif(APPLE)

set(ZAP_EXAMPLE_FILES
        zap_example.cpp
        tools/log.hpp
        maths/vec3.hpp
        maths/maths.hpp
        core/core.hpp
        maths/vec2.hpp
        maths/vec4.hpp
        maths/mat2.hpp
        maths/mat3.hpp
        maths/mat4.hpp
        engine/shader.hpp
        engine/engine.hpp
        engine/program.hpp
        engine/shader.cpp
        engine/gl_api.cpp
        engine/gl_api.hpp
        engine/program.cpp
        engine/buffer.cpp
        engine/buffer.hpp
        engine/buffer_format.hpp
        engine/vertex_buffer.hpp
        engine/index_buffer.hpp
        engine/pixel_buffer.hpp
        engine/vertex_buffer.cpp
        engine/vertex_attribute.hpp
        maths/functions.hpp)

set(ZAP_COMMAND_LINE
        zap_command.cpp)

set(ZAP_TESTS_MATHS_FILES
        tests/maths/vector_tests.cpp
        tests/maths/maths_tests.cpp
        tests/maths/matrix_tests.cpp
        tests/engine/buffer_format_tests.cpp)

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

add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example glfw3 ${GLFW_LIBS} ${LOADER_LIBS})

add_executable(zap_cmd ${ZAP_COMMAND_LINE})
#target_link_libraries(zap_cmd)

add_executable(zap_maths_tests ${ZAP_TESTS_MATHS_FILES})
target_link_libraries(zap_maths_tests gtest ${GTEST_LIBS})
