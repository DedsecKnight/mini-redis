#include "include/concurrency/thread_pool.h"

#include <functional>
#include <mutex>

namespace lib::concurrency {
void thread_pool::worker() noexcept {
  while (!done_.load()) {
    std::unique_lock lk{task_queue_mut_};
    task_queue_cond_var_.wait(lk, [this]() { return !this->tasks_.empty(); });
    auto f = std::move(tasks_.front());
    tasks_.pop();
    lk.unlock();
    f();
  }
}
thread_pool::~thread_pool() { done_.store(true); }
thread_pool::thread_pool(std::size_t num_threads) : joiner_{workers_} {
  for (int i = 0; i < num_threads; i++) {
    workers_.emplace_back(&thread_pool::worker, this);
  }
}
void thread_pool::schedule_task(std::function<void()>&& task) noexcept {
  {
    std::scoped_lock lk{task_queue_mut_};
    tasks_.push(std::move(task));
  }
  task_queue_cond_var_.notify_all();
}
};  // namespace lib::concurrency