#ifndef __INETINTERFACEOBSERVER_H
#define __INETINTERFACEOBSERVER_H

#include "NetInterfaceInfo.h"

namespace Network {

class INetInterfaceObserver {
 public:
  virtual void netChanged(const NetInterfaceInfo& changed_interface) = 0;
};
}  // namespace Network

#endif
