// Author: Zhongwen Lan(runbus@qq.com)
// Created: 2018/07/02
#ifndef _ETCD_CLIENT_H_
#define _ETCD_CLIENT_H_

#include <string>

#include <boost/thread.hpp>
#include <grpc++/grpc++.h>
#include "proto/rpc.grpc.pb.h"

using etcdserverpb::KV;
using etcdserverpb::Watch;
using etcdserverpb::Lease;

namespace etcd {
class Client {
  public:
    /**
     * Constructor create etcd client with format like "192.168.1.2:2379"
     * @param etcd address include ip address and port
     * TODO: etcd cluster support
     */
    Client(const std::string& etcd_addr);

    /**
     * Set value of a key with specified lease id
     * @param key is the key to be created or modified
     * @param value is the new value to be set
     * @param lease_id is the lease attached to the key
     * @return true if grpc success otherwise false
     */
    bool Set(const std::string& key, const std::string& value, int64_t lease_id);

    /**
     * Get the value of the specified key
     * @param the request key
     * @return return value string if success otherwise empty string
     */
    std::string Get(const std::string& key);

    /**
     * Delete the record of a key
     * @param key is intended to delete
     * @return true if grpc success otherwise false
     */
    bool Delete(const std::string& key);

    /**
     * LeaseGrant request a lease id with ttl interval
     * @param ttl is the living time of the lease id
     * @return lease id if sucess otherwise 0
     */
    int64_t LeaseGrant(int64_t ttl);

    /**
     * KeepAlive maintain health check between client and server with specified lease_id
     * @param lease id to keep alive
     */
    void KeepAlive(int64_t lease_id);

    /**
     * WatchGuard keep watch a key and will re-register key with specified value while detected DELETE happen on the key
     * @param the key request to watch and re-register
     * @param the value to be re-registered with the key
     * @param re-register with the lease id
     */
    void WatchGuard(const std::string& key, const std::string& value, int64_t lease_id);
  private:
    std::unique_ptr<KV::Stub> kv_stub_;
    std::unique_ptr<Watch::Stub> watch_stub_;
    std::unique_ptr<Lease::Stub> lease_stub_;
    std::unique_ptr<boost::thread> lease_thread_;
    std::unique_ptr<boost::thread> watch_thread_;
};
}// namespace etcd

#endif// _ETCD_CLIENT_H_
