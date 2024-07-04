#ifndef __PATHWATCHER_H
#define __PATHWATCHER_H

#include <functional>
#include <map>
#include <string>
#include <thread>

#include "IPathObserver.h"

class PathWatcher {
 public:
  PathWatcher();
  ~PathWatcher();

  bool addPathObserver(std::string path, IPathObserver* observer);
  bool removePathObserver(std::string path, IPathObserver* observer);

 private:
  int m_fd = -1;

  std::multimap<std::string, IPathObserver*> m_observer_list;
  std::unordered_map<int, std::string> m_wd_list;

  bool m_stop = false;
  std::thread* m_thread = nullptr;

  void process_handle(void);
};

#endif
