#ifndef AGENT_H
#define AGENT_H

#include "utils.h"
#include "variable.h"

struct Story;

struct Agent
{
  Story *story = nullptr;

  const int id;
  double current_time = 0;
  std::unordered_map<std::string, std::string> labels;
  std::set<std::string> tags;
  std::unordered_map<std::string, Variable> parameters;
  std::unordered_map<int, std::unordered_map<std::string, Variable>> relationships;

  int other_agent_id = -1;

  static Agent invalid_agent;

  Agent(int id = -1);
  ~Agent();

  std::string getName() const;
  bool buildFromJSON(json &agent_json);
  void fillDefaultParameters(Agent &default_agent);

  bool hasVariable(std::string name) const;
  Agent &updateVariable(std::string name, Variable value);
  Variable getVariable(std::string name) const;

  void updateRelationship(int other_agent_id, std::string name, Variable value);

  Variable &operator[](const std::string &name);
  void operator=(const Agent &other);
  bool operator==(const Agent &other) const;
};

#endif // AGENT_H