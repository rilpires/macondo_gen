#ifndef STORY_H
#define STORY_H

#include "utils.h"
#include "expression.h"
#include "event.h"
#include "agent.h"

struct Story
{

  double current_time = 0;
  int open_agent_id = 0;
  int open_event_id = 0;
  int open_event_template_id = 0;

  std::unordered_map<std::string, double> relation_default;
  std::unordered_map<int, EventTemplate> event_templates;
  std::unordered_map<std::string, Expression> expressions;
  std::unordered_map<int, Agent> agents;
  std::list<Event> events;

  Story();

  void buildFromJSON(json &story_json);

  void proceed(double time);
};

#endif // !STORY_H