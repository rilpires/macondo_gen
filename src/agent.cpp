#include "agent.h"
#include "story.h"
#include "expression.h"
#include "variable.h"

Agent Agent::invalid_agent;

Agent::Agent(int id, int current_time) : id(id), current_time(current_time)
{
  // std::cout << "Agent " << id << " created" << std::endl;
}

Agent::~Agent()
{
  // std::cout << "Agent " << id << " destroyed" << std::endl;
}

bool Agent::buildFromJSON(json &agent_json)
{
  try
  {
    if (agent_json.contains("labels") && agent_json["labels"].is_object())
      for (auto &label : agent_json["labels"].items())
        if (label.value().is_string())
          labels.emplace(label.key(), label.value().get<std::string>());
        else
          std::cerr << "Label " << label.key() << " of agent " << id << " is not a string" << std::endl;

    if (agent_json.contains("parameters") && agent_json["parameters"].is_object())
      for (auto &parameter : agent_json["parameters"].items())
        if (parameter.value().is_number())
          parameters.emplace(parameter.key(), Variable(parameter.value().get<double>()));
        else
          std::cerr << "Variable " << parameter.key() << " of agent " << id << " is not a number" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return false;
  }
  return true;
}

Agent &Agent::updateVariable(std::string name, Variable value)
{
  if (parameters.find(name) != parameters.end())
    parameters[name] = value;
  return *this;
};

Variable Agent::getVariable(std::string name) const
{

  if ((parameters.size() > 0) && parameters.find(name) != parameters.end())
  {
    return parameters.find(name)->second;
  }
  else if ((story != nullptr) && (story->expressions.size() > 0) && (story->expressions.find(name) != story->expressions.end()))
  {
    const Expression &e = (story->expressions.find(name)->second);
    return e.evaluate(*this, *this);
  }
  else if (other_agent_id != -1 &&
           (relationships.find(other_agent_id) != relationships.end()) &&
           (relationships.at(other_agent_id).find(name) != relationships.at(other_agent_id).end()))
  {
    return relationships.at(other_agent_id).at(name);
  }
  else
  {
    std::cerr << "Variable " << name << " not found" << std::endl;
    return Variable();
  }
};

bool Agent::hasVariable(std::string name) const
{
  if (parameters.find(name) != parameters.end())
    return true;
  else if ((story != nullptr) && (story->expressions.find(name) != story->expressions.end()))
  {
    return true;
  }
  else
  {
    return false;
  }
}

void Agent::updateRelationship(int other_agent_id, std::string name, Variable value)
{
  if (other_agent_id == id)
    return;
  else if (relationships.find(other_agent_id) == relationships.end())
  {
    relationships.emplace(other_agent_id, std::unordered_map<std::string, Variable>());
  }
  relationships.at(other_agent_id).emplace(name, value);
}

std::string Agent::getName() const
{
  if (labels.find("name") != labels.end())
    return labels.at("name");
  else
    return "Agent_" + std::to_string(id);
}

Variable &Agent::operator[](const std::string &name)
{
  return parameters[name];
}
