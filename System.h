#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <string>
#include <sstream>
#include <Logger.h>

class System {
 private:
  /* data */
 public:
  System();
  ~System();

  static bool my_system(std::string cmd) {
    int ret = system(cmd.c_str());
    int err = WEXITSTATUS(ret);
    if (err == 0) {
      LOG_I() << "exec: [" << cmd << "]";
    }
    return err == 0;
  }
};

#endif
