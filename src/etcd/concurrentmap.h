// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/03
#ifndef _ETCD_CONCURRENTMAP_H_
#define _ETCD_CONCURRENTMAP_H_
#include <etcd/task.h>
#include <boost/thread/shared_mutex.hpp>
#include <string>
#include <unordered_map>

namespace etcd
{

struct Info
{
  Info(int64_t id, Task *lease_task, Task *watch_task) : lease_id_(id),
                                                         watch_task_(watch_task),
                                                         lease_task_(lease_task)
  {
  }
  ~Info()
  {
    delete lease_task_;
    delete watch_task_;
  }
  int64_t lease_id_;
  Task *watch_task_;
  Task *lease_task_;
};

class ConCurrentMap
{
public:
  ConCurrentMap();
  ~ConCurrentMap();
  /**
     * Get value of the key from map
     * @param key for search
     * @return zero when not found otherwise return the value found
     */
  int64_t GetLeaseId(const std::string &key);

  /**
     * Set set the value for the key if key existing it would replace old value with supplied value
     * @param key to set
     * @param lease id to set
     * @return true if updated otherwise false if not existing
     */
  bool Set(const std::string &key, int64_t lease_id);

  /**
     * Insert record
     * @param key to insert
     * @param lease_task for leasing
     * @param watch_task for watching
     * @param lease_id to be watched
     */
  void Insert(const std::string &key, Task *lease_task, Task *watch_task, int64_t lease_id);

  /**
     * Delete record
     * @param key to delete
     */
  void Delete(const std::string &key);

  /**
     * Size size of map
     * @return size of the map i.e. how many records
     */
  size_t Size() const;

private:
  ConCurrentMap(const ConCurrentMap &);
  const ConCurrentMap &operator=(const ConCurrentMap &);
  std::unordered_map<std::string, Info *> map_;
  mutable boost::shared_mutex rw_mu_;
};
} // namespace etcd

#endif // _ETCD_CONCURRENTMAP_H_
