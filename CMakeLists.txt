
cmake_minimum_required(VERSION 3.19)

project(cccp
        VERSION 0
        DESCRIPTION "C Code Creation Preprocessor"
        HOMEPAGE_URL https://github.com/oitzujoey/engine-1
        LANGUAGES C)

set(CMAKE_BUILD_TYPE Debug)

if(CMAKE_BUILD_TYPE STREQUAL Debug)
	add_compile_options(-std=c99 -pedantic-errors -Werror -Wall -Wstrict-prototypes -DDEBUG -g -O0)
else()
	add_compile_options(-std=c99 -O3)
endif()

add_compile_options(-DLINUX)

add_executable(cccp)

add_subdirectory(src)

target_link_libraries(cccp lua)
