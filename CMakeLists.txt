cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project("BTTE" CXX)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Collect all source files (*.h | *.cpp)
set(SRC src)
set(CUSTOM custom)
file(GLOB_RECURSE SRC_FILES "${SRC}/*.cpp", "${CUSTOM}/*.cpp")

# Set compiler flags
# project rely on c++11
add_definitions(-std=c++11)

# debug or release
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")

# (exe, sources)
add_executable(BTTE ${SRC_FILES})
