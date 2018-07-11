cmake_minimum_required (VERSION 3.1.3 FATAL_ERROR)
project (etcd-cpp-client)

# Options. Turn on with 'cmake -Dtest=ON'
option(test "Build all tests." OFF) # Makes boolean 'test' available.

find_package(Boost REQUIRED COMPONENTS system thread)

set (etcd-cpp-client_VERSION_MAJOR 0)
set (etcd-cpp-client_VERSION_MINOR 1)

include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wpedantic -Werror -std=c++11")

add_subdirectory(src/etcd)
add_subdirectory(src/etcd/example)

##########################################
# Testing
##########################################
# required to install gtest lib first
if (test)

enable_testing()

include_directories(/usr/local/include)
add_executable(runTaskTest src/etcd/task_test.cpp)
add_executable(runTest src/etcd/client_test.cpp)
add_executable(runSClientTest src/etcd/concurrentmap_test.cpp src/etcd/sclient_test.cpp)

target_link_libraries(runTaskTest /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a boost_thread boost_system pthread grpc++)

target_link_libraries(runTest /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a boost_system boost_thread pthread etcd-cpp-client)
target_link_libraries(runSClientTest /usr/local/lib/libgtest.a /usr/local/lib/libgtest_main.a boost_system boost_thread pthread etcd-cpp-client grpc++)

add_test(NAME runTaskTest COMMAND runTaskTest)
add_test(NAME runTest COMMAND runTest)
add_test(NAME runSClientTest COMMAND runSClientTest)

endif()
