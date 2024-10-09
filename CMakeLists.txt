cmake_minimum_required(VERSION 3.5)
project(catfig C)

# library
file(GLOB_RECURSE CATFIG_SOURCES "catfig/Source/*.c")
file(GLOB_RECURSE CATFIG_HEADERS "catfig/Source/*.h")

add_library(catfig STATIC ${CATFIG_SOURCES} ${CATFIG_HEADERS})
target_include_directories(catfig PUBLIC "catfig/Source")

# test exec
add_executable(catfig-test "test/Source/Main.c")
target_link_libraries(catfig-test PRIVATE catfig)
