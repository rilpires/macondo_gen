#ifndef STORY_H
#define STORY_H

#include "agent.h"
#include "event.h"
#include "expression.h"

struct Story
{

  int open_agent_id = 0;
  int open_event_id = 0;
  int open_event_template_id = 0;

  std::unordered_map<int, Event> events;
  std::unordered_map<int, EventTemplate> event_templates;
  std::unordered_map<int, Agent> agents;
  std::unordered_map<std::string, Expression> expressions;

  Story();

  void buildFromJSON(json &story_json);
};

#endif // !STORY_H