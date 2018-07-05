// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/03
#ifndef _ETCD_CONCURRENTMAP_H_
#define _ETCD_CONCURRENTMAP_H_
#include <boost/thread.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <string>
#include <unordered_map>

namespace etcd {

struct Info {
  Info(int64_t id, boost::thread* lease_thread_ptr, boost::thread* watch_thread_ptr) :
    lease_id_(id),
    lease_thread_(lease_thread_ptr),
    watch_thread_(watch_thread_ptr) {
  }
  int64_t lease_id_;
  std::unique_ptr<boost::thread> lease_thread_;
  std::unique_ptr<boost::thread> watch_thread_;
};

class ConCurrentMap {
  public:
    ConCurrentMap();
    ~ConCurrentMap();
    /**
     * Get value of the key from map
     * @param key for search
     * @return zero when not found otherwise return the value found
     */
    int64_t GetLeaseId(const std::string& key);

    /**
     * Set set the value for the key if key existing it would replace old value with supplied value
     * @param key to set
     * @param lease id to set
     * @return true if updated otherwise false if not existing
     */
    bool Set(const std::string& key, int64_t lease_id);

    /**
     * Insert record
     * @param key to insert
     * @param lease_thread for leasing
     * @param watch_thread for watching
     * @param lease_id to be watched
     */
    void Insert(const std::string& key, boost::thread* lease_thread, boost::thread* watch_thread, int64_t lease_id);
    
    /**
     * Delete record
     * @param key to delete
     */
    void Delete(const std::string& key);

    /**
     * Size size of map
     * @return size of the map i.e. how many records
     */
    size_t Size() const;
  private:
    ConCurrentMap(const ConCurrentMap&);
    const ConCurrentMap& operator=(const ConCurrentMap&);
    std::unordered_map<std::string, std::unique_ptr<Info>> map_;
    mutable boost::shared_mutex rw_mu_;
};
} // namespace etcd

#endif// _ETCD_CONCURRENTMAP_H_
