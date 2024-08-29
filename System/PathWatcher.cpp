#include "PathWatcher.h"

#include <stdexcept>

#include "Logger.h"
#include "sys/inotify.h"
#include "unistd.h"

#define BUFFER_LEN (128 * sizeof(struct inotify_event))

PathWatcher::PathWatcher() {
  m_fd = inotify_init();
  if (m_fd < 0) {
    throw std::runtime_error("inotify init failed");
  }
}

PathWatcher::~PathWatcher() {
  for (const auto& it : m_wd_list) {
    inotify_rm_watch(m_fd, it.first);
  }

  if (m_fd >= 0) {
    close(m_fd);
  }
  if (m_thread != nullptr) {
    m_stop = true;
    m_thread->join();
  }
  LOG_I() << "Destroy PathWatcher ";
}

bool PathWatcher::addPathObserver(std::string path, IPathObserver* observer) {
  auto range = m_observer_list.equal_range(path);
  for (auto it = range.first; it != range.second; ++it) {
    if (it->second == observer) {
      LOG_W() << "Observer have been registered on " << path;
      return false;
    }
  }
  bool found = false;
  for (const auto& it : m_wd_list) {
    if (it.second == path) {
      found = true;
      break;
    }
  }
  if (!found) {
    int wd = inotify_add_watch(m_fd, path.c_str(), IN_MODIFY);
    if (wd < 0) {
      LOG_E() << "Watch failed:" << path;
      return false;
    }
    m_wd_list.emplace(wd, path);
    if (!m_thread) {
      m_thread = new std::thread(std::bind(&PathWatcher::process_handle, this),
                                 nullptr);
    }
  }
  m_observer_list.emplace(path, observer);
  LOG_I() << "Add observer on " << path;
  return true;
}

bool PathWatcher::removePathObserver(std::string path,
                                     IPathObserver* observer) {
  auto range = m_observer_list.equal_range(path);
  if (std::distance(range.first, range.second) == 1) {
    for (const auto& it : m_wd_list) {
      if (it.second == path) {
        m_observer_list.erase(range.first);
        inotify_rm_watch(m_fd, it.first);
        return true;
      }
    }
    LOG_E() << "observer mismatch with wd";
  } else {
    for (auto it = range.first; it != range.second; ++it) {
      if (it->second == observer) {
        m_observer_list.erase(it);
        return true;
      }
    }
  }
  return false;
}

void PathWatcher::process_handle(void) {
  pthread_setname_np(pthread_self(), "PathWatcher");
  u8 buffer[BUFFER_LEN];
  while (!m_stop) {
    int length = read(m_fd, buffer, BUFFER_LEN);
    if (length < 0) {
      continue;
    }
    uint i = 0;
    while (i < length / sizeof(struct inotify_event)) {
      struct inotify_event* event = ((struct inotify_event*)buffer) + i;
      if (event->mask & IN_MODIFY) {
        auto range = m_observer_list.equal_range(m_wd_list[event->wd]);
        for (auto it = range.first; it != range.second; ++it) {
          it->second->pathChanged(m_wd_list[event->wd]);
        }
      }
      i++;
    }
  }
}
