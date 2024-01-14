#include "event.h"
#include "utils.h"
#include "story.h"
#include "agent.h"

EventTemplate::EventTemplate(int id, json &event_template_json) : id(id)
{
  try
  {
    // pretty name
    pretty_name = event_template_json["name"];
    // reason
    if (event_template_json.contains("reason") && event_template_json["reason"].is_string())
      reason = event_template_json["reason"].get<std::string>();
    // type
    if (event_template_json.contains("type") && event_template_json["type"].is_string())
    {
      std::string type = event_template_json["type"].get<std::string>();
      if (type == "self")
        this->type = EVENT_TYPE_SELF;
      else if (type == "relation")
        this->type = EVENT_TYPE_RELATION;
    }
    // expression
    expression = Expression(event_template_json["expression"].get<std::string>());
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}

Event::Event(
    int id,
    double time,
    int agent_id,
    const EventTemplate &_template) : id(id), time(time), agent_id(agent_id), _template(_template) {}

std::string Event::buildExplanation()
{
  return Utils::replaceAll("Event <EVENT> was triggered by agent <AGENT><OTHER_AGENT><REASON>",
                           StringMap({
                               {"<EVENT>", _template.pretty_name},
                               {"<AGENT>", getAgent().name},
                               {"<OTHER_AGENT>", (other_agent_id == -1) ? "" : (" towards " + getOtherAgent().name)},
                               {"<REASON>", (_template.reason.size() == 0) ? "" : (" due to " + _template.reason)},
                           }));
}

Agent &Event::getAgent()
{
  if (_template.story != NULL && agent_id != -1)
    return (_template.story)->agents[agent_id];
  else
    return Agent::invalid_agent;
}

Agent &Event::getOtherAgent()
{
  if (_template.story != NULL && other_agent_id != -1)
    return (_template.story)->agents[other_agent_id];
  else
    return Agent::invalid_agent;
}
