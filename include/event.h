#ifndef EVENT_H
#define EVENT_H

#include <unordered_map>
#include <string>
#include "parameter.h"

struct EventTemplate
{
  const int id;
  std::string pretty_name;
  std::unordered_map<std::string, Parameter> default_parameters;
  // calculated with....
};

struct Event
{
  const int id;
  const double time;
  const EventTemplate &_template;
  const int agent_id;
  const int other_agent_id = -1;
  std::unordered_map<std::string, Parameter> parameters;

  Event(int id, double time, int agent_id, const EventTemplate &_template) : id(id), time(time), agent_id(agent_id), _template(_template) {}

  std::string buildExplanation()
  {
    std::string explanation = "Event " + _template.pretty_name + " was generated with parameters: ";
    for (auto &parameter : parameters)
    {
      explanation += parameter.first + " = " + parameter.second.toString();
      if (parameter.first != parameters.end()->first)
        explanation += ", ";
    }
    return explanation;
  }
};

#endif // !EVENT_H