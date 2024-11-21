#ifndef __TYPE_H
#define __TYPE_H

#include <QRegularExpression>
#include <bitset>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

#include "Logger.h"

namespace Network {
class MAC {
 public:
  MAC(uint8_t *data) { memcpy(m_mac, data, 6); }
  MAC(const std::string &mac_str) {
    QRegularExpressionMatch matched =
        m_pattern.match(QString::fromStdString(mac_str));
    if (matched.hasMatch()) {
      for (uint8_t i = 0; i < 6; i++) {
        m_mac[i] = std::stoul(matched.captured(i + 1).toStdString(), 0, 16);
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
  static inline const QRegularExpression m_pattern{
      "(^([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{"
      "0,2}):([0-9a-fA-F]{0,2}):([0-9a-fA-F]{0,2})$)"};
  uint8_t m_mac[6];
};

class Netmask {
 public:
  Netmask(uint8_t cidr) : m_cidr(cidr) {}
  Netmask(const std::string &netmask) {
    QRegularExpressionMatch matched =
        m_pattern.match(QString::fromStdString(netmask));
    int cidr = 0;
    if (matched.hasMatch() && matched.capturedLength() == 5) {
      for (int i = 1; i < 5; i++) {
        int num = std::stoi(matched.captured(i).toStdString());
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
  static inline const QRegularExpression m_pattern{
      R"(^(\d+)\.(\d+)\.(\d+)\.(\d+)$)"};
  uint8_t m_cidr;
};
}  // namespace Network

#endif
