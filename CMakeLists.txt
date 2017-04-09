cmake_minimum_required(VERSION 3.7)

project(Deppy VERSION 1.0.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(${PROJECT_SOURCE_DIR} ext/catch/single_include)

enable_testing()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lib)
add_subdirectory(analyser)
add_subdirectory(tracer)
add_subdirectory(test)
