#pragma once

#include <mutex>
#include <queue>

namespace PB {
template <typename T> class TSQueue {
public:
  // Adds an element to the queue.
  void enqueue(T item)
  {
    std::lock_guard<std::mutex> lock(mtx);
    q.push(std::move(item));
    cv.notify_one(); // Notify one waiting thread, if any.
  }

  // Removes and returns an element from the queue.
  // If the queue is empty, the calling thread is blocked until an element is
  // available.
  T dequeue()
  {
    std::unique_lock<std::mutex> lock(mtx);
    cv.wait(lock, [this] {
      return !q.empty();
    }); // Wait until the queue is not empty.
    T item = std::move(q.front());
    q.pop();
    return item;
  }

  // Returns true if the queue is empty.
  bool empty() const
  {
    std::lock_guard<std::mutex> lock(mtx);
    return q.empty();
  }

  // Returns the size of the queue.
  size_t size() const
  {
    std::lock_guard<std::mutex> lock(mtx);
    return q.size();
  }

private:
  std::queue<T>           q;
  mutable std::mutex      mtx;
  std::condition_variable cv;
};
} // namespace PB
