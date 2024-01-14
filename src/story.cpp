#include "story.h"

#include "agent.h"
#include "event.h"
#include "expression.h"

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

    // Build expressions
    for (auto &expression_json : story_json["expressions"])
    {
      if (!expression_json.contains("name") || !expression_json["name"].is_string())
        continue;
      if (!expression_json.contains("expression") || !expression_json["expression"].is_string())
        continue;
      expressions.emplace(expression_json["name"].get<std::string>(), Expression(expression_json["expression"].get<std::string>()));
    }

    // Build event templates
    for (auto &event_template_json : story_json["event_templates"])
    {
      EventTemplate event_template(open_event_template_id++, event_template_json);
      event_template.story = this;
      event_templates.emplace(event_template.id, event_template);
    }
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}
