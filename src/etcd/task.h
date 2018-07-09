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
#include <thread>

namespace etcd {
class Task {
  public:
    Task() : stoppable_(false), thread_(nullptr) {

    }
    ~Task() {
      std::cout << "Start of ~Task()" << std::endl;
      if (thread_ != nullptr && thread_->joinable()) {
      	pthread_t tid = thread_->native_handle();
      	std::cout << "Stop: tid: " << tid << std::endl;
      	pthread_kill(tid, SIGTERM);

	thread_->join();
	std::cout << "To delete thread_" << std::endl;
	delete thread_;
	thread_ = nullptr;
      }
      std::cout << "End of ~Task()" << std::endl;
    }

    void Start(const std::function<void()>& fn) {
      signal(SIGTERM, OnSignalTerm);
      thread_ = new std::thread(fn);
    }

    bool IsStop() {
      return stoppable_.load();
    }

    void Stop() {
      stoppable_.store(true);
    }

    static void OnSignalTerm(int sig_num) {
      std::cout << "On SIGTERM: thread id: " << std::this_thread::get_id() << std::endl;
      pthread_exit(nullptr);
    }
  protected:
    std::atomic<bool> stoppable_;
    std::thread* thread_;
};
} // namespace etcd
#endif //_ETCD_TASK_H_
