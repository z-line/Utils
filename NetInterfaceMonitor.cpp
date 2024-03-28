#include "NetInterfaceMonitor.h"

#include <cstring>
#include <functional>
#include <stdexcept>

#include "Logger.h"
#include "arpa/inet.h"
#include "linux/netlink.h"
#include "linux/route.h"
#include "linux/rtnetlink.h"
#include "unistd.h"

#define BUFSIZE 8192

NetInterfaceMonitor::NetInterfaceMonitor(std::function<void(void)> callback)
    : m_callback(callback) {
  if (m_thread.get() == nullptr) {
    m_force_stop = false;
    m_thread = std::make_unique<std::thread>(
        std::bind(&NetInterfaceMonitor::process, this), nullptr);
  }
}

NetInterfaceMonitor::~NetInterfaceMonitor() {
  m_force_stop = true;
  if (m_netlink_socket >= 0) {
    shutdown(m_netlink_socket, SHUT_RDWR);
  }
  if (m_thread.get() != nullptr) {
    m_thread->join();
    close(m_netlink_socket);
  }
}

void NetInterfaceMonitor::handle_net_route(void* nlh) {
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct rtmsg* rt = (struct rtmsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  int len;
  char tmp[256];

  for (attr = IFLA_RTA(rt); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    //    LOG_I() << "route " << attr->rta_type;
    switch (attr->rta_type) {
      case RTA_DST:
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        //        printf("DST: %s", tmp);
        break;
      case RTA_SRC:
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        //        printf("SRC: %s", tmp);
        break;
      case RTA_GATEWAY:
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        //        printf("GATEWAY: %s", tmp);
        break;
      default:
        break;
    }
  }
}

void NetInterfaceMonitor::handle_net_link(void* nlh) {
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct ifinfomsg* ifi = (struct ifinfomsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  int len;

  for (attr = IFLA_RTA(ifi); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    //    LOG_I() << "link " << attr->rta_type;
    switch (attr->rta_type) {
      case IFLA_IFNAME:
        //        printf("Interface %d : %s %s\n", ifi->ifi_index,
        //        (char*)RTA_DATA(attr),
        //               (ifi->ifi_flags & IFF_RUNNING) ? "running" : "");
        break;
      default:
        break;
    }
  }
}

void NetInterfaceMonitor::handle_net_addr(void* nlh) {
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct ifaddrmsg* ifaddr = (struct ifaddrmsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  char tmp[256];
  int len;

  for (attr = IFLA_RTA(ifaddr); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    //    LOG_I() << "addr " << attr->rta_type;
    switch (attr->rta_type) {
      case IFA_LABEL:
        //        printf("%s\n", (char*)RTA_DATA(attr));
        break;
      case IFA_ADDRESS:
        inet_ntop(ifaddr->ifa_family, RTA_DATA(attr), tmp, sizeof(tmp));
        //        printf("%s ", tmp);
        break;
      default:
        break;
    }
  }
}

void NetInterfaceMonitor::process(void) {
  pthread_setname_np(pthread_self(), "NetIfaceMonitor");
  m_netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (m_netlink_socket == -1) {
    throw std::runtime_error("netlink open failed");
  }

  struct sockaddr_nl sa;
  memset(&sa, 0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE |
                 RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;
  if (bind(m_netlink_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
    throw std::runtime_error("netlink bind failed");
  }

  char buffer[BUFSIZE];
  struct nlmsghdr* nlh;

  LOG_I() << "Start net interface monitor";
  while (!m_force_stop) {
    // FIXME block in recv when destruct this instance
    ssize_t len = recv(m_netlink_socket, buffer, sizeof(buffer), 0);
    if (len != -1) {
      m_callback();
      for (nlh = (struct nlmsghdr*)buffer; NLMSG_OK(nlh, len);
           nlh = NLMSG_NEXT(nlh, len)) {
        //        LOG_I() << "event " << nlh->nlmsg_type;
        switch (nlh->nlmsg_type) {
          case NLMSG_DONE:
          case NLMSG_ERROR:
            break;
          case RTM_NEWLINK:
          case RTM_DELLINK:
            handle_net_link(nlh);
            break;
          case RTM_NEWADDR:
          case RTM_DELADDR:
            handle_net_addr(nlh);
            break;
          case RTM_NEWROUTE:
          case RTM_DELROUTE:
            handle_net_route(nlh);
            break;
          default:
            break;
        }
      }
    }
  }
  LOG_I() << "Stop net interface monitor";
}
