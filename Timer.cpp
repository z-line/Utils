#include "Timer.h"

#include <chrono>

#include "Logger.h"

Timer::Timer(std::string name, bool start_immediate,
             std::function<void(void)> handle, int interval)
    : m_name(name), m_interval(interval), m_handle(handle) {
  if (start_immediate) {
    start();
  }
}

Timer::~Timer() { stop(); }

void Timer::start() {
  if (!m_started) {
    m_thread = std::thread(std::bind(&Timer::process, this), nullptr);
  }
}

void Timer::stop() {
  LOG_I() << "Stopping timer";
  m_force_stop = true;
  m_cv.notify_all();
  m_thread.join();
}

void Timer::process() {
  LOG_I() << "Start timer: " << m_name;
  while (!m_force_stop) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_started = true;
    m_handle();
    switch (m_cv.wait_for(lock, std::chrono::milliseconds(m_interval))) {
      case std::cv_status::no_timeout:
        break;
      case std::cv_status::timeout:
        break;
      default:
        LOG_I() << "Wrong condition variable status";
        break;
    }
  }
  LOG_I() << "Stop timer: " << m_name;
}
