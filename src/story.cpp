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
    // Build default agent
    for (auto &agent_json : story_json["agents"])
    {
      if (agent_json.contains("default") && agent_json["default"].is_boolean() && agent_json["default"].get<bool>())
      {
        default_agent = Agent(-1);
        default_agent.story = this;
        default_agent.buildFromJSON(agent_json);
      }
    }
    // Build agents
    for (auto &agent_json : story_json["agents"])
    {
      if (agent_json.contains("default") && agent_json["default"].is_boolean() && agent_json["default"].get<bool>())
        continue;
      Agent agent(open_agent_id++);
      agent.story = this;
      agent.buildFromJSON(agent_json);
      agent.fillDefaultParameters(default_agent);
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

    // Filling relation defaults
    for (auto &agent : agents)
    {

      for (auto &agent2 : agents)
      {
        for (auto &relation : relation_default)
        {
          agent.second.updateRelationship(agent2.second.id, relation.first, relation.second);
        }
      }
    }

    // Building triggers
    for (auto &trigger_json : story_json["triggers"])
    {
      Trigger trigger(trigger_json);
      trigger.id = open_trigger_id++;
      triggers.emplace(trigger.id, trigger);
    }

    // Filtering triggers
    for (auto &event_template_entry : event_templates)
    {
      for (auto &trigger_entry : triggers)
      {
        Trigger &trigger = trigger_entry.second;
        EventTemplate &event_template = event_template_entry.second;
        bool valid = false;
        if (trigger.event_tags_any.size() > 0)
        {
          for (auto &tag : trigger.event_tags_any)
          {
            if (event_template.tags.find(tag) != event_template.tags.end())
            {
              valid = true;
              break;
            }
          }
        }
        else if (trigger.event_tags_all.size() > 0)
        {
          valid = true;
          for (auto &tag : trigger.event_tags_all)
          {
            if (event_template.tags.find(tag) == event_template.tags.end())
            {
              valid = false;
              break;
            }
          }
        }
        if (valid)
          event_template.available_trigger_ids.insert(trigger.id);
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
        double mean = event_template.second.expression.evaluate(agent.second, agent.second).toDouble();
        double now = current_time + Random::exponential(mean);
        while ((mean > 0) && (now < current_time + duration))
        {
          Event event({
              now,
              event_template.second,
              agent.second.id,
              -1,
              mean,
          });
          events.push_back(event);
          now += Random::exponential(mean);
        }
      }
    }
    else
    {
      for (auto &agent : agents)
      {
        for (auto &other_agent : agents)
        {
          if (agent.second.id == other_agent.second.id)
            continue;
          if (event_template.second.type == EVENT_TYPE_UNIDIRECTIONAL)
            if (agent.second.id > other_agent.second.id)
              continue;
          agent.second.other_agent_id = other_agent.second.id;
          double mean = event_template.second.expression.evaluate(agent.second, other_agent.second).toDouble();
          double now = current_time + Random::exponential(mean);
          while ((mean > 0) && (now < current_time + duration))
          {
            Event event({
                now,
                event_template.second,
                agent.second.id,
                other_agent.second.id,
                mean,
            });
            events.push_back(event);
            now += Random::exponential(mean);
          }
        }
      }
    }
  }
  events.sort([](const Event &a, const Event &b)
              { return a.time < b.time; });
  for (auto &event : events)
  {
    triggerEvent(event);
  }
  current_time += duration;
}

void Story::triggerEvent(Event &event)
{
  for (auto available_entry : event._template.available_trigger_ids)
  {
    int trigger_id = available_entry;
    if (triggers.find(trigger_id) == triggers.end())
    {
      int template_id = event._template.id;
      EventTemplate &event_template = event_templates.at(template_id);
      event_template.available_trigger_ids.erase(trigger_id);
      continue;
    }
    Trigger &trigger = triggers.at(trigger_id);
    Agent &agent1 = event.getAgent();
    Agent &agent2 = event.getOtherAgent();

    if (agent1 == Agent::invalid_agent)
      continue;

    for (const auto &trigger_param : trigger.on_self_expressions)
    {
      if (agent2 == Agent::invalid_agent)
      {
        Variable value = trigger_param.second.evaluate(agent1, agent1);
        agent1.updateVariable(trigger_param.first, value);
      }
      else
      {
        Variable value = trigger_param.second.evaluate(agent1, agent2);
        agent1.updateVariable(trigger_param.first, value);
      }
    }

    if (agent2 == Agent::invalid_agent)
      continue;

    for (const auto &trigger_param : trigger.on_other_expressions)
    {
      Variable value = trigger_param.second.evaluate(agent2, agent1);
      agent2.updateVariable(trigger_param.first, value);
    }
  }
}

void Story::clear()
{
  open_agent_id = 0;
  open_event_template_id = 0;
  open_trigger_id = 0;
  default_agent = Agent();
  labels.clear();
  tags.clear();
  current_time = 0;
  relation_default.clear();
  event_templates.clear();
  parameter_aliases.clear();
  agents.clear();
  events.clear();
  triggers.clear();
}
