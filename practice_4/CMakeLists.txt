cmake_minimum_required(VERSION 3.17)
project(C++)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_OSX_ARCHITECTURES arm64)

#find_library(OpenCV)
find_package(OpenCV REQUIRED)
include_directories( ${OpenCV_INCLUDE_DIRS} )

message(STATUS "OpenCV library status:")
message(STATUS "    config: ${OpenCV_DIR}")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

add_executable(C++ main.cpp)

target_link_libraries(C++ ${OpenCV_LIBS})
