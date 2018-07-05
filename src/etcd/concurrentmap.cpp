// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/03
#include "etcd/concurrentmap.h"

#include <assert.h>
#include <iostream>

namespace etcd {
ConCurrentMap::ConCurrentMap() {
}

ConCurrentMap::~ConCurrentMap() {
}

int64_t ConCurrentMap::GetLeaseId(const std::string& key) {
  boost::shared_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end()) {
    return search->second.get()->lease_id_;
  }
  return 0;
}

bool ConCurrentMap::Set(const std::string& key, int64_t lease_id) {
  assert(lease_id != 0);
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end()) {
    search->second.get()->lease_id_ = lease_id;
    return true;
  } else {
    std::cerr << "key is non-existing: " << key << std::endl;
    return false;
  }
}

void ConCurrentMap::Delete(const std::string& key) {
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  map_.erase(key);
}

void ConCurrentMap::Insert(const std::string& key, boost::thread* lease_thread, boost::thread* watch_thread, int64_t lease_id) {
  assert(key.length() > 0);
  assert(watch_thread != nullptr);
  assert(lease_id != 0);
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end()) {
    map_.erase(search);
  }
  Info* info_ptr = new Info(lease_id, lease_thread, watch_thread);
  std::unique_ptr<Info> info(info_ptr);

  map_[key] = std::move(info);
}

size_t ConCurrentMap::Size() const {
  return map_.size();
}

} // namespace etcd
