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

  void onExit(std::function<void()> callback);
  void onOutput(std::function<void(std::string)> callback);

  // Context switching
  std::string LIST_STORIES();
  std::string SELECT_STORY(int story_id);
  std::string NEW_STORY();
  std::string DEL_STORY(int story_id);

  // Misc
  std::string HELP();
  std::string JSON(std::string json_string);
  std::string PROCEED(double time);
  std::string CLEAR();
  std::string LIST_PARAMETER_ALIASES();
  std::string POP_EVENT(int amount = 1);
  std::string EXIT();

  /// Agents
  std::string LIST_AGENTS();
  std::string UPDATE_AGENT_PARAMETER(int agent_id, std::string parameter, double value);
  std::string UPDATE_AGENT_RELATION(int agent_id, int other_agent_id, std::string parameter, double value);
  std::string UPDATE_AGENT_LABEL(int agent_id, std::string key, std::string value);
  std::string ADD_AGENT_TAG(int agent_id, std::string tag);
  std::string REMOVE_AGENT_TAG(int agent_id, std::string tag);
  std::string DESCRIBE_AGENT(int agent_id);
  std::string DESCRIBE_AGENT_RELATION(int agent_id, int other_agent_id);

  // EventTemplates
  std::string LIST_EVENT_TEMPLATES();
  std::string DESCRIBE_EVENT_TEMPLATE(int event_template_id);

  std::string command(std::string input);
  API &operator<<(std::string line);
  Story &getCurrentStory();
};

#endif // !API_H