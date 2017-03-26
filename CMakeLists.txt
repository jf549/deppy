cmake_minimum_required(VERSION 3.7)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(Deppy VERSION 1.0.0 LANGUAGES C CXX)

include_directories ("${PROJECT_SOURCE_DIR}")

add_subdirectory(lib)
add_subdirectory(analyser)
add_subdirectory(tracer)
