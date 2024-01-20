#include "story.h"

#include "agent.h"
#include "event.h"
#include "expression.h"
#include "random.h"

Story::Story()
{
}

void Story::buildFromJSON(json &story_json)
{
  try
  {
    // Build agents
    for (auto &agent_json : story_json["agents"])
    {
      Agent agent(open_agent_id++);
      agent.story = this;
      agent.buildFromJSON(agent_json);
      agents.emplace(agent.id, agent);
    }

    // Build parameter_aliases
    for (auto &param_aliases_json : story_json["parameter_aliases"])
    {
      if (!param_aliases_json.contains("name") || !param_aliases_json["name"].is_string())
        continue;
      if (!param_aliases_json.contains("expression") || !param_aliases_json["expression"].is_string())
        continue;
      parameter_aliases.emplace(param_aliases_json["name"].get<std::string>(), Expression(param_aliases_json["expression"].get<std::string>()));
    }

    // Build event templates
    for (auto &event_template_json : story_json["event_templates"])
    {
      EventTemplate event_template(open_event_template_id++, event_template_json);
      event_template.story = this;
      event_templates.emplace(event_template.id, event_template);
    }

    // Building relation defaults
    for (auto &relation_json : story_json["relations"])
    {
      if (!relation_json.contains("name") || !relation_json["name"].is_string())
        continue;
      if (!relation_json.contains("default") || !relation_json["default"].is_number())
        continue;
      relation_default.emplace(relation_json["name"].get<std::string>(), relation_json["default"].get<double>());
    }

    // Filling defaults
    for (auto &agent : agents)
      for (auto &agent2 : agents)
      {
        for (auto &relation : relation_default)
        {
          agent.second.updateRelationship(agent2.second.id, relation.first, relation.second);
        }
      }

    std::cout << "Story built" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}

void Story::proceed(double duration)
{
  for (auto &event_template : event_templates)
  {
    if (event_template.second.type == EVENT_TYPE_SELF)
    {
      for (auto &agent : agents)
      {
        double d = event_template.second.expression.evaluate(agent.second, agent.second).toDouble();
        if (d > 0)
        {
          int amount = Random::exponentialSamples(d, duration);
          for (int i = 0; i < amount; i++)
          {
            Event event({
                Random::randomDouble(current_time, current_time + duration),
                event_template.second,
                agent.second.id,
                -1,
                d,
            });
            events.push_back(event);
          }
        }
      }
    }
    else if (event_template.second.type == EVENT_TYPE_RELATION)
    {
      for (auto &agent : agents)
      {
        for (auto &other_agent : agents)
        {
          if (agent.second.id == other_agent.second.id)
            continue;
          agent.second.other_agent_id = other_agent.second.id;
          double d = event_template.second.expression.evaluate(agent.second, other_agent.second).toDouble();
          if (d > 0)
          {
            int amount = Random::exponentialSamples(d, duration);
            for (int i = 0; i < amount; i++)
            {
              Event event({
                  Random::randomDouble(current_time, current_time + duration),
                  event_template.second,
                  agent.second.id,
                  other_agent.second.id,
                  d,
              });
              events.push_back(event);
            }
          }
        }
      }
    }
  }
  events.sort([](const Event &a, const Event &b)
              { return a.time < b.time; });
  current_time += duration;
}

void Story::clear()
{
  open_agent_id = 0;
  open_event_template_id = 0;
  current_time = 0;
  relation_default.clear();
  event_templates.clear();
  parameter_aliases.clear();
  agents.clear();
  events.clear();
}
