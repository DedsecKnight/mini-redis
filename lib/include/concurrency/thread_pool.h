#pragma once

#include <condition_variable>
#include <functional>
#include <queue>
#include <thread>

namespace lib::concurrency {
class thread_pool {
 private:
  class thread_joiner {
   private:
    std::vector<std::thread>& threads_;

   public:
    explicit thread_joiner(std::vector<std::thread>& threads)
        : threads_(threads) {}
    ~thread_joiner() {
      for (std::size_t i = 0; i < threads_.size(); i++) {
        if (threads_[i].joinable()) {
          threads_[i].join();
        }
      }
    }
  };

 private:
  void worker() noexcept;

 public:
  explicit thread_pool(
      std::size_t num_threads = std::thread::hardware_concurrency());
  void schedule_task(std::function<void()>&& task) noexcept;
  ~thread_pool();

 private:
  std::atomic<bool> done_{false};
  std::mutex task_queue_mut_;
  std::condition_variable task_queue_cond_var_;
  std::queue<std::function<void()>> tasks_;
  std::vector<std::thread> workers_;
  thread_joiner joiner_;
};
}  // namespace lib::concurrency