#ifndef __IPATHOBSERVER_H
#define __IPATHOBSERVER_H

#include <string>

class IPathObserver {
 public:
  IPathObserver(){};
  ~IPathObserver(){};

  virtual void pathChanged(const std::string& path) = 0;

 private:
  /* data */
};

#endif