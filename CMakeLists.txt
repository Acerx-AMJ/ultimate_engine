cmake_minimum_required(VERSION 3.10)
project(ultimate)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
add_compile_options(-Wall)

file(GLOB SOURCES ${PROJECT_SOURCE_DIR}/source/*.cpp)
include_directories(${PROJECT_SOURCE_DIR}/include)
add_executable(${PROJECT_NAME} ${SOURCES})

find_package(raylib REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE raylib)
