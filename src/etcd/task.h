// Author: Zhongwen(Alan) Lan(runbus@qq.com)
// Created: 2018/07/06
#ifndef _ETCD_TASK_H_
#define _ETCD_TASK_H_
#include <assert.h>
#include <atomic>
#include <functional>
#include <iostream>
#include <pthread.h>
#include <signal.h>
#include <boost/thread.hpp>
#include <thread>

#include "proto/rpc.grpc.pb.h"

using grpc::ClientReaderWriter;

using etcdserverpb::LeaseKeepAliveRequest;
using etcdserverpb::LeaseKeepAliveResponse;
using etcdserverpb::WatchRequest;
using etcdserverpb::WatchResponse;

namespace etcd
{
class Task
{
public:
  Task() : stoppable_(false), thread_(nullptr), watch_stream_(nullptr), keep_stream_(nullptr)
  {
  }
  ~Task()
  {
    std::cout << "Start of ~Task()" << std::endl;
    if (thread_ != nullptr && thread_->joinable())
    {
      pthread_t tid = thread_->native_handle();
      std::cout << "Stop: tid: " << tid << std::endl;
      Stop();
      //pthread_kill(tid, SIGTERM);

      std::cout << "Before join()" << std::endl;
      thread_->join();
      std::cout << "To delete thread_" << std::endl;
      delete thread_;
      thread_ = nullptr;
    }
    keep_stream_ = nullptr;
    watch_stream_ = nullptr;
    std::cout << "End of ~Task()" << std::endl;
  }

  void Start(const std::function<void()> &fn)
  {
    signal(SIGTERM, OnSignalTerm);
    thread_ = new boost::thread(fn);
  }

  bool IsStop()
  {
    return stoppable_.load();
  }

  void Stop()
  {
    if (!stoppable_.load())
    {
      stoppable_.store(true);
      if (keep_stream_ != nullptr)
      {
        std::cout << "Calling keep stream::WritesDone()" << std::endl;
        keep_stream_->WritesDone();
      }
      if (watch_stream_ != nullptr)
      {
        std::cout << "Calling watch stream::WritesDone()" << std::endl;
        watch_stream_->WritesDone();
      }
      std::cout << "task this address: " << (int64_t)this << std::endl;
      std::cout << "Task::Stoppable: " << stoppable_ << std::endl;
      std::cout << "Task::Stoppable address: " << (int64_t)&stoppable_ << std::endl;
    }
  }

  static void OnSignalTerm(int sig_num)
  {
    std::cout << "On SIGTERM: thread id: " << std::this_thread::get_id() << std::endl;
    pthread_exit(nullptr);
  }

  void SetWatchStream(ClientReaderWriter<WatchRequest, WatchResponse> *watch_stream)
  {
    watch_stream_ = watch_stream;
  }
  void SetKeepStream(ClientReaderWriter<LeaseKeepAliveRequest, LeaseKeepAliveResponse> *keep_stream)
  {
    keep_stream_ = keep_stream;
  }

protected:
  std::atomic<bool> stoppable_;
  boost::thread *thread_;
  // Not Owned
  ClientReaderWriter<WatchRequest, WatchResponse> *watch_stream_;
  ClientReaderWriter<LeaseKeepAliveRequest, LeaseKeepAliveResponse> *keep_stream_;
};
} // namespace etcd
#endif //_ETCD_TASK_H_
