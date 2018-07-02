// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/02
#include "etcd/client.h"

namespace etcd {
Client::Client(const std::string & etcd_addr) {
}	

void Client::Set(const std::string& key, const std::string& value, int64_t lease_id) {
}

int64_t Client::LeaseGrant(int64_t ttl) {
  return 0;
}

void Client::KeepAlive(int64_t lease_id) {
  
}

void WatchOne(const std::string& key, const std::string& value, int64_t lease_id) {
}

} // namespace etcd
