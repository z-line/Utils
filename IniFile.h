#ifndef __INIFILE_H
#define __INIFILE_H

#include <algorithm>
#include <fstream>
#include <functional>
#include <mutex>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "Logger.h"
#include "StringHelper.h"

class IniFile {
 public:
  class Value;
  using IniData =
      std::unordered_map<std::string, std::unordered_map<std::string, Value>>;

  IniFile(const std::string& path) : m_path(path) {}
  ~IniFile() {}

  bool load() { return load(m_file_data); }
  bool save() { return save(m_file_data); }
  const IniData& getData() { return m_file_data; }
  Value getValue(const std::string& section, const std::string& key) {
    std::unique_lock lock(m_mutex);
    return getValue(m_file_data, section, key);
  }
  void setValue(const std::string& section, const std::string& key,
                Value&& value) {
    std::unique_lock lock(m_mutex);
    setValue(m_file_data, section, key, value);
  }
  const std::string& getPath() { return m_path; }
  bool load(IniData& data_buffer) {
    std::unique_lock lock(m_mutex);
    std::ifstream in(m_path);
    if (!in.is_open()) {
      return false;
    }
    in.seekg(0);
    std::string line;
    std::string section;
    while (std::getline(in, line)) {
      if (line.empty()) {
        // Empty line
      } else if (line[0] == '#') {
        // Comment
      } else if (line[0] == '[') {
        // Section
        if (line[line.size() - 1u] == ']' ||
            (line[line.size() - 2u] == ']' && line[line.size() - 1u] == '\r')) {
          section = line.substr(
              1u, line.size() - (line[line.size() - 1u] == ']' ? 2u : 3u));
          auto it = data_buffer.find(section);
          if (it == data_buffer.end()) {
            data_buffer[section] = std::unordered_map<std::string, Value>();
          }
        } else {
          throw std::runtime_error("Wrong ini format: wrong section");
        }
      } else {
        // Parameter
        if (section.empty()) {
          throw std::runtime_error("Wrong ini format: no section");
        }
        if (line.back() == '\r') {
          line = line.substr(0, line.length() - 1);
        }
        if (line.empty()) {
          continue;
        }
        auto pos = line.find_first_of('=');
        if (pos != line.npos) {
          std::string name = line.substr(0, pos);
          std::string value = line.substr(pos + 1u);
          data_buffer[section][name] = value;
          if (name.empty()) {
            throw std::runtime_error("Wrong ini format: no name");
          }
        } else {
          throw std::runtime_error("Wrong ini format: no =");
        }
      }
    }
    return true;
  }
  bool save(IniData& data) {
    std::unique_lock lock(m_mutex);
    std::ofstream out(m_path);
    if (!out.is_open()) {
      return false;
    }
    out.seekp(0);
    for (auto section = data.begin(); section != data.end(); section++) {
      out << "[" << section->first << "]" << '\n';
      for (auto key_value = section->second.begin();
           key_value != section->second.end(); key_value++) {
        out << key_value->first << "="
            << static_cast<std::string>(key_value->second) << "\n";
      }
      out << "\n";
    }
    out.close();
    lock.unlock();
    load(m_file_data);
    lock.lock();
    return true;
  }
  static Value getValue(const IniData& source, const std::string& section,
                        const std::string& key) {
    auto found_section = source.find(section);
    if (found_section == source.end()) {
      return Value();
    }
    auto section_ref = found_section->second;
    auto found = section_ref.find(key);
    if (found == section_ref.end()) {
      return Value();
    }
    return found->second;
  }
  static void setValue(IniData& source, const std::string& section,
                       const std::string& key, const Value& value) {
    auto found_section = source.find(section);
    if (found_section != source.end()) {
      std::unordered_map<std::string, Value>& section_ref = source.at(section);
      section_ref[key] = value;
    } else {
      std::unordered_map<std::string, Value> buffer = {{key, value}};
      source.insert_or_assign(section, buffer);
    }
  }

  class Value {
   public:
    Value() = default;
    ~Value() = default;
    template <typename T>
    Value(T&& value) : m_value(toString(std::forward<T>(value))) {}
    Value(const char* value) : m_value(value) {}
    Value(Value&& value) noexcept : m_value(std::move(value.m_value)) {}
    Value(const Value& other) : m_value(other.m_value) {}
    Value(Value& other) : m_value(other.m_value) {}

    Value& operator=(const Value& copy) {
      m_value = copy.m_value;
      return *this;
    }

    template <typename T>
    explicit operator T() {
      return fromString<T>(m_value.has_value() ? m_value.value() : "");
    }

    bool operator==(const Value& other) { return m_value == other.m_value; }
    bool operator!=(const Value& other) { return !(*this == other); }

    bool hasValue() { return m_value.has_value(); }

   private:
    std::optional<std::string> m_value;
  };

 private:
  std::string m_path;
  IniData m_file_data;
  std::mutex m_mutex;
};

#endif
