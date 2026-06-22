cmake_minimum_required(VERSION 3.16)

project(MLModelConsumer
  VERSION 0.1.0
  DESCRIPTION "C++17 bridge consuming ML model outputs into deterministic BIM geometry modifications"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# ---- Library: the inference bridge ----
add_library(mlmodelconsumer
  src/model_output.cpp
  src/file_reader.cpp
  src/local_inference.cpp
  src/validation.cpp
  src/translator.cpp
  src/fallback.cpp
  src/bridge.cpp)

target_include_directories(mlmodelconsumer
  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(MSVC)
  target_compile_options(mlmodelconsumer PRIVATE /W4)
else()
  target_compile_options(mlmodelconsumer PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ---- Smoke test executable (no external framework) ----
enable_testing()

add_executable(test_main tests/test_main.cpp)
target_link_libraries(test_main PRIVATE mlmodelconsumer)

add_test(NAME smoke COMMAND test_main)
