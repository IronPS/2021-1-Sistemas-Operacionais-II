cmake_minimum_required(VERSION 3.10)

# Set the project name
project(SISOPII-Etapa1)

# Set C++11 as the required standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
add_compile_options(-Wall -Wextra -O2)


include_directories(code/third_party)


set(INSTALL_DIR ${CMAKE_SOURCE_DIR}/bin)

add_subdirectory(code)
