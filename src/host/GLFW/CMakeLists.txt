set(PUBLIC_HEADERS
        application.hpp
        )

set(SOURCE_FILES
        application.cpp
        )

find_package(glfw3 REQUIRED PATHS ${PROJECT_SOURCE_DIR}/third_party NO_DEFAULT_PATH)
#find_package(glfw3 REQUIRED)

if(glfw3_FOUND)
    message("found")
endif()

if(DYNAMIC_LINKAGE)
    add_library(zapHostGLFW-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapHostGLFW-shared
            PRIVATE core
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)
    target_link_libraries(zapHostGLFW-shared glfw zapEngine-shared zapMaths-shared)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapHostGLFW-shared PROPERTIES OUTPUT_NAME "zapHostGLFWD")
    else()
        set_target_properties(zapHostGLFW-shared PROPERTIES OUTPUT_NAME "zapHostGLFW")
    endif()

    install(TARGETS zapHostGLFW-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

if(STATIC_LINKAGE)
    add_library(zapHostGLFW-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapHostGLFW-static
            PRIVATE core
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)
    target_link_libraries(zapHostGLFW-static glfw zapEngine-static zapMaths-static)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapHostGLFW-static PROPERTIES OUTPUT_NAME "zapHostGLFWD")
    else()
        set_target_properties(zapHostGLFW-static PROPERTIES OUTPUT_NAME "zapHostGLFW")
    endif()

    set_target_properties(zapHostGLFW-static PROPERTIES PREFIX "lib")

    install(TARGETS zapHostGLFW-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/host/GLFW/${dir})
endforeach()
