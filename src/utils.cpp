#include "utils.h"

std::vector<std::string> Utils::split(std::string str, const char *delimiters)
{
  std::vector<std::string> tokens;
  char *token = strtok((char *)str.c_str(), delimiters);
  while (token != NULL)
  {
    if (strlen(token) > 0)
      tokens.push_back(std::string(token));
    token = strtok(NULL, delimiters);
  }
  return tokens;
}

std::string Utils::replaceAll(const std::string &context, const std::string &from, const std::string &to)
{
  std::string ret(context);
  size_t start_pos = 0;
  while ((start_pos = ret.find(from, start_pos)) != std::string::npos)
  {
    ret.replace(start_pos, from.length(), to);
    start_pos += to.length();
  }
  return ret;
}

std::string Utils::replaceAll(const std::string &context, const StringMap &replacements)
{
  std::string ret(context);
  for (auto &replacement : replacements)
    ret = replaceAll(ret, replacement.first, replacement.second);
  return ret;
}

json Utils::loadJSON(std::string filename)
{
  try
  {
    std::ifstream file(filename);
    json j;
    file >> j;
    file.close();
    return j;
  }
  catch (std::exception &e)
  {
    std::cout << "Error loading JSON file " << filename << " : " << e.what() << std::endl;
    return json();
  }
}