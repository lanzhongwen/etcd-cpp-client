// Author: Zhongwen(Alan) Lan (runbus@qq.com)
// Created: 2018/07/02
#include "etcd/client.h"
#include "etcd/sclient.h"
#include <chrono>
#include <iostream>
#include <string>
#include <thread>

int main()
{
      ////////////////////////////////////For SClient/////////////////////////
      // SClient support register multiple keys and only support register a key once only
      {
            etcd::SClient sclient("127.0.0.1:2379");
            {
                  const std::string key("/lzw/sclient/k");
                  const std::string value("ip:port");
                  int64_t ttl = 3;
                  sclient.SRegister(key, value, ttl);

                  const std::string key1("/lzw/sclient/k1");
                  const std::string value1("ip:port");
                  sclient.SRegister(key1, value1, ttl);
            }
      }

      std::this_thread::sleep_for(std::chrono::seconds(10));

      return 0;
}
