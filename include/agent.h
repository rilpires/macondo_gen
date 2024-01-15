#ifndef AGENT_H
#define AGENT_H

#include "utils.h"
#include "variable.h"

struct Story;

struct Agent
{
  Story *story = nullptr;

  const int id;
  const int current_time;
  std::unordered_map<std::string, std::string> labels;
  std::unordered_map<std::string, Variable> parameters;
  std::unordered_map<int, std::unordered_map<std::string, Variable>> relationships;

  int other_agent_id = -1;

  static Agent invalid_agent;

  Agent(int id = -1, int current_time = 0);
  ~Agent();

  std::string getName() const;
  bool buildFromJSON(json &agent_json);

  bool hasVariable(std::string name) const;
  Agent &updateVariable(std::string name, Variable value);
  Variable getVariable(std::string name) const;

  void updateRelationship(int other_agent_id, std::string name, Variable value);

  Variable &operator[](const std::string &name);
};

#endif // AGENT_H