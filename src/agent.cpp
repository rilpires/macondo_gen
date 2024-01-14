#include "agent.h"
#include "story.h"
#include "expression.h"
#include "parameter.h"

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
    name = agent_json["name"];

    if (agent_json.contains("tags") && agent_json["tags"].is_array())
      for (auto &tag : agent_json["tags"])
        if (tag.is_string())
          tags.insert(tag.get<std::string>());

    if (agent_json.contains("parameters") && agent_json["parameters"].is_object())
      for (auto &parameter : agent_json["parameters"].items())
        if (parameter.value().is_number())
          parameters.emplace(parameter.key(), Parameter(parameter.value().get<double>()));
        else
          std::cerr << "Parameter " << parameter.key() << " of agent " << id << " is not a number" << std::endl;
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return false;
  }
  return true;
}

Agent &Agent::updateParameter(std::string name, Parameter value)
{
  if (parameters.find(name) != parameters.end())
    parameters[name] = value;
  return *this;
};

Parameter &Agent::getParameter(std::string name)
{
  return parameters[name];
};

bool Agent::hasParameter(std::string name) const
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

const Parameter Agent::operator[](const std::string &name) const
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
  else
  {
    std::cerr << "Parameter " << name << " not found" << std::endl;
    return Parameter();
  }
}

Parameter &Agent::operator[](const std::string &name)
{
  return parameters.find(name)->second;
}
