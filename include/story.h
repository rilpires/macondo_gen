#ifndef STORY_H
#define STORY_H

#include "utils.h"
#include "expression.h"
#include "event.h"
#include "agent.h"
#include "trigger.h"

struct Story
{

  int id = -1;
  double current_time = 0;
  int open_agent_id = 0;
  int open_event_template_id = 0;
  int open_trigger_id = 0;

  Agent default_agent;
  std::unordered_map<std::string, std::string> labels;
  std::set<std::string> tags;
  std::unordered_map<std::string, double> relation_default;
  std::unordered_map<int, EventTemplate> event_templates;
  std::unordered_map<std::string, Expression> parameter_aliases;
  std::unordered_map<int, Agent> agents;
  std::list<Event> events;
  std::unordered_map<int, Trigger> triggers;

  Story();

  void buildFromJSON(json &story_json);
  void clear();
  void proceed(double time);
  void triggerEvent(Event &event);
};

#endif // !STORY_H