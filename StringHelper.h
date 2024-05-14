#ifndef __STRINGHELPER_H
#define __STRINGHELPER_H

#include <string>
#include <vector>

std::string& trim(std::string& str, const std::string& chars = " ");

std::string& ltrim(std::string& str, const std::string& chars = " ");

std::string& rtrim(std::string& str, const std::string& chars = " ");

std::vector<std::string> split(const std::string& str, char separator);

std::string& replace(std::string& str, const std::string& what,
                     const std::string& with, bool replace_all = true);

bool startsWith(const std::string& str, const std::string& substr);

bool endsWith(const std::string& str, const std::string& substr);

std::string hexToString(const std::string& hexString);

#endif
