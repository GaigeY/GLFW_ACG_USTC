cmake_minimum_required(VERSION 3.17)
project(EXP2)

set(CMAKE_CXX_STANDARD 11)

add_executable(EXP2 main.cpp src/glad.c)

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/include)
link_directories(${PROJECT_SOURCE_DIR}/lib)
target_link_libraries(EXP2 ${PROJECT_SOURCE_DIR}/dll/glfw3.dll)