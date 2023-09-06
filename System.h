#ifndef __SYSTEM_H
#define __SYSTEM_H

#include <string>
#include <sstream>
#include <Logger.h>
#include <cstdlib>
#include <cstring>

#include "unistd.h"
#include "arpa/inet.h"
#include "net/if.h"
#include <sys/types.h>
#include <ifaddrs.h>

class System {
private:
    /* data */
public:
    System();
    ~System();

    enum class IPType { IPv4, IPv6 };

    static bool mySystem(std::string cmd) {
        int ret = system(cmd.c_str());
        int err = WEXITSTATUS(ret);
        if (err == 0) {
            LOG_I() << "exec: [" << cmd << "]";
        }
        return err == 0;
    }

    static std::string getAppPath(void) {
        char buffer[1024];
        ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer));
        if (len == -1) {
            perror("readlink");
        }
        return std::string(buffer);
    }

    static std::string getIP(IPType type, std::string interface) {
        std::string ret="";
        struct ifaddrs* ifaddr = nullptr;
        getifaddrs(&ifaddr);
        for (struct ifaddrs* ifa = ifaddr; ifa != nullptr; ifa = ifa->ifa_next) {
            if (type == IPType::IPv4 && ifa->ifa_addr->sa_family == AF_INET &&
                strcmp(ifa->ifa_name, interface.c_str()) == 0) {
                void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                char buffer[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, tmpAddrPtr, buffer, sizeof(buffer));
                ret=std::string(buffer);
                break;
            } else if (type == IPType::IPv6 && ifa->ifa_addr->sa_family == AF_INET6 &&
                       strcmp(ifa->ifa_name, interface.c_str()) == 0) {
                void* tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;
                char buffer[INET6_ADDRSTRLEN];
                inet_ntop(AF_INET6, tmpAddrPtr, buffer, sizeof(buffer));
                ret=std::string(buffer);
                break;
            }
        }
        if (ifaddr != nullptr) {
            freeifaddrs(ifaddr);
        }
        return ret;
    }
};

#endif
