#ifndef __NETINTERFACEMONITOR_H
#define __NETINTERFACEMONITOR_H

#include <filesystem>
#include <functional>
#include <mutex>
#include <thread>

#include "EnumString.h"
#include "INetInterfaceObserver.h"
#include "NetInterfaceInfo.h"

namespace Network {

class NetInterfaceMonitor {
 public:
  NetInterfaceMonitor();
  ~NetInterfaceMonitor();

  bool requestInterfaceAddress(void);
  bool requestInterfaceLinkInfo(void);
  bool requestInterfaceRouteInfo(void);

  std::unordered_set<NetInterfaceInfo, NetInterfaceInfoHasher>
  getInterfaceList() {
    std::unique_lock lock(m_mutex);
    return m_interface_list;
  }

  void registerObserver(INetInterfaceObserver* observer) {
    m_observer_list.insert(observer);
  }

  static bool interfaceExists(const std::string& interface) {
    std::string path = "/sys/class/net/" + interface;
    return std::filesystem::exists(path);
  }

 private:
  static const EnumStringHelper<uint> RTMessageEnumString;
  static const EnumStringHelper<uint> IFLAEnumString;
  static const EnumStringHelper<uint> IFAEnumString;
  static const EnumStringHelper<uint> RTMTypeEnumString;
  static const EnumStringHelper<uint> RTAEnumString;

  std::unordered_set<INetInterfaceObserver*> m_observer_list;

  std::mutex m_mutex;
  std::unordered_set<NetInterfaceInfo, NetInterfaceInfoHasher> m_interface_list;

  bool m_link_got = false;
  bool m_route_got = false;
  bool m_address_got = false;

  int m_netlink_socket = -1;
  bool m_force_stop = false;
  std::unique_ptr<std::thread> m_thread;
  std::function<void(void)> m_callback;

  std::string ifindexToName(int ifindex);
  std::string prefixlenToNetmask(int family, int prefixlen);
  bool handle_net_link(void* nlh, bool add);
  bool handle_net_addr(void* nlh, bool add);
  bool handle_net_route(void* nlh, bool add);
  void process(void);
};
}  // namespace Network

#endif
