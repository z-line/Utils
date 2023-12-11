#include "System.h"

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <map>
#include <set>
#include <sstream>

#include "Logger.h"
#if defined(__linux__) || defined(__APPLE__)
#include "arpa/inet.h"
#include "ifaddrs.h"
#include "net/if.h"
#elif _WIN32
#include "windows.h"
#endif
#include "sys/ioctl.h"
#include "sys/types.h"
#include "unistd.h"

using namespace std;

map<string, set<string>> if_list;

bool System::Shell::mySystem(string cmd) {
#if defined(__linux__) || defined(__APPLE__)
  int ret = system(cmd.c_str());
  int err = WEXITSTATUS(ret);
  if (err == 0) {
    LOG_V() << "exec: [" << cmd << "]";
  } else {
    LOG_E() << "exec: [" << cmd << "] fail";
  }
  return err == 0;
#elif defined(_WIN32)
// TODO finish this
#endif
}

bool System::Shell::mySystem(string cmd, string& ret, bool stderr2stdout) {
  FILE* fp = NULL;
  char data[100] = {'0'};
  fp = popen((cmd + (stderr2stdout ? " 2>&1" : "")).c_str(), "r");
  if (fp == NULL) {
    LOG_E() << "popen error!";
    return false;
  }
  ret.clear();
  while (fgets(data, sizeof(data), fp) != NULL) {
    ret += string(data);
  }
  pclose(fp);
  LOG_V() << "exec: [" << cmd << "] ret: " << ret;
  return true;
}

string System::Path::getAppPath(void) {
  char buffer[512];
#if defined(__linux__) || defined(__APPLE__)
  ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));
  if (len == -1) {
    perror("readlink");
  }
#elif defined(_WIN32)
  GetCurrentDirectoryA(sizeof(buffer), buffer);
#endif
  return string(buffer);
}

string System::Path::getAppDir(void) {
  string appPath = getAppPath();
  return appPath.substr(0, appPath.find_last_of('/'));
}

bool System::Path::exist(string path) {
  if (access(path.c_str(), F_OK) == 0) {
    return true;
  }
  return false;
}

vector<string> System::Network::getIFList(void) {
  vector<string> ret;

#if defined(__linux__) || defined(__APPLE__)
  struct ifaddrs* ifaddr = nullptr;
  getifaddrs(&ifaddr);
  char buffer[INET6_ADDRSTRLEN];
  for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    inet_ntop(ifa->ifa_addr->sa_family == AF_INET6 ? AF_INET6 : AF_INET,
              &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr, buffer,
              sizeof(buffer));
    string name(ifa->ifa_name);
    string address(buffer);
    auto target = if_list.find(string(ifa->ifa_name));
    if (target != if_list.end()) {
      target->second.insert(name);
    } else {
      ret.push_back(name);
      if (ifa->ifa_addr->sa_family == AF_INET ||
          ifa->ifa_addr->sa_family == AF_INET6) {
        set<string> buf;
        buf.insert(address);
        if_list.insert({name, buf});
      } else {
        if_list.insert({name, set<string>()});
      }
    }
  }
  if (ifaddr != nullptr) {
    freeifaddrs(ifaddr);
  }
#elif defined(_WIN32)
// TODO finish
#endif

  return ret;
}

string System::Network::getIP(IPType type, string interface) {
  string ret = "";
#if defined(__linux__) || defined(__APPLE__)
  struct ifaddrs* ifaddr = nullptr;
  getifaddrs(&ifaddr);
  for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
    if (type == IPType::IPv4 && ifa->ifa_addr->sa_family == AF_INET &&
        strcmp(ifa->ifa_name, interface.c_str()) == 0) {
      void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
      char buffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, buffer, sizeof(buffer));
      ret = string(buffer);
      break;
    } else if (type == IPType::IPv6 && ifa->ifa_addr->sa_family == AF_INET6 &&
               strcmp(ifa->ifa_name, interface.c_str()) == 0) {
      void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
      char buffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, buffer, sizeof(buffer));
      ret = string(buffer);
      break;
    }
  }
  if (ifaddr != nullptr) {
    freeifaddrs(ifaddr);
  }
#elif defined(_WIN32)
// TODO finish this
#endif
  return ret;
}

void System::Network::setIP(std::string ifname, std::string ipv4) {
  int fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
  struct ifreq if_req;
  uint ip[4];
  if (fd < 0) {
    LOG_E() << "socket failed";
    goto exit;
  }

  memset(&if_req, 0, sizeof(if_req));
  strncpy(if_req.ifr_ifrn.ifrn_name, ifname.c_str(),
          sizeof(if_req.ifr_ifrn.ifrn_name));

  sscanf(ipv4.c_str(), "%u.%u.%u.%u", &ip[0], &ip[1], &ip[2], &ip[3]);

  if_req.ifr_ifru.ifru_addr.sa_family = AF_INET;
  memcpy(&((struct sockaddr_in*)&if_req.ifr_ifru.ifru_addr)->sin_addr.s_addr,
         ip, sizeof(ip));

  if (ioctl(fd, SIOCSIFADDR, &if_req) < 0) {
    LOG_E() << "ioctl failed";
    goto exit;
  }

  return;
exit:
  if (fd >= 0) {
    close(fd);
  }
}

u16 System::Algorithm::crc16(u8* data, size_t len) {
  u32 i;
  u16 crc = 0xFFFF;
  while (len--) {
    crc ^= *data++;
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

u32 System::Algorithm::crc32_calculate(u8* data, u32 data_length) {
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

u32 System::Algorithm::checksum(u8* data, size_t len) {
  u32 checksum = 0;
  for (size_t i = 0; i < len; i++) {
    checksum += data[i];
  }
  return checksum;
}
