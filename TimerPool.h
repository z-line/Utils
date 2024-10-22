#ifndef __TIMERPOOL_H
#define __TIMERPOOL_H

#include <chrono>
#include <condition_variable>
#include <functional>
#include <set>
#include <string>
#include <thread>

#include "BaseType.h"

class TimerPool {
 public:
  TimerPool(const std::string& name);
  ~TimerPool();

  void start();
  void stop();
  void addItem(const std::function<void(void)>& handle, bool repeat, bool immediate,
               std::chrono::milliseconds interval);

 private:
  struct Item {
    bool repeat;
    std::function<void(void)> handle;
    std::chrono::milliseconds interval;
    std::chrono::steady_clock::time_point next_tick;
  };

  struct ItemComparator {
    bool operator()(const Item& a, const Item& b) const {
      return a.next_tick < b.next_tick;
    }
  };

  std::chrono::milliseconds m_max_item_block_time =
      std::chrono::milliseconds(2);
  std::string m_name;
  bool m_force_stop = false;
  bool m_started = false;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::multiset<Item, ItemComparator> m_item_list;
  std::unique_ptr<std::thread> m_thread;

  void process();
};

#endif
