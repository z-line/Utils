#include "NetInterfaceMonitor.h"

#include <cstring>
#include <functional>

#include "Logger.h"
#include "arpa/inet.h"
#include "linux/if.h"
#include "linux/netlink.h"
#include "linux/rtnetlink.h"
#include "unistd.h"

#define BUFSIZE 8192

const EnumStringHelper<uint> NetInterfaceMonitor::RTMessageEnumString = {
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
    close(m_netlink_socket);
  }
  if (m_thread.get() != nullptr) {
    m_thread->join();
  }
}

bool NetInterfaceMonitor::handle_net_link(void* nlh) {
  bool ret = true;
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct ifinfomsg* ifi = (struct ifinfomsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  int len = 0;

  for (attr = IFLA_RTA(ifi); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    LOG_I() << "link type: " << IFLAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case IFLA_ADDRESS:
        break;
      case IFLA_BROADCAST:
        break;
      case IFLA_IFNAME:
        ret = true;
        printf("Interface %d : %s %s\n", ifi->ifi_index, (char*)RTA_DATA(attr),
               (ifi->ifi_flags & IFF_RUNNING) ? "running" : "");
        break;
      case IFLA_LINK:
        break;
      case IFLA_PERM_ADDRESS:
        break;
      case IFLA_MTU:
      case IFLA_QDISC:
      case IFLA_STATS:
      default:
        ret = false;
        break;
    }
  }
  LOG_I() << "quit handle_net_link";
  return ret;
}

bool NetInterfaceMonitor::handle_net_addr(void* nlh) {
  bool ret = true;
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct ifaddrmsg* ifaddr = (struct ifaddrmsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  char tmp[256];
  int len = 0;

  // FIXME 此处长挂会奔溃
  for (attr = IFLA_RTA(ifaddr); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    LOG_I() << "addr " << IFAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case IFA_ADDRESS:
        ret = true;
        inet_ntop(ifaddr->ifa_family, RTA_DATA(attr), tmp, sizeof(tmp));
        printf("%s ", tmp);
        break;
      case IFA_LOCAL:
        break;
      case IFA_LABEL:
        ret = true;
        printf("%s\n", (char*)RTA_DATA(attr));
        break;
      case IFA_BROADCAST:
        break;
      case IFA_ANYCAST:
        break;
      case IFA_CACHEINFO:
        break;
      case IFA_UNSPEC:
      default:
        ret = false;
        break;
    }
  }
  return ret;
}

bool NetInterfaceMonitor::handle_net_route(void* nlh) {
  bool ret = true;
  struct nlmsghdr* msg = (struct nlmsghdr*)nlh;
  struct rtmsg* rt = (struct rtmsg*)NLMSG_DATA(msg);
  struct rtattr* attr = (struct rtattr*)RTM_PAYLOAD(msg);
  int len = 0;
  char tmp[256];

  for (attr = IFLA_RTA(rt); RTA_OK(attr, len); attr = RTA_NEXT(attr, len)) {
    LOG_I() << "route " << RTAEnumString.toString(attr->rta_type);
    switch (attr->rta_type) {
      case RTA_DST:
        ret = true;
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        printf("DST: %s", tmp);
        break;
      case RTA_SRC:
        ret = true;
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        printf("SRC: %s", tmp);
        break;
      case RTA_GATEWAY:
        ret = true;
        inet_ntop(rt->rtm_family, RTA_DATA(attr), tmp, sizeof(tmp));
        printf("GATEWAY: %s", tmp);
        break;
      case RTA_UNSPEC:
      case RTA_IIF:
      case RTA_OIF:
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
        ret = false;
        break;
    }
  }
  return ret;
}

void NetInterfaceMonitor::process(void) {
  uint32_t retry = 0;
  pthread_setname_np(pthread_self(), "NetIfaceMonitor");
  do {
    m_netlink_socket = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
    if (m_netlink_socket == -1) {
      LOG_W() << "retry open netlink " << retry++ << " times";
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }
  } while (m_netlink_socket == -1 && !m_force_stop);

  struct timeval timeout = {.tv_sec = 1, .tv_usec = 0};
  while (setsockopt(m_netlink_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                    sizeof(timeout)) < 0 &&
         !m_force_stop) {
    LOG_W() << "retry set timeout of netlink " << retry++ << " times";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  struct sockaddr_nl sa;
  memset(&sa, 0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_groups = RTMGRP_LINK | RTMGRP_IPV4_IFADDR | RTMGRP_IPV4_ROUTE |
                 RTMGRP_IPV6_IFADDR | RTMGRP_IPV6_ROUTE;
  retry = 0;
  while (bind(m_netlink_socket, (struct sockaddr*)&sa, sizeof(sa)) == -1 &&
         !m_force_stop) {
    LOG_W() << "retry bind netlink " << retry++ << " times";
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

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
        case EINTR:
          break;
        default:
          LOG_W() << "Recv netlink failed: " << errno;
          break;
      }
    } else {
      LOG_I() << "Recv netlink";
      bool refresh = false;
      for (nlh = (struct nlmsghdr*)buffer; NLMSG_OK(nlh, len);
           nlh = NLMSG_NEXT(nlh, len)) {
        LOG_I() << "netlink event:"
                << RTMessageEnumString.toString(nlh->nlmsg_type);
        switch (nlh->nlmsg_type) {
          case NLMSG_DONE:
          case NLMSG_ERROR:
            break;
          case RTM_NEWLINK:
          case RTM_DELLINK:
          case RTM_GETLINK:
            refresh = handle_net_link(nlh);
            break;
          case RTM_NEWADDR:
          case RTM_DELADDR:
            refresh = handle_net_addr(nlh);
            break;
          case RTM_NEWROUTE:
          case RTM_DELROUTE:
            refresh = handle_net_route(nlh);
            break;
          default:
            break;
        }
      }
      if (refresh && m_callback) {
        m_callback();
      }
    }
  }
  LOG_I() << "Stop net interface monitor";
}
