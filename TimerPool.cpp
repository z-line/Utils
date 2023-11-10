#include "TimerPool.h"

#include "Logger.h"

using namespace std;

TimerPool::TimerPool(string name) : m_name(name) { start(); }

TimerPool::~TimerPool() {
  stop();
}

void TimerPool::start() {
  if (m_thread.get() == nullptr) {
    m_thread = unique_ptr<thread>(
        new thread(bind(&TimerPool::process, this), nullptr));
  }
}

void TimerPool::stop() {
  LOG_I() << "Stopping timer";
  m_force_stop = true;
  m_cv.notify_all();
  m_thread->join();
}

void TimerPool::addItem(function<void(void)> handle, bool repeat,
                        bool immediate, chrono::milliseconds interval) {
  Item item = {.repeat = repeat,
               .handle = handle,
               .interval = interval,
               .next_tick = immediate
                                ? chrono::steady_clock::now()
                                : (chrono::steady_clock::now() + interval)};
  m_item_list.insert(item);
  m_cv.notify_all();
}

void TimerPool::process() {
  LOG_I() << "Start timer: " << m_name;
  while (!m_force_stop) {
    unique_lock<mutex> lock(m_mutex);
    m_started = true;
    if (m_item_list.empty()) {
      m_cv.wait(lock);
    } else {
      auto first = m_item_list.begin();
      switch (m_cv.wait_until(lock, first->next_tick)) {
        case cv_status::no_timeout:
        case cv_status::timeout: {
          chrono::steady_clock::time_point start = chrono::steady_clock::now();
          first->handle();
          if (chrono::steady_clock::now() - start > m_max_item_block_time) {
            LOG_W() << "Item block time exceed "
                    << m_max_item_block_time.count() << " ms.";
          }
          if (first->repeat) {
            Item new_item(*first);
            new_item.next_tick = start + new_item.interval;
            m_item_list.insert(new_item);
          }
          m_item_list.erase(first);
          break;
        }
        default:
          LOG_I() << "Wrong condition variable status";
          break;
      }
    }
  }
  LOG_I() << "Stop timer: " << m_name;
}
