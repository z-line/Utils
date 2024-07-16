#include "NetInterfaceMonitor.h"

#include <cstring>
#include <functional>

#include "Logger.h"
#include "arpa/inet.h"
#include "linux/netlink.h"
#include "linux/rtnetlink.h"
#include "net/if.h"
#include "unistd.h"

#define BUFSIZE 8192

using namespace Network;

const EnumStringHelper<uint> NetInterfaceMonitor::RTMessageEnumString = {
    {NLMSG_NOOP, "NOOP"},
    {NLMSG_ERROR, "ERROR"},
    {NLMSG_DONE, "DONE"},
    {NLMSG_OVERRUN, "OVERRUN"},
    {RTM_NEWLINK, "NEWLINK"},
    {RTM_DELLINK, "DELLINK"},
    {RTM_GETLINK, "GETLINK"},
    {RTM_SETLINK, "SETLINK"},
    {RTM_NEWADDR, "NEWADDR"},
    {RTM_DELADDR, "DELADDR"},
    {RTM_GETADDR, "GETADDR"},
    {RTM_NEWROUTE, "NEWROUTE"},
    {RTM_DELROUTE, "DELROUTE"},
    {RTM_GETROUTE, "GETROUTE"},
    {RTM_NEWNEIGH, "NEWNEIGH"},
    {RTM_DELNEIGH, "DELNEIGH"},
    {RTM_GETNEIGH, "GETNEIGH"},
    {RTM_NEWRULE, "NEWRULE"},
    {RTM_DELRULE, "DELRULE"},
    {RTM_GETRULE, "GETRULE"},
    {RTM_NEWQDISC, "NEWQDISC"},
    {RTM_DELQDISC, "DELQDISC"},
    {RTM_GETQDISC, "GETQDISC"},
    {RTM_NEWTCLASS, "NEWTCLASS"},
    {RTM_DELTCLASS, "DELTCLASS"},
    {RTM_GETTCLASS, "GETTCLASS"},
    {RTM_NEWTFILTER, "NEWTFILTER"},
    {RTM_DELTFILTER, "DELTFILTER"},
    {RTM_GETTFILTER, "GETTFILTER"},
    {RTM_NEWACTION, "NEWACTION"},
    {RTM_DELACTION, "DELACTION"},
    {RTM_GETACTION, "GETACTION"},
    {RTM_NEWPREFIX, "NEWPREFIX"},
    {RTM_GETMULTICAST, "GETMULTICAST"},
    {RTM_GETANYCAST, "GETANYCAST"},
    {RTM_NEWNEIGHTBL, "NEWNEIGHTBL"},
    {RTM_GETNEIGHTBL, "GETNEIGHTBL"},
    {RTM_SETNEIGHTBL, "SETNEIGHTBL"},
    {RTM_NEWNDUSEROPT, "NEWNDUSEROPT"},
    {RTM_NEWADDRLABEL, "NEWADDRLABEL"},
    {RTM_DELADDRLABEL, "DELADDRLABEL"},
    {RTM_GETADDRLABEL, "GETADDRLABEL"},
    {RTM_GETDCB, "GETDCB"},
    {RTM_SETDCB, "SETDCB"},
    {RTM_NEWNETCONF, "NEWNETCONF"},
    {RTM_DELNETCONF, "DELNETCONF"},
    {RTM_GETNETCONF, "GETNETCONF"},
    {RTM_NEWMDB, "NEWMDB"},
    {RTM_DELMDB, "DELMDB"},
    {RTM_GETMDB, "GETMDB"},
    {RTM_NEWNSID, "NEWNSID"},
    {RTM_DELNSID, "DELNSID"},
    {RTM_GETNSID, "GETNSID"},
    {RTM_NEWSTATS, "NEWSTATS"},
    {RTM_GETSTATS, "GETSTATS"},
    {RTM_NEWCACHEREPORT, "NEWCACHEREPORT"},
    {RTM_NEWCHAIN, "NEWCHAIN"},
    {RTM_DELCHAIN, "DELCHAIN"},
    {RTM_GETCHAIN, "GETCHAIN"},
    {RTM_NEWNEXTHOP, "NEWNEXTHOP"},
    {RTM_DELNEXTHOP, "DELNEXTHOP"},
    {RTM_GETNEXTHOP, "GETNEXTHOP"},
    {RTM_NEWLINKPROP, "NEWLINKPROP"},
    {RTM_DELLINKPROP, "DELLINKPROP"},
    {RTM_GETLINKPROP, "GETLINKPROP"},
    {RTM_NEWVLAN, "NEWVLAN"},
    {RTM_DELVLAN, "DELVLAN"},
    {RTM_GETVLAN, "GETVLAN"},
    {RTM_NEWNEXTHOPBUCKET, "NEWNEXTHOPBUCKET"},
    {RTM_DELNEXTHOPBUCKET, "DELNEXTHOPBUCKET"},
    {RTM_GETNEXTHOPBUCKET, "GETNEXTHOPBUCKET"},
};

const EnumStringHelper<uint> NetInterfaceMonitor::IFLAEnumString = {
    {IFLA_UNSPEC, "UNSPEC"},
    {IFLA_ADDRESS, "ADDRESS"},
    {IFLA_BROADCAST, "BROADCAST"},
    {IFLA_IFNAME, "IFNAME"},
    {IFLA_MTU, "MTU"},
    {IFLA_LINK, "LINK"},
    {IFLA_QDISC, "QDISC"},
    {IFLA_STATS, "STATS"},
    {IFLA_COST, "COST"},
    {IFLA_PRIORITY, "PRIORITY"},
    {IFLA_MASTER, "MASTER"},
    {IFLA_WIRELESS, "WIRELESS"},
    {IFLA_PROTINFO, "PROTINFO"},
    {IFLA_TXQLEN, "TXQLEN"},
    {IFLA_MAP, "MAP"},
    {IFLA_WEIGHT, "WEIGHT"},
    {IFLA_OPERSTATE, "OPERSTATE"},
    {IFLA_LINKMODE, "LINKMODE"},
    {IFLA_LINKINFO, "LINKINFO"},
    {IFLA_NET_NS_PID, "NET_NS_PID"},
    {IFLA_IFALIAS, "IFALIAS"},
    {IFLA_NUM_VF, "NUM_VF"},
    {IFLA_VFINFO_LIST, "VFINFO_LIST"},
    {IFLA_STATS64, "STATS64"},
    {IFLA_VF_PORTS, "VF_PORTS"},
    {IFLA_PORT_SELF, "PORT_SELF"},
    {IFLA_AF_SPEC, "AF_SPEC"},
    {IFLA_GROUP, "GROUP"},
    {IFLA_NET_NS_FD, "NET_NS_FD"},
    {IFLA_EXT_MASK, "EXT_MASK"},
    {IFLA_PROMISCUITY, "PROMISCUITY"},
    {IFLA_NUM_TX_QUEUES, "NUM_TX_QUEUES"},
    {IFLA_NUM_RX_QUEUES, "NUM_RX_QUEUES"},
    {IFLA_CARRIER, "CARRIER"},
    {IFLA_PHYS_PORT_ID, "PHYS_PORT_ID"},
    {IFLA_CARRIER_CHANGES, "CARRIER_CHANGES"},
    {IFLA_PHYS_SWITCH_ID, "PHYS_SWITCH_ID"},
    {IFLA_LINK_NETNSID, "LINK_NETNSID"},
    {IFLA_PHYS_PORT_NAME, "PHYS_PORT_NAME"},
    {IFLA_PROTO_DOWN, "PROTO_DOWN"},
    {IFLA_GSO_MAX_SEGS, "GSO_MAX_SEGS"},
    {IFLA_GSO_MAX_SIZE, "GSO_MAX_SIZE"},
    {IFLA_PAD, "PAD"},
    {IFLA_XDP, "XDP"},
    {IFLA_EVENT, "EVENT"},
    {IFLA_NEW_NETNSID, "NEW_NETNSID"},
    {IFLA_IF_NETNSID, "IF_NETNSID"},
    {IFLA_TARGET_NETNSID, "TARGET_NETNSID"},
    {IFLA_CARRIER_UP_COUNT, "CARRIER_UP_COUNT"},
    {IFLA_CARRIER_DOWN_COUNT, "CARRIER_DOWN_COUNT"},
    {IFLA_NEW_IFINDEX, "NEW_IFINDEX"},
    {IFLA_MIN_MTU, "MIN_MTU"},
    {IFLA_MAX_MTU, "MAX_MTU"},
    {IFLA_PROP_LIST, "PROP_LIST"},
    {IFLA_ALT_IFNAME, "ALT_IFNAME"},
    {IFLA_PERM_ADDRESS, "PERM_ADDRESS"},
    {IFLA_PROTO_DOWN_REASON, "PROTO_DOWN_REASON"},
    {IFLA_PARENT_DEV_NAME, "PARENT_DEV_NAME"},
    {IFLA_PARENT_DEV_BUS_NAME, "PARENT_DEV_BUS_NAME"},
};

const EnumStringHelper<uint> NetInterfaceMonitor::IFAEnumString = {
    {IFA_UNSPEC, "UNSPEC"},
    {IFA_ADDRESS, "ADDRESS"},
    {IFA_LOCAL, "LOCAL"},
    {IFA_LABEL, "LABEL"},
    {IFA_BROADCAST, "BROADCAST"},
    {IFA_ANYCAST, "ANYCAST"},
    {IFA_CACHEINFO, "CACHEINFO"},
    {IFA_MULTICAST, "MULTICAST"},
    {IFA_FLAGS, "FLAGS"},
    {IFA_RT_PRIORITY, "RT_PRIORITY"},
    {IFA_TARGET_NETNSID, "TARGET_NETNSID"},
};

const EnumStringHelper<uint> NetInterfaceMonitor::RTMTypeEnumString = {
    {RTN_UNSPEC, "UNSPEC"},
    {RTN_UNICAST, "UNICAST"},
    {RTN_LOCAL, "LOCAL"},
    {RTN_BROADCAST, "BROADCAST"},
    {RTN_ANYCAST, "ANYCAST"},
    {RTN_MULTICAST, "MULTICAST"},
    {RTN_BLACKHOLE, "BLACKHOLE"},
    {RTN_UNREACHABLE, "UNREACHABLE"},
    {RTN_PROHIBIT, "PROHIBIT"},
    {RTN_THROW, "THROW"},
    {RTN_NAT, "NAT"},
    {RTN_XRESOLVE, "XRESOLVE"},
};

const EnumStringHelper<uint> NetInterfaceMonitor::RTAEnumString = {
    {RTA_UNSPEC, "UNSPEC"},
    {RTA_DST, "DST"},
    {RTA_SRC, "SRC"},
    {RTA_IIF, "IIF"},
    {RTA_OIF, "OIF"},
    {RTA_GATEWAY, "GATEWAY"},
    {RTA_PRIORITY, "PRIORITY"},
    {RTA_PREFSRC, "PREFSRC"},
    {RTA_METRICS, "METRICS"},
    {RTA_MULTIPATH, "MULTIPATH"},
    {RTA_PROTOINFO, "PROTOINFO"},
    {RTA_FLOW, "FLOW"},
    {RTA_CACHEINFO, "CACHEINFO"},
    {RTA_SESSION, "SESSION"},
    {RTA_MP_ALGO, "MP_ALGO"},
    {RTA_TABLE, "TABLE"},
    {RTA_MARK, "MARK"},
    {RTA_MFC_STATS, "MFC_STATS"},
    {RTA_VIA, "VIA"},
    {RTA_NEWDST, "NEWDST"},
    {RTA_PREF, "PREF"},
    {RTA_ENCAP_TYPE, "ENCAP_TYPE"},
    {RTA_ENCAP, "ENCAP"},
    {RTA_EXPIRES, "EXPIRES"},
    {RTA_PAD, "PAD"},
    {RTA_UID, "UID"},
    {RTA_TTL_PROPAGATE, "TTL_PROPAGATE"},
    {RTA_IP_PROTO, "IP_PROTO"},
    {RTA_SPORT, "SPORT"},
    {RTA_DPORT, "DPORT"},
    {RTA_NH_ID, "NH_ID"},
};

NetInterfaceMonitor::NetInterfaceMonitor() {
  uint32_t retry = 0;
  do {
    m_netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_netlink_socket == -1) {
      LOG_W() << "retry open netlink " << retry++ << " times";
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  } while (m_netlink_socket == -1 && !m_force_stop);

  retry = 0;
  struct timeval timeout = {.tv_sec = 0, .tv_usec = 500 * 1000};

  while (setsockopt(m_netlink_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                    sizeof(timeout)) < 0 &&
         !m_force_stop) {
    LOG_W() << "retry set timeout of netlink " << retry++ << " times";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  struct sockaddr_nl sa;
  memset(&sa, 0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE;

  while (bind(m_netlink_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1 &&
         !m_force_stop) {
    LOG_W() << "retry bind netlink " << retry++ << " times";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  if (m_thread.get() == nullptr) {
    m_force_stop = false;
    m_thread = std::make_unique<std::thread>(
        std::bind(&NetInterfaceMonitor::process, this), nullptr);
  }

  if (!requestInterfaceLinkInfo()) {
    LOG_W() << "Request link info failed";
  }
  if (!requestInterfaceRouteInfo()) {
    LOG_W() << "Request route info failed";
  }
  if (!requestInterfaceAddress()) {
    LOG_W() << "Request address info failed";
  }
}

NetInterfaceMonitor::~NetInterfaceMonitor() {
  m_force_stop = true;
  if (m_netlink_socket >= 0) {
    shutdown(m_netlink_socket, SHUT_RDWR);
    close(m_netlink_socket);
  }
  if (m_thread.get() != nullptr) {
    m_thread->join();
  }
}

bool NetInterfaceMonitor::requestInterfaceAddress(void) {
  struct nl_req {
    struct nlmsghdr nh;
    struct ifaddrmsg ifa;
  };

  if (m_netlink_socket <= 0) {
    return false;
  }
  LOG_I() << "Request address";
  struct nl_req req;
  memset(&req, 0, sizeof(req));
  req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifaddrmsg));
  req.nh.nlmsg_type = RTM_GETADDR;
  req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.ifa.ifa_family = AF_INET;
  if (send(m_netlink_socket, &req, req.nh.nlmsg_len, 0) < 0) {
    return false;
  }
  return true;
}

bool NetInterfaceMonitor::requestInterfaceLinkInfo(void) {
  struct nl_req {
    struct nlmsghdr nh;
    struct ifinfomsg ifi;
  };
  if (m_netlink_socket <= 0) {
    return false;
  }
  LOG_I() << "Request link";
  struct nl_req req;
  memset(&req, 0, sizeof(req));
  req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct ifinfomsg));
  req.nh.nlmsg_type = RTM_GETLINK;
  req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.ifi.ifi_family = AF_PACKET;
  if (send(m_netlink_socket, &req, req.nh.nlmsg_len, 0) < 0) {
    return false;
  }
  return true;
}

bool NetInterfaceMonitor::requestInterfaceRouteInfo(void) {
  struct nl_req {
    struct nlmsghdr nh;
    struct rtmsg rt;
  };
  if (m_netlink_socket <= 0) {
    return false;
  }
  LOG_I() << "Request route";
  struct nl_req req;
  memset(&req, 0, sizeof(req));
  req.nh.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
  req.nh.nlmsg_type = RTM_GETROUTE;
  req.nh.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.rt.rtm_family = AF_INET;
  if (send(m_netlink_socket, &req, req.nh.nlmsg_len, 0) < 0) {
    return false;
  }
  return true;
}

std::string NetInterfaceMonitor::ifindexToName(int ifindex) {
  char ifname[IF_NAMESIZE];
  if (if_indextoname(ifindex, ifname)) {
    return std::string(ifname);
  }
  return "";
}

std::string NetInterfaceMonitor::prefixlenToNetmask(int family, int prefixlen) {
  char netmask[INET6_ADDRSTRLEN];
  if (family == AF_INET) {
    struct in_addr addr;
    addr.s_addr = htonl(~((1 << (32 - prefixlen)) - 1));
    inet_ntop(AF_INET, &addr, netmask, sizeof(netmask));
  } else if (family == AF_INET6) {
    struct in6_addr addr;
    for (int i = 0; i < 16; ++i) {
      if (prefixlen >= 8) {
        addr.s6_addr[i] = 0xff;
        prefixlen -= 8;
      } else {
        addr.s6_addr[i] = (uint8_t)(~(0xff >> prefixlen));
        prefixlen = 0;
      }
    }
    inet_ntop(AF_INET6, &addr, netmask, sizeof(netmask));
  } else {
    return "";
  }
  return std::string(netmask);
}

bool NetInterfaceMonitor::handle_net_link(void* nlh, bool add) {
  std::unique_lock lock(m_mutex);
  bool ret = false;
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct ifinfomsg* ifi = (struct ifinfomsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)IFLA_RTA(msg);
  int len = IFLA_PAYLOAD(msg);

  NetInterfaceInfo info;

  for (attr = IFLA_RTA(ifi); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    LOG_V() << "link type: " << IFLAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case IFLA_ADDRESS:
        ret = true;
        info.setMac(MAC((uint8_t*)RTA_DATA(attr)));
        break;
      case IFLA_IFNAME:
        info.setName(std::string((char*)RTA_DATA(attr)));
        info.setLink(ifi->ifi_flags & IFF_RUNNING);
        if (!add) {
          auto it = m_interface_list.find(info);
          if (it != m_interface_list.end()) {
            m_interface_list.erase(it);
          }
        }
        break;
      case IFLA_BROADCAST:
      case IFLA_LINK:
      case IFLA_PERM_ADDRESS:
      case IFLA_MTU:
      case IFLA_QDISC:
      case IFLA_STATS:
      default:
        break;
    }
  }

  if (add) {
    auto found = m_interface_list.find(info);
    if (found == m_interface_list.end()) {
      m_interface_list.insert(info);
    } else {
      NetInterfaceInfo buffer;
      buffer = *found;
      if (info.getLink().has_value()) {
        buffer.setLink(info.getLink().value());
      }
      if (info.getMac().has_value()) {
        buffer.setMac(info.getMac().value());
      }
      m_interface_list.erase(found);
      m_interface_list.emplace(buffer);
    }
  } else {
    if (!m_observer_list.empty()) {
      for (auto observer : m_observer_list) {
        (*observer).linkRemove(info);
        LOG_I() << static_cast<std::string>(info);
      }
    }
    m_interface_list.erase(info);
  }
  return ret;
}

bool NetInterfaceMonitor::handle_net_addr(void* nlh, bool add) {
  std::unique_lock lock(m_mutex);
  bool ret = false;
  struct ifaddrmsg* ifaddr =
      (struct ifaddrmsg*)NLMSG_DATA((struct nlmsghdr*)nlh);
  struct rtattr* attr = IFA_RTA(ifaddr);
  int rtl = IFA_PAYLOAD((struct nlmsghdr*)nlh);
  char tmp[256];

  NetInterfaceInfo info;

  info.setName(ifindexToName(ifaddr->ifa_index));

  for (; RTA_OK(attr, rtl); attr = RTA_NEXT(attr, rtl)) {
    LOG_V() << "addr " << IFAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case IFA_ADDRESS:
        ret = true;
        inet_ntop(ifaddr->ifa_family, RTA_DATA(attr), tmp, sizeof(tmp));
        info.addIP(std::string(tmp));
        info.setNetmask(
            prefixlenToNetmask(ifaddr->ifa_family, ifaddr->ifa_prefixlen));
        break;
      case IFA_LABEL:
      case IFA_LOCAL:
      case IFA_BROADCAST:
      case IFA_ANYCAST:
      case IFA_CACHEINFO:
      case IFA_UNSPEC:
      default:
        break;
    }
  }

  auto found = m_interface_list.find(info);
  if (add) {
    if (found == m_interface_list.end()) {
      m_interface_list.emplace(info);
    } else {
      NetInterfaceInfo buffer = *found;
      for (auto it : info.getIP()) {
        buffer.addIP(it);
      }
      if (info.getNetmask().has_value()) {
        buffer.setNetmask(info.getNetmask().value().getCIDR());
      }
      m_interface_list.erase(found);
      m_interface_list.emplace(buffer);
    }
  } else {
    if (found != m_interface_list.end()) {
      NetInterfaceInfo buffer = *found;
      for (auto it : info.getIP()) {
        buffer.removeIP(it);
      }
      m_interface_list.erase(found);
      m_interface_list.emplace(buffer);
    }
  }

  return ret;
}

bool NetInterfaceMonitor::handle_net_route(void* nlh, bool add) {
  std::unique_lock lock(m_mutex);
  bool ret = false;
  struct rtmsg* rt = (struct rtmsg*)NLMSG_DATA(nlh);
  struct rtattr* attr = RTM_RTA(rt);
  int len = RTM_PAYLOAD((struct nlmsghdr*)nlh);
  char tmp[256];

  NetInterfaceInfo info;

  for (; RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    LOG_V() << "route " << RTAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case RTA_GATEWAY:
        ret = true;
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        info.setGateway(std::string(tmp));
        break;
      case RTA_OIF:
        info.setName(ifindexToName(*(int*)RTA_DATA(attr)));
        break;
      case RTA_DST:
      case RTA_SRC:
      case RTA_UNSPEC:
      case RTA_IIF:
      case RTA_PRIORITY:
      case RTA_PREFSRC:
      case RTA_METRICS:
      case RTA_MULTIPATH:
      case RTA_PROTOINFO:
      case RTA_FLOW:
      case RTA_CACHEINFO:
      case RTA_SESSION:
      case RTA_MP_ALGO:
      case RTA_TABLE:
      case RTA_MARK:
      case RTA_MFC_STATS:
      case RTA_VIA:
      case RTA_NEWDST:
      case RTA_PREF:
      case RTA_ENCAP_TYPE:
      case RTA_ENCAP:
      case RTA_EXPIRES:
      default:
        break;
    }
  }

  auto found = m_interface_list.find(info);
  LOG_E() << info.getName() << " " << add;
  if (add) {
    if (found == m_interface_list.end()) {
      m_interface_list.emplace(info);
    } else {
      NetInterfaceInfo buffer = *found;
      if (info.getGateway().has_value()) {
        buffer.setGateway(info.getGateway().value());
      }
      m_interface_list.erase(found);
      m_interface_list.emplace(buffer);
    }
  } else {
    if (found != m_interface_list.end()) {
      NetInterfaceInfo buffer = *found;
      buffer.setGateway("");
      m_interface_list.erase(found);
      m_interface_list.emplace(buffer);
    }
  }
  LOG_E() << info.getName() << "quit";

  return ret;
}

void NetInterfaceMonitor::process(void) {
  pthread_setname_np(pthread_self(), "NetIfaceMonitor");

  char buffer[BUFSIZE];
  struct nlmsghdr* nlh;
  LOG_I() << "Start net interface monitor";
  while (!m_force_stop) {
    // FIXME block in recv when destruct this instance
    ssize_t len = recv(m_netlink_socket, buffer, sizeof(buffer), 0);
    if (len == 0) {
      LOG_W() << "Netlink has been close by server";
    } else if (len < 0) {
      switch (errno) {
        case EAGAIN:
          if (!m_link_got) {
            requestInterfaceLinkInfo();
          }
          if (!m_route_got) {
            requestInterfaceRouteInfo();
          }
          if (!m_address_got) {
            requestInterfaceAddress();
          }
          break;
        case EINTR:
          break;
        default:
          LOG_W() << "Recv netlink failed: " << errno;
          break;
      }
    } else {
      bool refresh = false;
      for (nlh = (struct nlmsghdr*)buffer; NLMSG_OK(nlh, len);
           nlh = NLMSG_NEXT(nlh, len)) {
        LOG_I() << "netlink event:"
                << RTMessageEnumString.toString(nlh->nlmsg_type) << " - "
                << nlh->nlmsg_type;
        switch (nlh->nlmsg_type) {
          case NLMSG_DONE:
            break;
          case NLMSG_ERROR: {
            struct nlmsgerr* err = (struct nlmsgerr*)NLMSG_DATA(nlh);
            if (nlh->nlmsg_len < NLMSG_LENGTH(sizeof(struct nlmsgerr))) {
              fprintf(stderr, "Truncated error message\n");
            } else {
              errno = -err->error;
              LOG_E() << strerror(errno) << ". code: " << errno;
            }
          } break;
          case RTM_NEWLINK:
            m_link_got = true;
            refresh = handle_net_link(nlh, true);
            break;
          case RTM_DELLINK:
            refresh = handle_net_link(nlh, false);
            break;
          case RTM_NEWADDR:
            m_address_got = true;
            refresh = handle_net_addr(nlh, true);
            break;
          case RTM_DELADDR:
            refresh = handle_net_addr(nlh, false);
            break;
          case RTM_NEWROUTE:
            m_route_got = true;
            refresh = handle_net_route(nlh, true);
            break;
          case RTM_DELROUTE:
            refresh = handle_net_route(nlh, false);
            break;
          default:
            break;
        }
      }
      if (refresh && !m_observer_list.empty()) {
        for (auto observer : m_observer_list) {
          for (auto it : m_interface_list) {
            (*observer).netChanged(it);
            LOG_I() << static_cast<std::string>(it);
          }
        }
      }
    }
  }
  LOG_I() << "Stop net interface monitor";
}
