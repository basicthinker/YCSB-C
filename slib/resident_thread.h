//
// resident_thread.h
//
// Created by Jinglei Ren on 2/22/15
// Copyright (c) 2015 Jinglei Ren <jinglei@ren.systems>
//

#ifndef RESIDENT_THREAD_H_
#define RESIDENT_THREAD_H_

#include <thread>
#include <future>
#include <functional>
#include <vector>
#include <mutex>
#include <cassert>

namespace slib {

class ResidentThread {
 public:
  ResidentThread();

  template <class Fn, class... Args>
  std::future<typename std::result_of<Fn(Args...)>::type>
  Run(Fn&& fn, Args&&... args);
  void Terminate();

  ~ResidentThread();

 private:
  enum State {
    FREE,
    BUSY,
    STOPPED,
  };

  State state_;
  std::mutex cond_mutex_; // protects state_; used by cond_var_
  std::condition_variable cond_var_;

  std::function<void()> work_;
  std::thread thread_;
};

ResidentThread::ResidentThread() : state_(FREE), thread_([this] {
    while (true) {
      std::unique_lock<std::mutex> lock(this->cond_mutex_);
      this->cond_var_.wait(lock, [this]{ return this->state_ != FREE; });
      if (state_ == STOPPED) return;
      assert(state_ == BUSY);
      work_();
      state_ = FREE;
    }}) {
}

template <class Fn, class... Args>
std::future<typename std::result_of<Fn(Args...)>::type>
ResidentThread::Run(Fn&& fn, Args&&... args) {
  using return_type = typename std::result_of<Fn(Args...)>::type;

  auto task = std::make_shared<std::packaged_task<return_type()>>(
      std::bind(fn, args...));

  std::unique_lock<std::mutex> lock(cond_mutex_);

  assert(state_ == FREE);
  work_ = [task]{ (*task)(); };
  state_ = BUSY;

  cond_var_.notify_one();
  return task->get_future();
}

void ResidentThread::Terminate() {
  std::unique_lock<std::mutex> lock(cond_mutex_);
  state_ = STOPPED;
  lock.unlock();
  cond_var_.notify_all();
  thread_.join();
}

ResidentThread::~ResidentThread() {
  std::unique_lock<std::mutex> lock(cond_mutex_);
  if (state_ == STOPPED) return;
  state_ = STOPPED;
  lock.unlock();
  cond_var_.notify_all();
  thread_.join();
}

} // namespace slib

#endif // RESIDENT_THREAD_H_

