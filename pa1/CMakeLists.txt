cmake_minimum_required(VERSION 3.1...3.15)

if(${CMAKE_VERSION} VERSION_LESS 3.12)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

project(AMR
    VERSION 0.0
    DESCRIPTION "A serial C program for simple Adaptive Mesh Refinement (AMR)"
    LANGUAGES C)

set(SRC_DIR "src")
set(INC_DIR "include")
set(SOURCES ${SRC_DIR}/amr.c ${SRC_DIR}/common.c)
set(HEADERS ${INC_DIR}/common.h)
add_executable(amr ${SOURCES} ${HEADERS})

set(C_FLAGS "-O3")
set(DEBUG_FLAGS "-DDEBUG" "-Wall" "-Wextra" "-Wshadow" "-pedantic")
target_compile_options(amr PRIVATE ${C_FLAGS})
target_compile_options(amr PRIVATE "$<$<CONFIG:DEBUG>:${DEBUG_FLAGS}>")
target_include_directories(amr PRIVATE ${INC_DIR})
