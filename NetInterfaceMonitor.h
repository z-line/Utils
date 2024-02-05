#ifndef __NETINTERFACEMONITOR_H
#define __NETINTERFACEMONITOR_H

#include <functional>
#include <thread>

class NetInterfaceMonitor {
 public:
  NetInterfaceMonitor(std::function<void(void)> callback);
  ~NetInterfaceMonitor();

 private:
  int m_netlink_socket = -1;
  bool m_force_stop = false;
  std::unique_ptr<std::thread> m_thread;
  std::function<void(void)> m_callback;

  void handle_net_route(void* nlh);
  void handle_net_link(void* nlh);
  void handle_net_addr(void* nlh);
  void process(void);
};

#endif
