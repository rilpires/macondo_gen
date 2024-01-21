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
    this->type = EVENT_TYPE_SELF;
    if (event_template_json.contains("type") && event_template_json["type"].is_string())
    {
      std::string type = event_template_json["type"].get<std::string>();
      if (type == "self")
        this->type = EVENT_TYPE_SELF;
      else if (type == "unidirectional")
        this->type = EVENT_TYPE_UNIDIRECTIONAL;
      else if (type == "bidirectional")
        this->type = EVENT_TYPE_BIDIRECTIONAL;
    }
    // expression
    expression = Expression(event_template_json["expression"].get<std::string>());
    // labels
    if (event_template_json.contains("labels") && event_template_json["labels"].is_object())
      for (auto &label : event_template_json["labels"].items())
        if (label.value().is_string())
          labels[label.key()] = label.value().get<std::string>();
    // tags
    if (event_template_json.contains("tags") && event_template_json["tags"].is_array())
      for (auto &tag : event_template_json["tags"])
        if (tag.is_string())
          tags.insert(tag.get<std::string>());
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
  }
}

std::string Event::buildExplanation()
{
  return Utils::replaceAll("Event <EVENT> was triggered by agent <AGENT><OTHER_AGENT><REASON><RATE>",
                           StringMap({
                               {"<EVENT>", _template.pretty_name},
                               {"<AGENT>", getAgent().getName()},
                               {"<OTHER_AGENT>", (other_agent_id == -1) ? "" : (" towards " + getOtherAgent().getName())},
                               {"<REASON>", (_template.reason.size() == 0) ? "" : (" due to " + _template.reason)},
                               {"<RATE>", " (rate " + std::to_string(rate) + ")"},
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
