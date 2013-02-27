cmake_minimum_required(VERSION 2.6)

project(blobserver CXX)

FIND_PACKAGE(Boost 1.50 REQUIRED)

INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

# -Wall -Wextra
add_definitions(-g -gstabs+)

add_executable(main main.cpp)
TARGET_LINK_LIBRARIES(main)
