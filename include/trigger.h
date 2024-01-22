#ifndef TRIGGER_H
#define TRIGGER_H

#include "expression.h"
#include "utils.h"

struct Trigger
{

  int id = -1;
  std::set<std::string> event_tags_any;
  std::set<std::string> event_tags_all;
  std::unordered_map<std::string, Expression> on_self_expressions;
  std::unordered_map<std::string, Expression> on_other_expressions;

  Trigger();
  Trigger(json &trigger_json);
};

#endif // !TRIGGER_H