
#include "expression.h"
#include "trigger.h"

Trigger::Trigger()
{
}
Trigger::Trigger(json &trigger_json)
{
  if (trigger_json.contains("id") && trigger_json["id"].is_number_integer())
    id = trigger_json["id"].get<int>();

  if (trigger_json.contains("on_self") && trigger_json["on_self"].is_object())
  {
    for (auto &pair : trigger_json["on_self"].items())
    {
      if (pair.value().is_string())
        on_self_expressions[pair.key()] = Expression(pair.value().get<std::string>());
    }
  }
  if (trigger_json.contains("on_other") && trigger_json["on_other"].is_object())
  {
    for (auto &pair : trigger_json["on_other"].items())
    {
      if (pair.value().is_string())
        on_other_expressions[pair.key()] = Expression(pair.value().get<std::string>());
    }
  }
  auto filter_json = trigger_json["filter"];
  if (filter_json.contains("event_tags_any") && filter_json["event_tags_any"].is_array())
  {
    for (auto &tag : filter_json["event_tags_any"])
    {
      if (tag.is_string())
        event_tags_any.insert(tag.get<std::string>());
    }
  }
  if (filter_json.contains("event_tags_all") && filter_json["event_tags_all"].is_array())
  {
    for (auto &tag : filter_json["event_tags_all"])
    {
      if (tag.is_string())
        event_tags_all.insert(tag.get<std::string>());
    }
  }
}
