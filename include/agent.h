#ifndef AGENT_H
#define AGENT_H

#include "utils.h"
#include "parameter.h"

struct Story;

struct Agent
{
  Story *story = nullptr;

  const int id;
  const int current_time;
  std::string name;
  std::set<std::string> tags;
  std::unordered_map<std::string, Parameter> parameters;
  std::unordered_map<int, std::unordered_map<std::string, Parameter>> relationships;

  int other_agent_id = -1;

  Agent(int id, int current_time = 0);
  ~Agent();

  bool buildFromJSON(json &agent_json);

  Agent &updateParameter(std::string name, Parameter value);
  Parameter &getParameter(std::string name);

  bool hasParameter(std::string name) const;

  const Parameter operator[](const std::string &name) const;
  Parameter &operator[](const std::string &name);
};

#endif // AGENT_H