#ifndef AGENT_H
#define AGENT_H

#include "utils.h"

struct Agent
{
  const int id;
  const int current_time;
  std::set<std::string> tags;
  std::unordered_map<std::string, Parameter> parameters;
  std::unordered_map<int, std::unordered_map<std::string, Parameter>> relationships;

  int other_agent_id = -1;

  Agent(int id, int current_time = 0) : id(id), current_time(current_time) {}
  ~Agent() {}

  Agent &updateParameter(std::string name, Parameter &value)
  {
    if (parameters.find(name) != parameters.end())
      parameters[name] = value;
    return *this;
  };
  Parameter &getParameter(std::string name)
  {
    return parameters[name];
  };

  const Parameter &operator[](const std::string &name) const
  {
    if (parameters.find(name) == parameters.end())
      return parameters.find(name)->second;
    else if (other_agent_id != -1)
    {
      if (relationships.find(other_agent_id) == relationships.end())
      {
        // relationships[other_agent_id] = std::unordered_map<std::string, Parameter>();
      }
      // if (relationships[other_agent_id].find(name) == relationships[other_agent_id].end())
      // {
      //   //
      // }
      // return relationships[other_agent_id][name];
      return Parameter();
    }
    else
    {
      std::cout << "Parameter " << name << " not found in agent " << id << std::endl;
      return Parameter();
    }
  }
};

#endif // AGENT_H