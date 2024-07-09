#ifndef __NETINTERFACEINFO_H
#define __NETINTERFACEINFO_H

#include <optional>
#include <string>
#include <unordered_set>

#include "Type.h"

namespace Network {

class NetInterfaceInfo {
 public:
  NetInterfaceInfo() {}
  ~NetInterfaceInfo() {}

  const std::string& getName() const { return m_name; }
  void setName(std::string name) { m_name = name; }

  const std::unordered_set<std::string>& getIP() const { return m_ip_list; }
  void addIP(std::string ip) { m_ip_list.emplace(ip); }
  void removeIP(std::string& ip) { m_ip_list.erase(ip); }

  std::optional<std::string> getGateway() const { return m_gateway; }
  void setGateway(std::string gateway) { m_gateway = gateway; }

  std::optional<MAC> getMac() const { return m_mac; }
  void setMac(MAC mac) { m_mac = mac; }

  std::optional<bool> getLink() const { return m_link; }
  void setLink(bool link) { m_link = link; }

  std::optional<Netmask> getNetmask() const { return m_netmask; }
  void setNetmask(std::string netmask) {
    m_netmask = std::make_optional<Netmask>(netmask);
  }
  void setNetmask(uint8_t cidr) {
    m_netmask = std::make_optional<Netmask>(cidr);
  }

  bool operator==(const NetInterfaceInfo& value) const {
    return m_name == value.m_name;
  }

  explicit operator std::string() const {
    std::ostringstream ret;
    ret << m_name;
    ret << (m_link.value_or(false) ? " up" : " down");
    ret << " gateway " << (m_gateway.has_value() ? m_gateway.value() : "");
    ret << " mac "
        << (m_mac.has_value() ? static_cast<std::string>(m_mac.value()) : "");
    ret << " netmask "
        << (m_netmask.has_value() ? static_cast<std::string>(m_netmask.value())
                                  : "");
    ret << " ip ";
    for (auto it : m_ip_list) {
      ret << it << " ";
    }
    return ret.str();
  }

 private:
  std::optional<bool> m_link;
  std::string m_name;
  std::unordered_set<std::string> m_ip_list;
  std::optional<std::string> m_gateway;
  std::optional<MAC> m_mac;
  std::optional<Netmask> m_netmask;
};

struct NetInterfaceInfoHasher {
  std::size_t operator()(const NetInterfaceInfo& info) const {
    return std::hash<std::string>{}(info.getName());
  }
};

}  // namespace Network

#endif
