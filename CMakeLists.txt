cmake_minimum_required (VERSION 3.1.3 FATAL_ERROR)
project (etcd-cpp-client)

find_package(Boost REQUIRED COMPONENTS system thread)

set (etcd-cpp-client_VERSION_MAJOR 0)
set (etcd-cpp-client_VERSION_MINOR 1)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Werror -std=c++11")

add_subdirectory(src/etcd)
