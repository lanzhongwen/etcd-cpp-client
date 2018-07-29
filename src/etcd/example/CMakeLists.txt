include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../..)
include_directories(../../..)

add_executable(example_client_test example_client.cpp)
add_executable(example_sclient_test example_sclient.cpp)
set_property(TARGET example_client_test PROPERTY CXX_STANDARD 11)
set_property(TARGET example_sclient_test PROPERTY CXX_STANDARD 11)

target_link_libraries(example_client_test etcd-cpp-client boost_system boost_thread)
target_link_libraries(example_sclient_test etcd-cpp-client boost_system boost_thread)

add_test(example_client_test example_client_test)
add_test(example_sclient_test example_sclient_test)
