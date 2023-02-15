
cmake_minimum_required(VERSION 3.15)

# Project name and a few useful settings. Other commands can pick up the results
project(
    sky
    VERSION 0.1
    DESCRIPTION "An example project with CMake"
    LANGUAGES CXX)

find_package(OpenCV REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp")

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_20)

target_link_libraries(${PROJECT_NAME} PUBLIC ${OpenCV_LIBS})
