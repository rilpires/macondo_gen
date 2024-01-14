#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <iostream>

// External libraries
#include "nlohmann/json.hpp"
using json = nlohmann::json;

// My actual code
#include "random.h"
#include "parameter.h"
#include "event.h"
#include "agent.h"
#include "expression.h"
#include "story.h"

namespace Utils
{
  std::vector<std::string> split(std::string str, const char *delimiters);
  std::string &replaceAll(std::string &context, const std::string &from, const std::string &to);
};

#endif