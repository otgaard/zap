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
option(BUILD_RAYTRACER "Build the Raytracer library" ON)
option(BUILD_EXAMPLES "Build examples" ON)
option(BUILD_TEST "Build tests" OFF)
option(BUILD_DOCUMENTATION "Build doxygen documentation" OFF)

option(VERBOSE_LOGGING "Verbose logging output" ON)
option(STATIC_LINKAGE "Build zap with static linking" ON)
option(DYNAMIC_LINKAGE "Build zap with dynamic linking" ON)

#[[
GLOBAL CONFIG
#]]

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

#[[
PLATFORM CONFIG
#]]

if(APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_definitions(-Wall -Werror -Wno-unused-local-typedef -DASIO_STANDALONE)   # unused-local-typedef caused by third_party/asio

    set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
elseif(WIN32)
    include(GenerateExportHeader)
    add_definitions(/W3 -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
    get_windows_version(WINVER)
    add_definitions(-D_WIN32_WINNT=${WINVER})
elseif(UNIX AND NOT APPLE)
    set(CMAKE_POSITION_INDEPENDENT_CODE OFF)
    add_definitions(-Wall -Werror)
endif()

if(MSVC)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /ENTRY:mainCRTStartup")
endif()

# Find OpenGL
find_package(OpenGL REQUIRED)

if(${OPENGL_FOUND})
    message(STATUS "OpenGL was found")
else()
    message(STATUS "OpenGL was not found")
endif()

# GLEW is not automatically configured so manually load it
set(GLEW_INCLUDE ${CMAKE_SOURCE_DIR}/third_party/glew/include)
if(UNIX)
    set(GLEW_LIBRARY ${CMAKE_SOURCE_DIR}/third_party/lib/libGLEW.a)
elseif(WIN32)
    if(CMAKE_CL_64)
        set(GLEW_LIBRARY ${CMAKE_SOURCE_DIR}/third_party/glew/lib/Release/x64/glew32s.lib)
    else()
        set(GLEW_LIBRARY ${CMAKE_SOURCE_DIR}/third_party/glew/lib/Release/Win32/glew32s.lib)
    endif()
endif()

#[[
BUILD CONFIG
#]]

set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH "Installation directory for header files")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin)

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
    add_definitions(-DLOGGING_ENABLED)
endif()

if(WIN32 AND NOT CYGWIN)
    set(DEF_INSTALL_CMAKE_DIR CMake)
    add_compile_options(/wd4251)
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
add_subdirectory(src/tools)
add_subdirectory(third_party)

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

if(BUILD_RASTERISER)
    message(STATUS "Configuring zapCanvas")
    add_subdirectory(src/rasteriser)
endif()

if(BUILD_RAYTRACER)
    message(STATUS "Configuring zapTracer")
    add_subdirectory(src/raytracer)
endif()

if(BUILD_EXAMPLES)
    message(STATUS "Configuring Examples")
    add_subdirectory(src/examples)
endif()

if(BUILD_TEST)
    message(STATUS "Configuring Tests")
endif()

export(TARGETS core FILE "${PROJECT_BINARY_DIR}/zapTargets.cmake")

if(STATIC_LINKAGE)
export(TARGETS zapMaths-static
        zapEngine-static
        zapHostGLFW-static
        zapGraphics-static
        zapRenderer-static
        FILE "${PROJECT_BINARY_DIR}/zapTargets.cmake")
add_definitions(-DZAP_STATIC)
endif()

if(DYNAMIC_LINKAGE)
export(TARGETS zapMaths-shared
        zapEngine-shared
        zapHostGLFW-shared
        zapGraphics-shared
        zapRenderer-shared
        FILE "${PROJECT_BINARY_DIR}/zapTargets.cmake")
add_definitions(-DZAP_DYNAMIC)
endif()

export(PACKAGE zap)

message(${INSTALL_CMAKE_DIR})

file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}" "${INSTALL_INCLUDE_DIR}")
set(CONF_INCLUDE_DIRS ${CMAKE_INSTALL_PREFIX})
configure_file(zapConfig.cmake.in "${PROJECT_BINARY_DIR}/zapConfig.cmake" @ONLY)
#set(CONF_INCLUDE_DIRS "\${ZAP_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(zapConfig.cmake.in "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/zapConfig.cmake" @ONLY)
configure_file(zapConfigVersion.cmake.in "${PROJECT_BINARY_DIR}/zapConfigVersion.cmake" @ONLY)

install(FILES
        "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/zapConfig.cmake"
        "${PROJECT_BINARY_DIR}/zapConfigVersion.cmake"
        DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)

install(EXPORT zapTargets DESTINATION "${INSTALL_CMAKE_DIR}" COMPONENT dev)
