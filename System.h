#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <string>
#include <sstream>
#include <Logger.h>
#include <cstdlib>
#include <cstring>

#include "unistd.h"
#include "arpa/inet.h"
#include "net/if.h"
#include <sys/types.h>
#include <ifaddrs.h>

class System {
 private:
  /* data */
 public:
  System();
  ~System();

  enum class IPType { IPv4, IPv6 };

  static bool mySystem(std::string cmd) {
    int ret = system(cmd.c_str());
    int err = WEXITSTATUS(ret);
    if (err == 0) {
      LOG_I() << "exec: [" << cmd << "]";
    }
    return err == 0;
  }

  static std::string getAppPath(void) {
    char buffer[1024];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));
    if (len == -1) {
      perror("readlink");
    }
    return std::string(buffer);
  }

  static std::string getIP(IPType type, std::string interface) {
    std::string ret = "";
    struct ifaddrs* ifaddr = nullptr;
    getifaddrs(&ifaddr);
    for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
      if (type == IPType::IPv4 && ifa->ifa_addr->sa_family == AF_INET &&
          strcmp(ifa->ifa_name, interface.c_str()) == 0) {
        void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
        char buffer[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, tmpAddrPtr, buffer, sizeof(buffer));
        ret = std::string(buffer);
        break;
      } else if (type == IPType::IPv6 && ifa->ifa_addr->sa_family == AF_INET6 &&
                 strcmp(ifa->ifa_name, interface.c_str()) == 0) {
        void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
        char buffer[INET6_ADDRSTRLEN];
        inet_ntop(AF_INET6, tmpAddrPtr, buffer, sizeof(buffer));
        ret = std::string(buffer);
        break;
      }
    }
    if (ifaddr != nullptr) {
      freeifaddrs(ifaddr);
    }
    return ret;
  }

  static u16 crc16(u8* pdata, size_t len) {
    u32 i;
    u16 crc = 0xFFFF;
    while (len--) {
      crc ^= *pdata++;
      for (i = 0; i < 8; ++i) {
        if (crc & 1) {
          crc = (crc >> 1) ^ 0xA001;
        } else {
          crc = (crc >> 1);
        }
      }
    }
    return crc;
  }

  static u32 crc32_calculate(u8* data, u32 data_length) {
#define Polynomial 0x04c11db7
    u32 crc_value = 0xffffffff;
    u32 xbit = 0x80000000;
    for (u32 i = 0; i < data_length / 4; i++) {
      xbit = 0x80000000;
      for (u32 bits = 0; bits < 32; bits++) {
        if (crc_value & 0x80000000) {
          crc_value <<= 1;
          crc_value ^= Polynomial;
        } else {
          crc_value <<= 1;
        }
        if (((u32*)data)[i] & xbit) {
          crc_value ^= Polynomial;
        }
        xbit >>= 1;
      }
    }
    if (data_length % 4 != 0) {
      u32 buf = 0;
      for (int i = data_length % 4 - 1; i >= 0; i--) {
        buf = buf << 8 | data[data_length - data_length % 4 + i];
      }
      xbit = 0x80000000;
      for (u32 bits = 0; bits < 32; bits++) {
        if (crc_value & 0x80000000) {
          crc_value <<= 1;
          crc_value ^= Polynomial;
        } else {
          crc_value <<= 1;
        }
        if (buf & xbit) {
          crc_value ^= Polynomial;
        }
        xbit >>= 1;
      }
    }
    return crc_value;
  }
};

#endif
