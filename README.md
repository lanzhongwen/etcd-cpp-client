# etcd-cpp-client
It's simple etcd cpp client based on gRPC. This client only provides limited service such as register a key with ttl, keepalive and watch changes on a key.

How to build:
1. compile proto
cd proto
protoc -I. --grpc_out=. --plugin=protoc-gen-grpc=`which grpc_cpp_plugin` ./rpc.proto
protoc -I. --cpp_out=. ./*.proto
