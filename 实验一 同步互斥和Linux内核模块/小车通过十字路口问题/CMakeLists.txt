cmake_minimum_required(VERSION 2.8.4)
project(CarInCrossing)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -lpthread")

set(SOURCE_FILES main.cpp)
add_executable(CarInCrossing ${SOURCE_FILES})
TARGET_LINK_LIBRARIES(CarInCrossing pthread)