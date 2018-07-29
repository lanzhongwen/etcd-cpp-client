// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/03
#include "etcd/concurrentmap.h"

#include <assert.h>
#include <iostream>

namespace etcd
{
ConCurrentMap::ConCurrentMap()
{
}

ConCurrentMap::~ConCurrentMap()
{
  while(map_.size()) {
    auto it = map_.begin();
    Delete(it->first);
  }
}

int64_t ConCurrentMap::GetLeaseId(const std::string &key)
{
  boost::shared_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end())
  {
    return search->second->lease_id_;
  }
  return 0;
}

bool ConCurrentMap::Set(const std::string &key, int64_t lease_id)
{
  assert(lease_id != 0);
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end())
  {
    search->second->lease_id_ = lease_id;
    return true;
  }
  else
  {
    std::cerr << "key is non-existing: " << key << std::endl;
    return false;
  }
}

void ConCurrentMap::Delete(const std::string &key)
{
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end())
  {
    Info *info = search->second;
    map_.erase(search);
    info->lease_task_->Stop();
    info->watch_task_->Stop();
    delete info;
  }
}

void ConCurrentMap::Insert(const std::string &key, Task *lease_task, Task *watch_task, int64_t lease_id)
{
  assert(key.length() > 0);
  assert(lease_task != nullptr);
  assert(watch_task != nullptr);
  assert(lease_id != 0);
  boost::unique_lock<boost::shared_mutex> lock(rw_mu_);
  auto search = map_.find(key);
  if (search != map_.end())
  {
    Info *info = search->second;
    map_.erase(search);
    info->watch_task_->Stop();
    info->lease_task_->Stop();
    delete info;
  }
  Info *info_ptr = new Info(lease_id, lease_task, watch_task);

  map_[key] = info_ptr;
}

size_t ConCurrentMap::Size() const
{
  return map_.size();
}

} // namespace etcd
