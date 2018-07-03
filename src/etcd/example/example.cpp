// Author: Zhongwen(Alan) Lan (runbus@qq.com)
// Created: 2018/07/02
#include "etcd/client.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main() {
    etcd::Client client("127.0.0.1:2379");
    const std::string key("/lzw/test/set");
    const std::string value("I'm happy");
    {
      bool ret = client.Set(key, value,0);
      if (ret) {
	std::cout << "Set Success" << std::endl;
      } else {
	std::cout << "Set Failed" << std::endl;
      }
    }

    {
      bool ret = client.Delete(key);
      if (ret) {
	std::cout << "Delete Success" << std::endl;
      } else {
	std::cout << "Delete Failed" << std::endl;
      }
    }

    // Real case: Register service and require to be able to re-register while detecting service deleted
    // 1. Get lease id first
    // 2. Set key/value with lease id
    // 3. KeepAlive with lease id
    // 4. WathGuard key/value with lease id
    {
      int64_t ttl = 10;
      int64_t lease_id = client.LeaseGrant(10);
      if (lease_id == 0) {
	std::cerr << "LeaseGrant Failed" << std::endl;
	return 1;
      } else {
	std::cout << "LeaseGrant Success" << std::endl;
      }
      bool ret = client.Set(key, value, lease_id);
      if (ret) {
	std::cout << "Set Success" << std::endl;
      } else {
	std::cout << "Set Failed" << std::endl;
      }

      client.KeepAlive(lease_id);
      client.WatchGuard(key, value, ttl);
    }

    std::this_thread::sleep_for(std::chrono::seconds(60));

    return 0;
}
