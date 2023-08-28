#ifndef __TIMER_H
#define __TIMER_H

#include <thread>
#include <functional>
#include <condition_variable>

class Timer {
 public:
  Timer(bool start, std::function<void(void)> handle, int interval = 1000);
  ~Timer();

  void start();
  void stop();

 private:
  bool m_force_stop = false;
  bool m_started = false;
  int m_interval;
  std::mutex m_mutex;
  std::condition_variable m_cv;
  std::function<void(void)> m_handle;
  std::thread m_thread;

  void process();
};

#endif
