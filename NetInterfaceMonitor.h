#ifndef __NETINTERFACEMONITOR_H
#define __NETINTERFACEMONITOR_H

#include <functional>
#include <thread>

#include "EnumString.h"

class NetInterfaceMonitor {
 public:
  NetInterfaceMonitor(std::function<void(void)> callback);
  ~NetInterfaceMonitor();

 private:
  static const EnumStringHelper<uint> RTMessageEnumString;
  static const EnumStringHelper<uint> IFLAEnumString;
  static const EnumStringHelper<uint> IFAEnumString;
  static const EnumStringHelper<uint> RTMTypeEnumString;
  static const EnumStringHelper<uint> RTAEnumString;

  int m_netlink_socket = -1;
  bool m_force_stop = false;
  std::unique_ptr<std::thread> m_thread;
  std::function<void(void)> m_callback;

  bool handle_net_link(void* nlh);
  bool handle_net_addr(void* nlh);
  bool handle_net_route(void* nlh);
  void process(void);
};

#endif
