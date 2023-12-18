#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <string>
#include <vector>

#include "BaseType.h"

namespace System {

namespace Shell {

bool mySystem(std::string cmd);
bool mySystem(std::string cmd, std::string& ret, bool stderr2stdout = true);

}  // namespace Shell
namespace Network {

enum class IPType { IPv4, IPv6 };

std::vector<std::string> getIFList(void);
std::string getIP(IPType type, std::string iface);
void setIP(std::string ifname, std::string ipv4);

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
