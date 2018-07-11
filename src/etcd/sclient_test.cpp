// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/05
#include "gtest/gtest.h"
#include "etcd/sclient.h"
#include <chrono>
#include <thread>

namespace etcd {
class SClientTest : public testing::Test {
  protected:
    SClientTest() {
    }
    void SetUp() {
      client_.reset(new SClient("127.0.0.1:2379"));
    }
    void TearDown() {
    }
    std::unique_ptr<SClient> client_;
};

TEST_F(SClientTest, SRegister) {
  // Register first
  {
    const std::string key("/lzw/sclient/sregister");
    const std::string value("127.0.0.1:18075");
    int64_t ttl = 2;
    std::string get_value = client_.get()->Get(key);
    EXPECT_EQ(0, get_value.length());
    client_.get()->SRegister(key, value, ttl);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(1, client_.get()->map_.Size());
    // Delete key
    bool ret = client_.get()->Delete(key);
    EXPECT_TRUE(ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(1, client_.get()->map_.Size());
    // After ttl
    std::this_thread::sleep_for(std::chrono::seconds(2*ttl));

    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(1, client_.get()->map_.Size());
  }

  // Register another one
  {
    const std::string key("/lzw/sclient/sregister/1");
    const std::string value("127.0.0.1:18076");
    int64_t ttl = 2;
    std::string get_value = client_.get()->Get(key);
    EXPECT_EQ(0, get_value.length());
    client_.get()->SRegister(key, value, ttl);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(2, client_.get()->map_.Size());
    // Delete key
    bool ret = client_.get()->Delete(key);
    EXPECT_TRUE(ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(2, client_.get()->map_.Size());
    // After ttl
    std::this_thread::sleep_for(std::chrono::seconds(2*ttl));

    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(2, client_.get()->map_.Size());
  }

  // Register 3rd
  {
    const std::string key("/lzw/sclient/sregister/2");
    const std::string value("127.0.0.1:18077");
    int64_t ttl = 3;
    std::string get_value = client_.get()->Get(key);
    EXPECT_EQ(0, get_value.length());
    client_.get()->SRegister(key, value, ttl);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(3, client_.get()->map_.Size());
    // Delete key
    bool ret = client_.get()->Delete(key);
    EXPECT_TRUE(ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(3, client_.get()->map_.Size());
    // After ttl
    std::this_thread::sleep_for(std::chrono::seconds(2*ttl));

    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(3, client_.get()->map_.Size());
  }

  // Register 4th
  {
    const std::string key("/lzw/sclient/sregister/3");
    const std::string value("127.0.0.1:18078");
    int64_t ttl = 3;
    std::string get_value = client_.get()->Get(key);
    EXPECT_EQ(0, get_value.length());
    client_.get()->SRegister(key, value, ttl);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
    // Delete key
    bool ret = client_.get()->Delete(key);
    EXPECT_TRUE(ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
    // After ttl
    std::this_thread::sleep_for(std::chrono::seconds(2*ttl));

    get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
  }

  // Delete some keys
  {
    const std::string key1("/lzw/sclient/sregister/1");
    const std::string value1("127.0.0.1:18076");
    const std::string key2("/lzw/sclient/sregister/2");
    const std::string value2("127.0.0.1:18077");
    const std::string key3("/lzw/sclient/sregister/3");
    const std::string value3("127.0.0.1:18078");
    bool ret = client_.get()->Delete(key1);
    EXPECT_TRUE(ret);
    ret = client_.get()->Delete(key2);
    EXPECT_TRUE(ret);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    std::string get_value1 = client_.get()->Get(key1);
    EXPECT_EQ(value1, get_value1);
    std::string get_value2 = client_.get()->Get(key2);
    EXPECT_EQ(value2, get_value2);
    EXPECT_EQ(4, client_.get()->map_.Size());
    std::string get_value3 = client_.get()->Get(key3);
    EXPECT_EQ(value3, get_value3);
  }

  // Register same key
  // TODO: It requires to explicit exit or terminate boost::thread otherwise Regsiter same key would cause more than one thread watch one same key. Let do it later.
  {
    const std::string key("/lzw/sclient/sregister/3");
    const std::string value("127.0.0.1:18078");
    const std::string new_value("127.0.0.1:28078");
    int64_t ttl = 3;
    std::string get_value = client_.get()->Get(key);
    EXPECT_EQ(value, get_value);
    std::cout << "Before SRegister" << std::endl;
    client_.get()->SRegister(key, new_value, ttl);
    std::cout << "After SRegister" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    std::cout << "Before Get" << std::endl;
    get_value = client_.get()->Get(key);
    std::cout << "After Get" << std::endl;
    EXPECT_EQ(new_value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
    std::cout << "Before Delete(" << key << ")" << std::endl;
    // Delete key
    bool ret = client_.get()->Delete(key);
    EXPECT_TRUE(ret);
    std::cout << "After Delete(" << key << ")" << std::endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    get_value = client_.get()->Get(key);
    EXPECT_EQ(new_value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
    // After ttl
    std::this_thread::sleep_for(std::chrono::seconds(2*ttl));

    get_value = client_.get()->Get(key);
    EXPECT_EQ(new_value, get_value);
    EXPECT_EQ(4, client_.get()->map_.Size());
  }
}
} // namespace etcd
