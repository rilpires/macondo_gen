
#include "api.h"

API::API()
{
  stories.emplace(0, Story());
  current_story_id = 0;
}

void API::onExit(std::function<void()> callback)
{
  on_exit = callback;
}
void API::onOutput(std::function<void(std::string)> callback)
{
  on_output = callback;
}

std::string API::command(std::string command)
{
  if (command.size() == 0)
    return "\n";
  std::string ret = "";
  std::vector<std::string> tokens;
  for (auto &str : Utils::split(command, " "))
    tokens.push_back((tokens.size() == 0) ? Utils::toUpper(str) : str);
#define ELSEIF_COMMAND(name, argc, retcall) \
  else if (tokens[0] == #name)              \
  {                                         \
    if (tokens.size() > argc)               \
    {                                       \
      ret = retcall;                        \
    }                                       \
    else                                    \
    {                                       \
      ret = "Missing argument";             \
    }                                       \
  }
#define COMMAND_0(name) \
  ELSEIF_COMMAND(name, 0, name())
#define COMMAND_1(name, caster1) \
  ELSEIF_COMMAND(name, 1, name(caster1(tokens[1])))
#define COMMAND_2(name, caster1, caster2) \
  ELSEIF_COMMAND(name, 2, name(caster1(tokens[1]), caster2(tokens[2])))
#define COMMAND_3(name, caster1, caster2, caster3) \
  ELSEIF_COMMAND(name, 3, name(caster1(tokens[1]), caster2(tokens[2]), caster3(tokens[3])))
#define COMMAND_4(name, caster1, caster2, caster3, caster4) \
  ELSEIF_COMMAND(name, 4, name(caster1(tokens[1]), caster2(tokens[2]), caster3(tokens[3]), caster4(tokens[4])))

  if (tokens[0] == "JSON")
  {
    if (tokens.size() > 1)
    {
      std::string all_json_string = command.substr(5);
      ret = JSON(all_json_string);
    }
    else
    {
      ret = "Missing argument";
    }
  }
  COMMAND_1(PROCEED, std::stod)
  COMMAND_0(CLEAR)
  COMMAND_0(HELP)
  COMMAND_1(POP_EVENT, std::stoi)
  COMMAND_0(LIST_AGENTS)
  COMMAND_0(LIST_PARAMETER_ALIASES)
  COMMAND_3(UPDATE_AGENT_PARAMETER, std::stoi, , std::stod)
  COMMAND_3(UPDATE_AGENT_LABEL, std::stoi, , )
  COMMAND_4(UPDATE_AGENT_RELATION, std::stoi, std::stoi, , std::stod)
  COMMAND_2(ADD_AGENT_TAG, std::stoi, )
  COMMAND_2(REMOVE_AGENT_TAG, std::stoi, )
  COMMAND_1(DESCRIBE_AGENT, std::stoi)
  COMMAND_2(DESCRIBE_AGENT_RELATION, std::stoi, std::stoi)
  COMMAND_0(LIST_EVENT_TEMPLATES)
  COMMAND_1(DESCRIBE_EVENT_TEMPLATE, std::stoi)
  COMMAND_0(LIST_STORIES)
  COMMAND_1(SELECT_STORY, std::stoi)
  COMMAND_0(NEW_STORY)
  COMMAND_1(DEL_STORY, std::stoi)
  else if (tokens[0] == "EXIT")
  {
    ret = "OK";
    on_exit();
  }
  else
  {
    ret = "Unknown command";
  }
  while (ret.size() > 1 && ret[0] == '\n')
    ret.erase(ret.begin());
  if (ret.size() == 0)
    ret = "\n";
  else if (ret.back() != '\n')
    ret += "\n";
  return ret;
}
std::string API::HELP()
{
  // Returns all comands and their descriptions
  // PROCEED
  // CLEAR
  // HELP
  // POP_EVENT
  // LIST_AGENTS
  // LIST_PARAMETER_ALIASES
  // UPDATE_AGENT_PARAMETER
  // UPDATE_AGENT_LABEL
  // UPDATE_AGENT_RELATION
  // ADD_AGENT_TAG
  // REMOVE_AGENT_TAG
  // DESCRIBE_AGENT
  // DESCRIBE_AGENT_RELATION
  // LIST_EVENT_TEMPLATES
  // DESCRIBE_EVENT_TEMPLATE
  // LIST_STORIES
  // SELECT_STORY
  // NEW_STORY
  // DEL_STORY
  std::string ret = "\n";
  ret += "JSON <json_string>\n";
  ret += "PROCEED <time>\n";
  ret += "CLEAR\n";
  ret += "HELP\n";
  ret += "POP_EVENT <amount>\n";
  ret += "LIST_AGENTS\n";
  ret += "LIST_PARAMETER_ALIASES\n";
  ret += "UPDATE_AGENT_PARAMETER <agent_id> <parameter> <value>\n";
  ret += "UPDATE_AGENT_LABEL <agent_id> <key> <value>\n";
  ret += "UPDATE_AGENT_RELATION <agent_id> <other_agent_id> <parameter> <value>\n";
  ret += "ADD_AGENT_TAG <agent_id> <tag>\n";
  ret += "REMOVE_AGENT_TAG <agent_id> <tag>\n";
  ret += "DESCRIBE_AGENT <agent_id>\n";
  ret += "DESCRIBE_AGENT_RELATION <agent_id> <other_agent_id>\n";
  ret += "LIST_EVENT_TEMPLATES\n";
  ret += "DESCRIBE_EVENT_TEMPLATE <event_template_id>\n";
  ret += "LIST_STORIES\n";
  ret += "SELECT_STORY <story_id>\n";
  ret += "NEW_STORY\n";
  ret += "DEL_STORY <story_id>\n";
  ret += "EXIT\n";
  ret.pop_back();
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
  std::string ret = "";
  for (auto &agent : story.agents)
  {
    ret += std::to_string(agent.first) + "\n";
  }
  ret.pop_back();
  return ret;
}
std::string API::LIST_PARAMETER_ALIASES()
{
  Story &story = getCurrentStory();
  std::string ret = "\n";
  for (auto &alias : story.parameter_aliases)
  {
    ret += alias.first + " : \"" + alias.second.expression_string + "\"\n";
  }
  ret.pop_back();
  return ret;
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
std::string API::UPDATE_AGENT_RELATION(int agent_id, int other_agent_id, std::string parameter, double value)
{
  Story &story = getCurrentStory();
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else if (story.agents.find(other_agent_id) == story.agents.end())
    return "Other agent not found";
  else if (story.relation_default.find(parameter) == story.relation_default.end())
    return "Parameter not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    agent.relationships[other_agent_id][parameter] = value;
    return "OK";
  }
}
std::string API::DESCRIBE_AGENT(int agent_id)
{
  Story &story = getCurrentStory();
  std::string ret = "";
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
      ret += "PARAMETER_ALIAS " + alias.first + " " + alias.second.evaluate(agent).toString() + "\n";
    }
    for (auto &label : agent.labels)
    {
      ret += "LABEL " + label.first + " : " + label.second + "\n";
    }
    for (auto tag_entry : agent.tags)
    {
      ret += "TAG " + tag_entry + "\n";
    }
    ret.pop_back();
    return ret;
  }
}
std::string API::DESCRIBE_AGENT_RELATION(int agent_id, int other_agent_id)
{
  Story &story = getCurrentStory();
  std::string ret = "\n";
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else if (story.agents.find(other_agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent1 = story.agents[agent_id];
    agent1.other_agent_id = other_agent_id;
    for (auto &relation_entry : story.relation_default)
    {
      ret += "RELATION " + relation_entry.first + " " + agent1.getVariable(relation_entry.first).toString() + "\n";
    }
    return ret;
  }
}
std::string API::UPDATE_AGENT_LABEL(int agent_id, std::string key, std::string value)
{
  bool to_remove = (value.size() == 0);
  if (key.size() == 0)
    return "Key cannot be empty";
  Story &story = getCurrentStory();
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    if (to_remove)
    {
      if (agent.labels.find(key) == agent.labels.end())
        return "Label not found";
      else
      {
        agent.labels.erase(key);
        return "OK";
      }
    }
    else
    {
      agent.labels[key] = value;
      return "OK";
    }
  }
}
std::string API::ADD_AGENT_TAG(int agent_id, std::string tag)
{
  Story &story = getCurrentStory();
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    agent.tags.insert(tag);
    return "OK";
  }
}
std::string API::REMOVE_AGENT_TAG(int agent_id, std::string tag)
{
  Story &story = getCurrentStory();
  if (story.agents.find(agent_id) == story.agents.end())
    return "Agent not found";
  else
  {
    Agent &agent = story.agents[agent_id];
    if (agent.tags.find(tag) == agent.tags.end())
      return "Tag not found";
    else
    {
      agent.tags.erase(tag);
      return "OK";
    }
  }
}
std::string API::LIST_EVENT_TEMPLATES()
{
  Story &story = getCurrentStory();
  std::string ret = "\n";
  for (auto &event_template_entry : story.event_templates)
  {
    ret += std::to_string(event_template_entry.first) + " " + event_template_entry.second.pretty_name + "\n";
  }
  ret.pop_back();
  return ret;
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
    switch (event_template.type)
    {
    case EVENT_TYPE_SELF:
      ret += "TYPE self\n";
      break;
    case EVENT_TYPE_UNIDIRECTIONAL:
      ret += "TYPE unidirectional\n";
      break;
    case EVENT_TYPE_BIDIRECTIONAL:
      ret += "TYPE bidirectional\n";
      break;
    }
    ret += "EXPRESSION \"" + event_template.expression.expression_string + "\"\n";
    for (auto label_entry : event_template.labels)
    {
      ret += "LABEL " + label_entry.first + " : " + label_entry.second + "\n";
    }
    for (auto tag_entry : event_template.tags)
    {
      ret += "TAG " + tag_entry + "\n";
    }
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