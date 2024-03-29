cmake_minimum_required(VERSION 3.7)

project(Deppy VERSION 1.0.0 LANGUAGES C CXX)

SET(CMAKE_CXX_FLAGS_BENCHMARK "-O3 -flto -march=native -DBENCHMARK")
SET(CMAKE_C_FLAGS_BENCHMARK "-O3 -flto -march=native -DBENCHMARK")

SET(CMAKE_CXX_FLAGS_EXTBENCH "-O3 -flto -march=native -DEXTBENCH")
SET(CMAKE_C_FLAGS_EXTBENCH "-O3 -flto -march=native -DEXTBENCH")

IF(NOT CMAKE_BUILD_TYPE)
   SET(CMAKE_BUILD_TYPE Debug
       CACHE STRING "Choose the type of build: None Debug Release RelWithDebInfo MinSizeRel"
       FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)
message("CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Wno-c++98-compat -Wno-exit-time-destructors -Wno-padded")
message("CMAKE_CXX_FLAGS is ${CMAKE_CXX_FLAGS}")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Weverything")
message("CMAKE_C_FLAGS is ${CMAKE_C_FLAGS}")

include_directories(${PROJECT_SOURCE_DIR} ext/catch/single_include)

enable_testing()

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/examples DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(lib)
add_subdirectory(analyser)
add_subdirectory(tracer)
add_subdirectory(test)
