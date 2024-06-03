#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <set>
#include <string>

#include "BaseType.h"

namespace System {

namespace Shell {

bool mySystem(std::string cmd);
bool mySystem(std::string cmd, std::string& ret, bool stderr2stdout = true);

}  // namespace Shell
namespace Network {

struct Info {
  std::string name;
  std::string ip;
  std::string netmask;
  std::string gateway;
  std::string mac;
  bool link;

  bool operator<(const Info& value) const { return name < value.name; }
};

enum class IPType { IPv4, IPv6 };

std::set<Info> getIfaceInfo(void);
std::set<std::string> getIFList(void);
std::string getIP(IPType type, std::string iface);

}  // namespace Network
namespace Path {

std::string getAppPath(void);
std::string getAppDir(void);
bool exist(std::string path);

}  // namespace Path

namespace Algorithm {

u16 crc16(u8* data, size_t len);
u32 crc32_calculate(u8* data, u32 data_length);
u32 checksum(u8* data, size_t len);

}  // namespace Algorithm
}  // namespace System

#endif
