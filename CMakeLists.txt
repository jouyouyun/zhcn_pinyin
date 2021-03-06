cmake_minimum_required(VERSION 2.8)
set(APP_NAME "zhcn-pinyin-trie")

find_package(PkgConfig)
pkg_check_modules(SQLITE REQUIRED sqlite3)
pkg_check_modules(GIO REQUIRED gio-2.0)
pkg_check_modules(GLIB REQUIRED glib-2.0)

cmake_policy(SET CMP0015 NEW)
AUX_SOURCE_DIRECTORY(src SRC_LIST)
add_executable(${APP_NAME} ${SRC_LIST})

include_directories(
    ${GIO_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
    ${SQLITE_INCLUDE_DIRS}
)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
target_link_libraries(${APP_NAME}
    ${GIO_LIBRARIES}
    ${GLIB_LIBRARIES}
    ${SQLITE_LIBRARIES}
    crypt
)

ADD_SUBDIRECTORY(database)

install(
    PROGRAMS ${PROJECT_BINARY_DIR}/${APP_NAME}
    DESTINATION bin
)
