cmake_minimum_required(VERSION 3.9.6)

## Base C project
project(UGLy
	VERSION 0.3.0
	LANGUAGES C
	DESCRIPTION "Unsafe Generic Library"
)
set(CMAKE_C_FLAGS "-std=c17")

if (NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
endif()

set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/bin)

add_library(ugly STATIC
	include/ugly/core.h
	src/core.c
	include/ugly/list.h
	src/list.c
	include/ugly/stack.h
	src/stack.c
	include/ugly/map.h
	src/map.c
	include/ugly/hash.h
	src/hash.c
	include/ugly/alloc.h
	src/alloc.c
)
target_include_directories(ugly PRIVATE include/ugly)
target_include_directories(ugly INTERFACE include)


## CTest suite
set(CMAKE_C_FLAGS_DEBUG "-g -O0 --coverage")

include(CTest) # includes enable_testing()

add_executable(test_core test/core.c)
target_link_libraries(test_core PUBLIC ugly)
add_test(NAME core COMMAND test_core)

add_executable(test_list test/list.c)
target_link_libraries(test_list PUBLIC ugly)
add_test(NAME list COMMAND test_list)

add_executable(test_stack test/stack.c)
target_link_libraries(test_stack PUBLIC ugly)
add_test(NAME stack COMMAND test_stack)

add_executable(test_map test/map.c)
target_link_libraries(test_map PUBLIC ugly)
add_test(NAME map COMMAND test_map)

add_executable(test_alloc test/alloc.c)
target_link_libraries(test_alloc PUBLIC ugly)
add_test(NAME alloc COMMAND test_alloc)


## Doxygen generation
find_package(Doxygen)
if (DOXYGEN_FOUND)
	set(DOXYGEN_STRIP_FROM_PATH "${CMAKE_SOURCE_DIR}/include/")
	set(DOXYGEN_HIDE_UNDOC_MEMBERS "YES")
	set(DOXYGEN_DISABLE_INDEX "NO")
	set(DOXYGEN_GENERATE_TREEVIEW "YES")
	doxygen_add_docs(docs "${PROJECT_SOURCE_DIR}/include/")
endif(DOXYGEN_FOUND)
