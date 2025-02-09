cmake_minimum_required(VERSION 3.5.0)
project(RayTracing VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out)

file(GLOB_RECURSE SRC_FILES src/*.cpp)

add_executable(RayTracing ${SRC_FILES})

include_directories(src)

# SFML
include(FetchContent)
FetchContent_Declare(SFML
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 3.0.0
    GIT_SHALLOW ON
    EXCLUDE_FROM_ALL
    SYSTEM)
FetchContent_MakeAvailable(SFML)

target_link_libraries(RayTracing PRIVATE SFML::Graphics SFML::Window)