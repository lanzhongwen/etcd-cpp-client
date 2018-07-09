# etcd-cpp-client
It's simple etcd cpp client based on gRPC. This client only provides limited service such as register a key with ttl, keepalive and watch deletion on a key and re-register the key while detecting deletion.

Pre-build:
1. Install boost
https://www.boost.org/doc/libs/1_67_0/more/getting_started/unix-variants.html

2. Install protobuf
https://github.com/google/protobuf/blob/master/src/README.md

$ git clone https://github.com/google/protobuf.git
$ cd protobuf
$ git submodule update --init --recursive
$ ./autogen.sh
To build and install the C++ Protocol Buffer runtime and the Protocol Buffer compiler (protoc) execute the following:

$ ./configure
$ make
$ make check
$ sudo make install
$ sudo ldconfig # refresh shared library cache.

3. Install Protoc for C++
https://github.com/grpc/grpc/blob/release-0_14/INSTALL.md

$ git clone https://github.com/grpc/grpc.git
$ cd grpc
$ git submodule update --init
$ make
$ [sudo] make install

How to build:
$git clone https://github.com/lanzhongwen/etcd-cpp-client.git
$cd etcd-cpp-client
1. compile proto
$cd proto
$protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./rpc.proto
$protoc -I. --cpp_out=. ./*.proto
2. build
$mkdir build
$cmake ..
$make
$make install

Prerequisite:
As it's not enough to time to make mock on etcd, it requires to install etcd before run gtest or example on default port 2379
How to run example
1. install etcd first
2. cd build/src/etcd/example
3. ./example

How to run gtest
1. mkdir build
2. cmake -Dtest=ON ..
3. make
4. make test
5. or: ./runTest
6. or: ./runSClientTest

TODO:
1. To support etcd cluster
2. Change multiple thread model to IO multiplexing or AIO model
3. To support more features
