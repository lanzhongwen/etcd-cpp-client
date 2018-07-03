include_directories(SYSTEM ${Boost_INCLUDE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../..)
include_directories(../../..)

add_executable(example_test example.cpp)
set_property(TARGET example_test PROPERTY CXX_STANDARD 11)

target_link_libraries(example_test etcd-cpp-client boost_system boost_thread)

add_test(example_test example_test)
