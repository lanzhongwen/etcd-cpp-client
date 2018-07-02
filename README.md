# etcd-cpp-client
It's simple etcd cpp client based on gRPC. This client only provides limited service such as register a key with ttl, keepalive and watch changes on a key.

How to build:
git clone https://github.com/lanzhongwen/etcd-cpp-client.git
cd etcd-cpp-client
1. compile proto
cd proto
protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./rpc.proto
protoc -I. --cpp_out=. ./*.proto
2. build
mkdir build
cmake ..
make
