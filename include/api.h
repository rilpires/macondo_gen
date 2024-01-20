#ifndef API_H
#define API_H

#include "story.h"
#include "utils.h"
#include <functional>

class API
{
private:
  std::unordered_map<int, Story> stories;
  std::function<void()> on_exit = []() {};
  std::function<void(std::string)> on_output = [](std::string s) {};
  int current_story_id = 0;

public:
  API();

  void onExit(std::function<void()> callback)
  {
    on_exit = callback;
  }
  void onOutput(std::function<void(std::string)> callback)
  {
    on_output = callback;
  }

  // All these below are callable by "command" above
  std::string HELP();
  std::string LIST_STORIES();
  std::string SELECT_STORY(int story_id);
  std::string NEW_STORY();
  std::string DEL_STORY(int story_id);
  std::string JSON(std::string json_string);
  std::string PROCEED(double time);
  std::string CLEAR();
  std::string POP_EVENT(int amount = 1);
  std::string LIST_AGENTS();
  std::string LIST_EVENTS();
  std::string LIST_EVENT_TEMPLATES();
  std::string LIST_PARAMETER_ALIASES();
  std::string UPDATE_AGENT_PARAMETER(int agent_id, std::string parameter, double value);
  std::string DESCRIBE_AGENT(int agent_id);
  std::string DESCRIBE_EVENT_TEMPLATE(int event_template_id);
  std::string EXIT();

  std::string command(std::string input);
  API &operator<<(std::string line);
  Story &getCurrentStory();
};

#endif // !API_H