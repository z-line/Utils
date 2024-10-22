#ifndef __INETINTERFACEOBSERVER_H
#define __INETINTERFACEOBSERVER_H

#include "NetInterfaceInfo.h"

namespace Network {

class INetInterfaceObserver {
 public:
  INetInterfaceObserver() = default;
  INetInterfaceObserver(const INetInterfaceObserver&) = delete;

  virtual ~INetInterfaceObserver() {}

  INetInterfaceObserver& operator=(const INetInterfaceObserver&) = delete;

  virtual void linkRemove(const NetInterfaceInfo& removed_interface) = 0;
  virtual void netChanged(const NetInterfaceInfo& changed_interface) = 0;
};
}  // namespace Network

#endif
