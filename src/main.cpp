
#include "utils.h"
#include "agent.h"
#include "expression.h"
#include "story.h"
#include "variable.h"

int main()
{

  Story story;
  std::cout << "Macondogen v0.0" << std::endl;
  std::cout << "Type HELP for a list of commands" << std::endl;

  while (true)
  {
    std::string line;
    std::string command;
    std::getline(std::cin, line);
    std::vector<std::string> tokens = Utils::split(line, " ");
    if (tokens.size() == 0)
      continue;
    command = tokens[0];
    command = Utils::toUpper(command);
    if (command == "EXIT" || command == "QUIT")
      break;
    else if (command == "PRINT")
    {
    }
    else if (command == "JSON")
    {
      std::string filename;
      if (tokens.size() > 1)
      {
        filename = tokens[1];
      }
      else
      {
        filename = "stories/politics.json";
      }
      story = Story();
      json j = Utils::loadJSON(filename);
      story.buildFromJSON(j);
    }
    else if (command == "PROCEED")
    {
      if (tokens.size() > 1)
      {
        double time = std::stod(tokens[1]);
        story.proceed(time);
      }
      else
      {
        std::cout << "Missing argument" << std::endl;
      }
    }
    else if (command == "LIST")
    {
      if (tokens.size() > 1)
      {
        std::string type = tokens[1];
        type = Utils::toUpper(type);
        if (type == "AGENTS")
        {
          for (auto &agent : story.agents)
            std::cout << agent.second.id << " | " << agent.second.getName() << std::endl;
        }
        else if (type == "EVENTS")
        {
          for (auto &event : story.events)
            std::cout << event.time << " | " << event._template.id << std::endl;
        }
        else if (type == "EVENT_TEMPLATES")
        {
          for (auto &_template : story.event_templates)
            std::cout << _template.second.id << " | " << _template.second.pretty_name << std::endl;
        }
        else
        {
          std::cout << "Unknown objects to list" << std::endl;
        }
      }
      else
      {
        std::cout << "Missing argument" << std::endl;
      }
    }
    else if (command == "TIME")
    {
      std::cout << story.current_time << std::endl;
    }
    else if (command == "POP")
    {
      int how_many = 0;
      if (tokens.size() > 1)
      {
        if (Utils::toUpper(tokens[1]) == "ALL")
          how_many = story.events.size();
        else
        {
          try
          {
            how_many = std::stoi(tokens[1]);
          }
          catch (const std::exception &e)
          {
            how_many = 0;
          }
        }
      }
      else
        how_many = 1;
      for (
          int i = 0;
          (i < how_many) && (story.events.size() > 0);
          i++)
      {
        Event event = story.events.front();
        story.events.pop_front();
        std::cout << event.buildExplanation() << std::endl;
      }
      std::cout << "Events left: " << story.events.size() << std::endl;
    }
    else if (command == "UPDATE")
    {
      if (tokens.size() == 6 && Utils::toUpper(tokens[1]) == "AGENT" && Utils::toUpper(tokens[2]) == "PARAM")
      {
        int agent_id = std::stoi(tokens[3]);
        std::string parameter_name = tokens[4];
        Variable value = Variable(std::stod(tokens[5]));
        Agent &agent = story.agents[agent_id];
        agent.parameters.emplace(parameter_name, value);
      }
      else if (tokens.size() == 6 && Utils::toUpper(tokens[1]) == "AGENT" && Utils::toUpper(tokens[2]) == "LABEL")
      {
        int agent_id = std::stoi(tokens[3]);
        std::string label_key = tokens[4];
        std::string label_value = tokens[5];
        Agent &agent = story.agents[agent_id];
        agent.labels.emplace(label_key, label_value);
      }
      else if (tokens.size() == 5 && Utils::toUpper(tokens[1]) == "EVENT_TEMPLATE" && Utils::toUpper(tokens[2]) == "EXPRESSION")
      {
        int event_template_id = std::stoi(tokens[3]);
        std::string value = tokens[4];
        if (story.event_templates.find(event_template_id) == story.event_templates.end())
        {
          std::cout << "Event template not found" << std::endl;
          continue;
        }
        EventTemplate &_template = story.event_templates.at(event_template_id);
        _template.expression = Expression(value);
      }
      else
      {
        std::cout << "Usage:" << std::endl;
        std::cout << "  UPDATE AGENT PARAM <agent_id> <parameter_name> <value>" << std::endl;
        std::cout << "  UPDATE AGENT LABEL <agent_id> <label_key> <label_value>" << std::endl;
        std::cout << "  UPDATE EVENT_TEMPLATE EXPRESSION <event_template_id> <value>" << std::endl;
      }
    }
    else if (command == "HELP" || command == "H")
    {
      std::cout << "Available commands:" << std::endl;
      std::cout << "  JSON <filename>" << std::endl;
      std::cout << "  PROCEED <time>" << std::endl;
      std::cout << "  LIST AGENTS" << std::endl;
      std::cout << "  LIST EVENTS" << std::endl;
      std::cout << "  LIST EVENT_TEMPLATES" << std::endl;
      std::cout << "  TIME" << std::endl;
      std::cout << "  POP <number>" << std::endl;
      std::cout << "  UPDATE AGENT PARAM <agent_id> <parameter_name> <value>" << std::endl;
      std::cout << "  UPDATE AGENT LABEL <agent_id> <label_key> <label_value>" << std::endl;
      std::cout << "  UPDATE EVENT_TEMPLATE EXPRESSION <event_template_id> <value>" << std::endl;
      std::cout << "  EXIT" << std::endl;
    }
    else
    {
      std::cout << "Unknown command" << std::endl;
    }
  }

  std::cout << "Bye!" << std::endl;

  return 0;
}