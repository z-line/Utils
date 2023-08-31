#include "Timer.h"
#include <chrono>

Timer::Timer(bool start_immediate, std::function<void(void)> handle,
             int interval)
    : m_interval(interval), m_handle(handle) {
  if (start_immediate) {
    start();
  }
}

Timer::~Timer() {
    stop();
}

void Timer::start() {
  if (!m_started) {
    m_thread = std::thread(std::bind(&Timer::process, this), nullptr);
  }
}

void Timer::stop() {
  std::lock_guard<std::mutex> lock(m_mutex);
  m_force_stop = true;
  m_cv.notify_all();
}

void Timer::process() {
  while (!m_force_stop) {
    std::unique_lock<std::mutex> lock(m_mutex);
    m_started = true;
    m_handle();
    if (m_cv.wait_for(lock, std::chrono::milliseconds(m_interval)) ==
        std::cv_status::timeout) {
    }
  }
}
