project(GtkNoteBookExt)
set (CMAKE_BUILD_TYPE Debug)
cmake_minimum_required(VERSION 2.6)
aux_source_directory(. SRC_LIST)
add_library(${PROJECT_NAME} SHARED ${SRC_LIST})
find_package (PkgConfig REQUIRED)
pkg_check_modules (GTK2 REQUIRED gtk+-2.0)
pkg_check_modules (GEANY REQUIRED geany)

# add include directories provided by pkg-config
include_directories(${GTK2_INCLUDE_DIRS} ${GEANY_INCLUDE_DIRS})
# add defines provided by pkg-config
add_definitions(${GEANY_CFLAGS})

link_directories (${GTK2_LIBRARY_DIRS})
add_definitions (${GTK2_CFLAGS_OTHER})
set(CMAKE_MODULE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} ${GTK2_LDFLAGS})
target_link_libraries(${PROJECT_NAME} "-L${GTK2_LIBRARY_DIRS}" ${GTK2_LIBRARIES} )


