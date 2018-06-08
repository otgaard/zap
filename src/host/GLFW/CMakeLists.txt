set(PUBLIC_HEADERS
        application.hpp
        )

set(SOURCE_FILES
        application.cpp
        )

find_package(glfw3 REQUIRED PATHS ${PROJECT_SOURCE_DIR}/third_party NO_DEFAULT_PATH)

if(glfw3_FOUND)
    message(STATUS "GLFW3 Found")
endif()

if(DYNAMIC_LINKAGE)
    add_library(zapHostGLFW-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapHostGLFW-shared
            PRIVATE core
            PRIVATE zapMaths-shared
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)

    target_link_libraries(zapHostGLFW-shared glfw ${GLEW_LIBRARY} zapEngine-shared zapMaths-shared)
    target_compile_definitions(zapHostGLFW-shared PRIVATE -DGLEW_STATIC)

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

if(WIN32)
    GENERATE_EXPORT_HEADER(zapHostGLFW-shared
            BASE_NAME zapHostGLFW
            EXPORT_MACRO_NAME ZAPHOSTGLFW_EXPORT
            EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/hostglfw_exports.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
    target_compile_definitions(zapHostGLFW-shared PUBLIC -DHOSTGLFW_EXPORT="hostglfw_exports.h")
    set_target_properties(zapHostGLFW-shared PROPERTIES LINK_FLAGS "/NODEFAULTLIB:LIBCMT")
    install(FILES "${CMAKE_BINARY_DIR}/exports/hostglfw_exports.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/host/GLFW/${dir})
endif()

endif()

if(STATIC_LINKAGE)
    add_library(zapHostGLFW-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapHostGLFW-static
            PRIVATE core
            PRIVATE zapMaths-static
            PRIVATE ${GLEW_INCLUDE}
            PRIVATE ${CMAKE_BINARY_DIR}/exports
            PRIVATE ${PROJECT_SOURCE_DIR}/third_party/include)

    target_link_libraries(zapHostGLFW-static glfw ${GLEW_LIBRARY} zapEngine-static zapMaths-static)
    target_compile_definitions(zapHostGLFW-static PRIVATE -DGLEW_STATIC)

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

if(WIN32)
    GENERATE_EXPORT_HEADER(zapHostGLFW-static
            BASE_NAME zapHostGLFW
            EXPORT_MACRO_NAME ZAPHOSTGLFW_EXPORT
            EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/exports/hostglfw_exports_s.h
            STATIC_DEFINE SHARED_EXPORTS_BUILT_AS_STATIC)
    target_compile_definitions(zapHostGLFW-static PUBLIC -DHOSTGLFW_EXPORT="hostglfw_exports_s.h")
    install(FILES "${CMAKE_BINARY_DIR}/exports/hostglfw_exports_s.h" DESTINATION ${INSTALL_INCLUDE_DIR}/zap/host/GLFW/${dir})
endif()

endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/host/GLFW/${dir})
endforeach()
