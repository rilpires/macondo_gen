#ifndef STORY_H
#define STORY_H

#include "utils.h"
#include "expression.h"
#include "event.h"
#include "agent.h"

struct Story
{

  int open_agent_id = 0;
  int open_event_id = 0;
  int open_event_template_id = 0;

  std::unordered_map<int, std::string> reason_db;
  std::unordered_map<int, std::string> relation_db;

  std::unordered_map<int, Event> events;
  std::unordered_map<int, EventTemplate> event_templates;
  std::unordered_map<std::string, Expression> expressions;
  std::unordered_map<int, Agent> agents;

  Story();

  void buildFromJSON(json &story_json);
};

#endif // !STORY_H