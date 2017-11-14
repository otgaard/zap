set(PUBLIC_HEADERS
        curves/curves.hpp
        curves/hermite.hpp
        geometry/AABB.hpp
        geometry/disc.hpp
        geometry/hull.hpp
        geometry/line.hpp
        geometry/plane.hpp
        geometry/ray.hpp
        geometry/rect.hpp
        geometry/segment.hpp
        geometry/sphere.hpp
        algebra.hpp
        functions.hpp
        io.hpp
        mat2.hpp
        mat3.hpp
        mat4.hpp
        maths.hpp
        matrix.hpp
        quaternion.hpp
        rand_lcg.hpp
        simd.hpp
        transform.hpp
        vec2.hpp
        vec3.hpp
        vec4.hpp
        vector.hpp
        )

set(SOURCE_FILES
        io.cpp
        transform.cpp
        )

if(DYNAMIC_LINKAGE)
    add_library(zapMaths-shared SHARED ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapMaths-shared PRIVATE core)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapMaths-shared PROPERTIES OUTPUT_NAME "zapMathsD")
    else()
        set_target_properties(zapMaths-shared PROPERTIES OUTPUT_NAME "zapMaths")
    endif()

    install(TARGETS zapMaths-shared
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

if(STATIC_LINKAGE)
    add_library(zapMaths-static STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})
    target_include_directories(zapMaths-static PRIVATE core)
    set_target_properties(zapMaths-static PROPERTIES PREFIX "lib")

    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DEBUG")
        set_target_properties(zapMaths-static PROPERTIES OUTPUT_NAME "zapMathsD")
    else()
        set_target_properties(zapMaths-static PROPERTIES OUTPUT_NAME "zapMaths")
    endif()

    install(TARGETS zapMaths-static
            EXPORT zapTargets
            RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
            ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
            COMPONENT dev)
endif()

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/maths/${dir})
endforeach()
