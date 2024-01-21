
#include "api.h"

API::API()
{
  stories.emplace(0, Story());
  current_story_id = 0;
}

std::string API::command(std::string command)
{
  if (command.size() == 0)
    return "\n";
  std::string ret = "";
  std::vector<std::string> tokens;
  for (auto &str : Utils::split(command, " "))
    tokens.push_back(Utils::toUpper(str));
  if (tokens[0] == "HELP")
  {
    ret = HELP();
  }
  else if (tokens[0] == "JSON")
  {
    if (tokens.size() > 1)
    {
      ret = JSON(tokens[1]);
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "PROCEED")
  {
    if (tokens.size() > 1)
    {
      ret = PROCEED(std::stod(tokens[1]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "CLEAR")
  {
    ret = CLEAR();
  }
  else if (tokens[0] == "POP_EVENT")
  {
    if (tokens.size() > 1)
    {
      ret = POP_EVENT(std::stoi(tokens[1]));
    }
    else
    {
      ret = POP_EVENT();
    }
  }
  else if (tokens[0] == "LIST_AGENTS")
  {
    ret = LIST_AGENTS();
  }
  else if (tokens[0] == "LIST_EVENTS")
  {
    ret = LIST_EVENTS();
  }
  else if (tokens[0] == "LIST_EVENT_TEMPLATES")
  {
    ret = LIST_EVENT_TEMPLATES();
  }
  else if (tokens[0] == "LIST_PARAMETER_ALIASES")
  {
    ret = LIST_PARAMETER_ALIASES();
  }
  else if (tokens[0] == "LIST_PARAMETER_ALIASES")
  {
    ret = LIST_PARAMETER_ALIASES();
  }
  else if (tokens[0] == "UPDATE_AGENT_PARAMETER")
  {
    if (tokens.size() > 3)
    {
      ret = UPDATE_AGENT_PARAMETER(std::stoi(tokens[1]), tokens[2], std::stod(tokens[3]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "DESCRIBE_AGENT")
  {
    if (tokens.size() > 1)
    {
      ret = DESCRIBE_AGENT(std::stoi(tokens[1]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "DESCRIBE_EVENT_TEMPLATE")
  {
    if (tokens.size() > 1)
    {
      ret = DESCRIBE_EVENT_TEMPLATE(std::stoi(tokens[1]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "LIST_STORIES")
  {
    ret = LIST_STORIES();
  }
  else if (tokens[0] == "SELECT_STORY")
  {
    if (tokens.size() > 1)
    {
      ret = SELECT_STORY(std::stoi(tokens[1]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "NEW_STORY")
  {
    ret = NEW_STORY();
  }
  else if (tokens[0] == "DEL_STORY")
  {
    if (tokens.size() > 1)
    {
      ret = DEL_STORY(std::stoi(tokens[1]));
    }
    else
    {
      ret = "Missing argument";
    }
  }
  else if (tokens[0] == "EXIT")
  {
    on_exit();
    ret = "EXIT";
  }
  else
  {
    ret = "Unknown command";
  }
  if (ret.size() == 0)
    ret = "\n";
  return ret;
}
std::string API::HELP()
{
  std::string ret = "HELP\n";
  ret += "JSON <filename>\n";
  ret += "PROCEED <time>\n";
  ret += "CLEAR\n";
  ret += "POP_EVENT <amount>\n";
  ret += "LIST_AGENTS\n";
  ret += "LIST_EVENTS\n";
  ret += "LIST_EVENT_TEMPLATES\n";
  ret += "LIST_PARAMETER_ALIASES\n";
  ret += "LIST_PARAMETER_ALIASES\n";
  ret += "UPDATE_AGENT_PARAMETER <agent_id> <parameter> <value>\n";
  ret += "DESCRIBE_AGENT <agent_id>\n";
  return ret;
}
std::string API::JSON(std::string json_string)
{
  Story &story = getCurrentStory();
  try
  {
    story = Story();
    auto json = json::parse(json_string);
    story.buildFromJSON(json);
    return "OK";
  }
  catch (std::exception &e)
  {
    return "ERROR";
  }
}
std::string API::LIST_STORIES()
{
  std::string ret = "\n";
  for (auto &story : stories)
  {
    ret += std::to_string(story.first) + "\n";
  }
  ret.pop_back();
  return ret;
}
std::string API::SELECT_STORY(int story_id)
{
  if (stories.find(story_id) == stories.end())
    return "NOT FOUND";
  else
  {
    current_story_id = story_id;
    return "OK";
  }
}
std::string API::NEW_STORY()
{
  int story_id = 0;
  while (stories.find(story_id) != stories.end())
    story_id = rand() % 100000;
  stories.emplace(story_id, Story());
  return std::to_string(story_id);
}
std::string API::DEL_STORY(int story_id)
{
  if (story_id == 0)
    return "CANNOT DELETE DEFAULT STORY";
  if (stories.find(story_id) == stories.end())
    return "NOT FOUND";
  else
  {
    if (current_story_id == story_id)
      current_story_id = 0;
    stories.erase(story_id);
    return "OK";
  }
}
std::string API::PROCEED(double time)
{
  Story &story = getCurrentStory();
  story.proceed(time);
  return "OK";
}
std::string API::CLEAR()
{
  Story &story = getCurrentStory();
  story.clear();
  return "OK";
}
std::string API::POP_EVENT(int amount)
{
  Story &story = getCurrentStory();
  amount = std::max(amount, 0);
  amount = std::min(amount, (int)story.events.size());
  std::string ret = "\n";
  for (int i = 0; i < amount; i++)
  {
    Event &e = story.events.front();
    ret += std::to_string(e.time) + " ";
    ret += std::to_string(e._template.id) + " " + std::to_string(e.agent_id);
    if (e.other_agent_id != -1)
      ret += " " + std::to_string(e.other_agent_id);
    ret += "\n";
    story.events.pop_front();
  }
  if (ret.size() > 0)
    ret.pop_back();
  return ret;
}
std::string API::LIST_AGENTS()
{
  Story &story = getCurrentStory();
  std::string ret = "\n";
  for (auto &agent : story.agents)
  {
    ret += std::to_string(agent.first) + "\n";
  }
  ret.pop_back();
  return ret;
}
std::string API::LIST_EVENTS()
{
  return "TO-DO";
}
std::string API::LIST_EVENT_TEMPLATES()
{
  return "TO-DO";
}
std::string API::LIST_PARAMETER_ALIASES()
{
  return "TO-DO";
}
std::string API::UPDATE_AGENT_PARAMETER(int agent_id, std::string parameter, double value)
{
  Story &story = getCurrentStory();
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    if (agent.parameters.find(parameter) == agent.parameters.end())
      return "Parameter not found";
    else
    {
      story.agents[agent_id].parameters[parameter] = value;
      return "OK";
    }
  }
}
std::string API::DESCRIBE_AGENT(int agent_id)
{
  Story &story = getCurrentStory();
  std::string ret = "\n";
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    for (auto &param : agent.parameters)
    {
      ret += "PARAMETER " + param.first + " " + param.second.toString() + "\n";
    }
    for (auto &alias : story.parameter_aliases)
    {
      ret += "PARAMETER_ALIAS " + alias.first + " " + alias.second.evaluate(agent, agent).toString() + "\n";
    }
    for (auto &label : agent.labels)
    {
      ret += "LABEL " + label.first + " " + label.second + "\n";
    }
    ret.pop_back();
    return ret;
  }
}
std::string API::DESCRIBE_EVENT_TEMPLATE(int event_template_id)
{
  Story &story = getCurrentStory();
  if (story.event_templates.find(event_template_id) == story.event_templates.end())
    return "Event template not found";
  else
  {
    EventTemplate &event_template = story.event_templates.at(event_template_id);
    std::string ret = "\n";
    ret += "EVENT_TEMPLATE " + std::to_string(event_template.id) + "\n";
    ret += "NAME " + event_template.pretty_name + "\n";
    ret += "REASON " + event_template.reason + "\n";
    ret += "EXPRESSION " + event_template.expression.expression_string + "\n";
    ret.pop_back();
    return ret;
  }
}

API &API::operator<<(std::string line)
{
  std::string output = command(line);
  on_output(output);
  return *this;
}
Story &API::getCurrentStory()
{
  return stories[current_story_id];
};