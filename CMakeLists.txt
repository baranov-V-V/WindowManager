cmake_minimum_required(VERSION 3.15)

project(GUI)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_STANDARD 99)

set(GUI_SOURCE_DIR  ${PROJECT_SOURCE_DIR}/src)

add_executable(main)

add_subdirectory(third_party)
add_subdirectory(src)