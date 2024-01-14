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

namespace Utils
{
  std::vector<std::string> split(std::string str, const char *delimiters);
  std::string &replaceAll(std::string &context, const std::string &from, const std::string &to);
  json loadJSON(std::string filename);
};

#endif