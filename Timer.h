#ifndef __TIMER_H
#define __TIMER_H

#include <condition_variable>
#include <functional>
#include <thread>

class Timer {
 public:
  Timer(std::string name, bool start, std::function<void(void)> handle,
        int interval = 1000);
  ~Timer();

  void start();
  void stop();

 private:
  std::string m_name;
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
