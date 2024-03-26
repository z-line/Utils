#ifndef __PATHWATCHER_H
#define __PATHWATCHER_H

#include <functional>
#include <string>
#include <thread>

class PathWatcher {
 public:
  PathWatcher(std::string path, std::function<void(void)> handle);
  ~PathWatcher();

 private:
  int m_fd = -1;
  int m_wd = -1;
  std::string m_path = "";

  bool m_stop = false;
  std::thread* m_thread = nullptr;
  std::function<void(void)> m_callback;

  void process_handle(void);
};

#endif
