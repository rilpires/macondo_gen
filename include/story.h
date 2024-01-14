#ifndef STORY_H
#define STORY_H

#include "utils.h"

struct Story
{
  std::unordered_map<int, Event> events;
  std::unordered_map<int, EventTemplate> event_templates;
  std::unordered_map<int, Agent> agents;
};

#endif // !STORY_H