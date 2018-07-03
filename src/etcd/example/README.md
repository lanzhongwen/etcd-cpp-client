g++ example.cpp -I../.. -I../../.. -L../../../build/src/etcd -letcd-cpp-client -std=c++11

export LD_LIBRARY_PATH=.:../../../build/src/etcd:/usr/local/lib

./a.out
