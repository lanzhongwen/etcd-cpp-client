// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/05
#include "gtest/gtest.h"
#include "etcd/concurrentmap.h"

namespace etcd {
class ConCurrentMapTest : public testing::Test {
  protected:
    ConCurrentMapTest() {
    }

    void SetUp() {
    }
    void TearDown() {
    }
    ConCurrentMap test_map_;
};

TEST_F(ConCurrentMapTest, Insert) {
  // Insert record into empty map
  {
    std::string key = "/lzw/insert";
    int64_t lease_id = 20180705;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    ASSERT_TRUE(test_map_.Size() == 0);
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(1, test_map_.Size());
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
  }
  // Insert same key
  {
    std::string key = "/lzw/insert";
    int64_t lease_id = 20180706;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(1, test_map_.Size());
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
  }

  // Insert another key
  {
    std::string key = "/lzw/insert/1";
    int64_t lease_id = 20180707;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(2, test_map_.Size());
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
  }
}

TEST_F(ConCurrentMapTest, GetLeaseId) {
  // Get existing key
  {
    ASSERT_TRUE(test_map_.Size() == 0);
    std::string key = "/lzw/insert";
    int64_t lease_id = 20180705;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
  }
  // Get nonexisting key
  {
    std::string key = "non-existing";
    EXPECT_EQ(0, test_map_.GetLeaseId(key));
  }
}

TEST_F(ConCurrentMapTest, Set) {
  // Insert first and set existing key
  {
    std::string key = "/lzw/insert";
    int64_t lease_id = 20180705;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
    int64_t new_id = 20180706;
    bool ret = test_map_.Set(key, new_id); 
    EXPECT_TRUE(ret);
    EXPECT_EQ(new_id, test_map_.GetLeaseId(key));
  }
  
  {
    std::string key = "non-existing";
    int64_t new_id = 20180706;
    bool ret = test_map_.Set(key, new_id); 
    EXPECT_FALSE(ret);
  }
}

TEST_F(ConCurrentMapTest, Delete) {
  // Insert first and delete existing key
  {
    std::string key = "/lzw/insert";
    int64_t lease_id = 20180705;
    boost::thread* lease_thread = new boost::thread;
    boost::thread* watch_thread = new boost::thread;
    test_map_.Insert(key, lease_thread, watch_thread, lease_id);
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
    EXPECT_EQ(1, test_map_.Size());
    std::string wrong_key = "/lzw/insert/1";
    test_map_.Delete(wrong_key);
    EXPECT_EQ(lease_id, test_map_.GetLeaseId(key));
    test_map_.Delete(key);
    EXPECT_EQ(0, test_map_.GetLeaseId(key));
    EXPECT_EQ(0, test_map_.Size());
  }
}

} // namespace etcd
