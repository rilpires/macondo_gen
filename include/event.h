#ifndef EVENT_H
#define EVENT_H

#include <unordered_map>
#include <string>
#include "variable.h"
#include "expression.h"

enum EVENT_TYPE
{
  EVENT_TYPE_SELF,
  EVENT_TYPE_RELATION
};

struct Story;

struct EventTemplate
{
  Story *story = nullptr;
  const int id;
  EVENT_TYPE type = EVENT_TYPE_SELF;
  std::string pretty_name;
  std::string reason;
  std::unordered_map<std::string, Variable> default_parameters;
  Expression expression;

  EventTemplate(int id, json &event_template_json);
};

struct Event
{
  const int id;
  const double time;
  const EventTemplate &_template;
  const int agent_id = -1;
  const int other_agent_id = -1;

  Event(int id, double time, int agent_id, const EventTemplate &_template);

  Agent &getAgent();
  Agent &getOtherAgent();
  std::string buildExplanation();
};

#endif // !EVENT_H