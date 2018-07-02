// Author: Zhongwen Lan(runbus@qq.com)
// Created: 2018/07/02
#ifndef _ETCD_CLIENT_H_
#define _ETCD_CLIENT_H_

#include <string>

namespace etcd {
class Client {
  public:
    Client(const std::string& etcd_addr);
    void Set(const std::string& key, const std::string& value, int64_t lease_id);
    int64_t LeaseGrant(int64_t ttl);
    void KeepAlive(int64_t lease_id);
    void WatchOne(const std::string& key, const std::string& value, int64_t lease_id);
};
}// namespace etcd

#endif// _ETCD_CLIENT_H_
