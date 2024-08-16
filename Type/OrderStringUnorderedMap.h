#ifndef __ORDERSTRINGUNORDEREDMAP_H
#define __ORDERSTRINGUNORDEREDMAP_H

#include <unordered_map>
#include <vector>

class OrderStringUnorderedMap {
 public:
  std::unordered_map<std::string, std::string> m_map;
  std::vector<std::string> m_order;
};

#endif
