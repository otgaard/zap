cmake_minimum_required(VERSION 3.3e)
project(zap)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/third_party/include)
link_directories(${CMAKE_SOURCE_DIR}/third_party/lib)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/output/bin)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_definitions(-Wall -Werror)

set(ZAP_EXAMPLE_FILES
        zap_example.cpp
        tools/log.hpp
        tools/logger.hpp
        maths/vec.hpp
        maths/vec3.hpp
        maths/maths.hpp)

set(ZAP_TESTS_MATHS_FILES
        tests/maths/vec3_tests.cpp)

if(APPLE)
    find_library(COCOA Cocoa)
    find_library(IOKIT IOKit)
    find_library(CARBON Carbon)
    find_library(COREVIDEO CoreVideo)
    find_library(QUARTZ QuartzCore)
    find_library(OPENGL OpenGL)
endif(APPLE)

if(APPLE)
add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example glfw3 ${COCOA} ${IOKIT} ${CARBON} ${COREVIDEO} ${QUARTZ} ${OPENGL})
add_executable(zap_maths_tests ${ZAP_TESTS_MATHS_FILES})
target_link_libraries(zap_maths_tests gtest)
elseif(UNIX)
add_executable(zap_example ${ZAP_EXAMPLE_FILES})
target_link_libraries(zap_example glfw3 GL m dl Xinerama Xrandr Xi Xcursor X11 Xxf86vm pthread)
add_executable(zap_maths_tests ${ZAP_TESTS_MATHS_FILES})
target_link_libraries(zap_maths_tests gtest pthread)
endif(APPLE)

