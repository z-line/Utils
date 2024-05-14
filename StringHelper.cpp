#include "StringHelper.h"

std::string& trim(std::string& str, const std::string& chars) {
  str.erase(0, str.find_first_not_of(chars, 0));
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}

std::string& ltrim(std::string& str, const std::string& chars) {
  str.erase(0, str.find_first_not_of(chars, 0));
  return str;
}

std::string& rtrim(std::string& str, const std::string& chars) {
  str.erase(str.find_last_not_of(chars) + 1);
  return str;
}

std::vector<std::string> split(const std::string& str, char separator) {
  std::vector<std::string> values;
  size_t pos = 0;
  size_t first_pos = 0;
  do {
    pos = str.find_first_of(separator, first_pos);
    if (pos != std::string::npos) {
      std::string value = str.substr(first_pos, (pos - first_pos));
      values.push_back(value);

      first_pos = pos + 1;
    }
  } while (pos != std::string::npos);
  std::string value = str.substr(first_pos);
  if (!value.empty()) {
    values.push_back(value);
  }
  return values;
}

std::string& replace(std::string& str, const std::string& what,
                     const std::string& with, bool replace_all) {
  std::size_t pos = 0;
  std::size_t start_pos = 0;
  do {
    pos = str.find(what, start_pos);
    if (pos != std::string::npos) {
      str.replace(pos, what.size(), with);
      start_pos = pos + 1u;
    }
  } while ((pos != std::string::npos) && (replace_all));

  return str;
}

bool startsWith(const std::string& str, const std::string& substr) {
  bool ret = false;
  if (str.size() >= substr.size()) {
    ret = (str.find(substr, 0) == 0);
  }
  return ret;
}

bool endsWith(const std::string& str, const std::string& substr) {
  bool ret = false;
  if (str.size() >= substr.size()) {
    size_t start_index = str.size() - substr.size();
    ret = (str.find(substr, start_index) == start_index);
  }
  return ret;
}

std::string hexToString(const std::string& hexString) {
  std::string result;
  for (size_t i = 0; i < hexString.length(); i += 2) {
    std::string byteString = hexString.substr(i, 2);
    char byte = static_cast<char>(std::stoi(byteString, nullptr, 16));
    result.push_back(byte);
  }
  return result;
}
