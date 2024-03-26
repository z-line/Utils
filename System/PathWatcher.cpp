#include "PathWatcher.h"

#include <stdexcept>

#include "Logger.h"
#include "sys/inotify.h"
#include "unistd.h"

#define BUFFER_LEN (128 * sizeof(struct inotify_event))

PathWatcher::PathWatcher(std::string path, std::function<void(void)> handle)
    : m_path(path), m_callback(handle) {
  m_fd = inotify_init();
  if (m_fd < 0) {
    throw std::runtime_error("inotify init failed");
  }
  m_wd = inotify_add_watch(m_fd, path.c_str(), IN_MODIFY);
  if (m_wd < 0) {
    throw std::runtime_error("watch failed");
  }
  m_thread =
      new std::thread(std::bind(&PathWatcher::process_handle, this), nullptr);
  LOG_I() << "Watch " << m_path;
}

PathWatcher::~PathWatcher() {
  if (m_wd >= 0) {
    inotify_rm_watch(m_fd, m_wd);
  }
  if (m_fd >= 0) {
    close(m_fd);
  }
  if (m_thread != nullptr) {
    m_stop = true;
    m_thread->join();
  }
  LOG_I() << "Delete watch of " << m_path;
}

void PathWatcher::process_handle(void) {
  u8 buffer[BUFFER_LEN];
  while (!m_stop) {
    int length = read(m_fd, buffer, BUFFER_LEN);
    if (length < 0) {
      break;
    }
    uint i = 0;
    while (i < length / sizeof(struct inotify_event)) {
      struct inotify_event* event = ((struct inotify_event*)buffer) + i;
      if (event->mask & IN_MODIFY) {
        if (m_callback) {
          m_callback();
        } else {
          LOG_W() << "not callback";
        }
      }
      i++;
    }
  }
}
