#ifndef __STRINGHELPER_H
#define __STRINGHELPER_H

#include <algorithm>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "Logger.h"
#include "OrderStringUnorderedMap.h"

std::string& trim(std::string& str, const std::string& chars = " ");

std::string& ltrim(std::string& str, const std::string& chars = " ");

std::string& rtrim(std::string& str, const std::string& chars = " ");

std::vector<std::string> split(const std::string& str, char separator);

std::string& replace(std::string& str, const std::string& what,
                     const std::string& with, bool replace_all = true);

bool startsWith(const std::string& str, const std::string& substr);

bool endsWith(const std::string& str, const std::string& substr);

std::string hexToString(const std::string& hexString);

template <typename T>
std::string toString(T&& value) {
  using decayType = typename std::decay<T>::type;
  if constexpr (std::is_same<decayType, std::string>::value) {
    return value;
  } else if constexpr (std::is_same<decayType, bool>::value) {
    return value ? "true" : "false";
  } else if constexpr (std::is_same<decayType, int>::value ||
                       std::is_same<decayType, double>::value) {
    return std::to_string(value);
  } else if constexpr (std::is_same<decayType,
                                    OrderStringUnorderedMap>::value) {
    std::stringstream ss;
    ss << "[";
    for (auto item : value.order) {
      auto found = value.map.find(item);
      if (found != value.map.end()) {
        ss << item.first << ":" << item.second;
      }
    }
    ss << "]";
  } else {
    throw std::runtime_error("Requested unknown datatype " +
                             std::string(__FUNCTION__) + " " +
                             std::string(typeid(T).name()));
  }
}

template <typename T>
T fromString(std::string&& str) {
  using decayType = typename std::decay<T>::type;
  if constexpr (std::is_same<decayType, std::string>::value) {
    return str;
  } else if constexpr (std::is_same<decayType, int>::value) {
    return std::stoi(str);
  } else if constexpr (std::is_same<decayType, uint>::value) {
    return std::stoul(str);
  } else if constexpr (std::is_same<decayType, double>::value) {
    return std::stod(str);
  } else if constexpr (std::is_same<decayType, bool>::value) {
    std::transform(str.begin(), str.end(), str.begin(), tolower);
    if (str == "true") {
      return true;
    }
    return false;
  } else if constexpr (std::is_same<decayType,
                                    std::vector<std::string>>::value) {
    std::vector<std::string> ret;
    if (!str.empty()) {
      if (str.find(',') == std::string::npos) {
        ret.push_back(str);
      } else {
        ret = split(str, ',');
      }
    }
    return ret;
  } else if constexpr (std::is_same<decayType, std::set<std::string>>::value) {
    std::set<std::string> ret;
    if (!str.empty()) {
      if (str.find(',') == std::string::npos) {
        ret.insert(str);
      } else {
        std::vector<std::string> buf = split(str, ',');
        ret = std::set<std::string>(buf.begin(), buf.end());
      }
    }
    return ret;
  } else if constexpr (std::is_same<decayType,
                                    OrderStringUnorderedMap>::value) {
    OrderStringUnorderedMap ret;
    if (!str.empty()) {
      if (str[0] == '[' && str[str.size() - 1] == ']') {
        std::vector<std::string> array_item =
            split(str.substr(1, str.size() - 2), ',');
        for (auto item : array_item) {
          std::vector<std::string> sub_str = split(item, ':');
          if (sub_str.size() != 2) {
            LOG_I() << "Wrong map: " << item;
            continue;
          }
          ret.m_order.push_back(sub_str.at(0));
          ret.m_map.emplace(sub_str.at(0), sub_str.at(1));
        }
      } else {
        LOG_E() << "Invalid array: " << str;
      }
    }
    return ret;
  } else {
    throw std::runtime_error("Requested unknown datatype " +
                             std::string(__FUNCTION__) + " " +
                             std::string(typeid(T).name()));
  }
}

#endif
