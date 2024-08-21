#ifndef __INETINTERFACEOBSERVER_H
#define __INETINTERFACEOBSERVER_H

#include "NetInterfaceInfo.h"

namespace Network {

class INetInterfaceObserver {
 public:
  virtual ~INetInterfaceObserver() {}

  virtual void linkRemove(const NetInterfaceInfo& removed_interface) = 0;
  virtual void netChanged(const NetInterfaceInfo& changed_interface) = 0;
};
}  // namespace Network

#endif
