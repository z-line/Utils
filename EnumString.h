
#ifndef __ENUMSTRING_H
#define __ENUMSTRING_H

#include <string>
#include <unordered_map>
#include <vector>

struct EnumClassHash {
  template <typename T>
  std::size_t operator()(T t) const {
    return static_cast<std::size_t>(t);
  }
};

template <typename EnumType>
class EnumStringHelper {
 public:
  EnumStringHelper(
      std::initializer_list<std::pair<EnumType, const char*>> mapping) {
    for (auto& it : mapping) {
      m_enum_to_string[it.first] = it.second;
      m_string_to_enum[it.second] = it.first;
    }
  }

  std::string toString(EnumType value) const {
    std::string ret;
    auto it = m_enum_to_string.find(value);
    if (it != m_enum_to_string.end()) {
      ret = it->second;
    }
    return ret;
  }

  EnumType fromString(const std::string& str) const {
    EnumType ret = m_string_to_enum.begin()->second;
    auto it = m_string_to_enum.find(str);
    if (it != m_string_to_enum.end()) {
      ret = it->second;
    }
    return ret;
  }

  bool fromString(const std::string& str, EnumType& val) const {
    bool ret = false;
    auto it = m_string_to_enum.find(str);
    if (it != m_string_to_enum.end()) {
      val = it->second;
      ret = true;
    }
    return ret;
  }

  std::vector<std::string> getEnumStrs() const {
    std::vector<std::string> ret;
    for (auto& it : m_string_to_enum) {
      ret.push_back(it.first);
    }
    return ret;
  }

 private:
  std::unordered_map<EnumType, std::string, EnumClassHash> m_enum_to_string;
  std::unordered_map<std::string, EnumType> m_string_to_enum;
};

#endif
