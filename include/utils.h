#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <fstream>
#include <iostream>

// External libraries
#include "nlohmann/json.hpp"
using json = nlohmann::json;

typedef std::unordered_map<std::string, std::string> StringMap;

namespace Utils
{
  std::vector<std::string> split(std::string str, const char *delimiters);
  std::string replaceAll(const std::string &context, const std::string &from, const std::string &to);
  std::string replaceAll(const std::string &context, const StringMap &replacements);
  json loadJSON(std::string filename);
};

#endif