// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/06
#include "gtest/gtest.h"
#include "etcd/task.h"
#include <chrono>
#include <thread>

namespace etcd
{
class TaskTest : public testing::Test
{
protected:
  TaskTest()
  {
  }
  void SetUp()
  {
  }
  void TearDown()
  {
  }
};

TEST_F(TaskTest, Fn1)
{
  Task *task = new Task();
  task->Start([=]() {
    while (task->IsStop() == false)
    {
      for (int i = 0; i < 100; i++)
      {
        std::cout << "I made it" << std::endl;
      }
      break;
    }
  });
  delete task;
}

TEST_F(TaskTest, Fn2)
{
  Task *task = new Task();
  task->Start([=]() {
    while (task->IsStop() == false)
    {
      std::cout << "I made it" << std::endl;
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  task->Stop();
  delete task;
}

} // namespace etcd
