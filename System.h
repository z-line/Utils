#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <string>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cstddef>

#include <unistd.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>

#include "Logger.h"

class System {
 private:
  /* data */
 public:
  System(){};
  ~System(){};

  enum class IPType { IPv4, IPv6 };

  static bool mySystem(std::string cmd);
  static bool mySystem(std::string cmd, std::string& ret,
                       bool stderr2stdout = true);
  static std::string getAppPath(void);
  static std::string getIP(IPType type, std::string interface);
  static u16 crc16(u8* pdata, size_t len);
  static u32 crc32_calculate(u8* data, u32 data_length);
};

#endif
