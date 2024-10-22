#ifndef __IPATHOBSERVER_H
#define __IPATHOBSERVER_H

#include <string>

class IPathObserver {
 public:
  IPathObserver() = default;
  IPathObserver(const IPathObserver&) = delete;

  virtual ~IPathObserver(){};

  IPathObserver& operator=(const IPathObserver&) = delete;

  virtual void pathChanged(const std::string& path) = 0;

 private:
  /* data */
};

#endif
