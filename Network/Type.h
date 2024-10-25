#ifndef __TYPE_H
#define __TYPE_H

#include <bitset>
#include <cstring>
#include <iomanip>
#include <regex>
#include <sstream>
#include <string>

#include "Logger.h"

namespace Network {
class MAC {
 public:
  MAC(uint8_t *data) { memcpy(m_mac, data, 6); }
  MAC(const std::string &mac_str) {
    std::regex pattern(
        R"(^([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2})$)");
    std::smatch matched;
    if (std::regex_search(mac_str, matched, pattern)) {
      for (uint8_t i = 0; i < 6; i++) {
        m_mac[i] = std::stoul(matched[i + 1], 0, 16);
      }
    } else {
      LOG_E() << "Prase mac string failed";
    }
  }
  MAC() { memset(m_mac, 0, 6); }

  explicit operator std::string() const {
    std::ostringstream macStr;
    macStr << std::hex << std::setfill('0');
    for (int i = 0; i < 6; ++i) {
      macStr << std::setw(2) << static_cast<int>(m_mac[i]);
      if (i != 5) {
        macStr << ":";
      }
    }
    return macStr.str();
  }

 private:
  uint8_t m_mac[6];
};

class Netmask {
 public:
  Netmask(uint8_t cidr) : m_cidr(cidr) {}
  Netmask(const std::string &netmask) {
    std::regex pattern(R"(^(\d+)\.(\d+)\.(\d+)\.(\d+)$)");
    std::smatch matched;
    int cidr = 0;
    if (std::regex_search(netmask, matched, pattern) && matched.size() == 5) {
      for (int i = 1; i < 5; i++) {
        int num = std::stoi(matched[i]);
        cidr += std::bitset<8>(num).count();
      }
    } else {
      cidr = 24;
    }
    m_cidr = cidr;
  }

  uint getCIDR() const { return m_cidr; }

  explicit operator std::string() const {
    uint8_t cidr = m_cidr;
    if (m_cidr > 32) {
      LOG_W() << "cidr " << m_cidr << " is wrong";
      cidr = 24;
    }

    uint32_t mask = (cidr == 0) ? 0 : (~0U << (32 - cidr));

    std::stringstream ss;
    for (int i = 3; i >= 0; --i) {
      ss << ((mask >> (i * 8)) & 0xFF);
      if (i > 0) {
        ss << ".";
      }
    }

    return ss.str();
  }

 private:
  uint8_t m_cidr;
};
}  // namespace Network

#endif
