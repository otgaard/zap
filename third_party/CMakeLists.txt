set(PUBLIC_HEADERS
        include/stb_image.h
        include/stb_image_write.h
        include/stb_truetype.h)

set(STB_ARCHIVE
        stb/stb.c)

set(SQLITE3_ARCHIVE
        sqlite3/sqlite3.c
        sqlite3/sqlite3.h)

add_library(stb STATIC ${STB_ARCHIVE})
target_include_directories(stb PRIVATE include)
add_library(sqlite3 STATIC ${SQLITE3_ARCHIVE})

install(TARGETS stb
        EXPORT zapTargets
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
        COMPONENT dev)

install(TARGETS sqlite3
        EXPORT zapTargets
        ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
        COMPONENT dev)

foreach(file ${PUBLIC_HEADERS})
    get_filename_component(dir ${file} DIRECTORY)
    install(FILES ${file} DESTINATION ${INSTALL_INCLUDE_DIR}/zap/third_party/stb/${dir})
endforeach()
