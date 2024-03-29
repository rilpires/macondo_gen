# macondogen: Storytelling API engine

The proposal is to run an engine that generates "story" events from a set of initial parameters, but also from updated parameters as the user interacts with the story. My primary objective was to create a tool for inserting procedural storytelling into games (that's why it is very needed that parameters could be updated in runtime), but it can be used for other purposes.

As for now, it is used as a REPL to interact with the engine. I plan to create a proper library in the future.

# Motivation
Just as a heads up, I don't have a big background in AI nor in storytelling, but I really like the idea of procedural storytelling in games and creating a tool that could be easily integrated into a game engine.

I also really like the idea of creating a tool that could be integrated in a already existing game without big changes in the code.

It won't be by any means a "plug & play" tool, because this is mainly a program that generates events based on a set of parameters and mathematical expressions that fits the user's needs. I hope, however, that it in the future, as users (including me) learns how to properly create expressions that dictates a good story, we could move forward to a more "plug & play" generic version.

# How it works
Here I'll try to explain the main concepts of the engine, and how it works.

## Time
Time is the main concept of the engine. It is the only thing that moves the engine forward. It is a float number that starts at 0 and goes up to infinity. Events only happen when time passes. So the user needs to periodically proceed the time with a propper call to the engine. When time passes (and if there was any event generated), events can be "POP"ed from the engine. **What exactly 1 time unit means is up to the user. I.e., 1 time unit could be 1 second, 1 minute, 1 hour, 1 day, 1 year, etc. in the story. This is very important to keep in mind,** since event expressions dictate how much time between events, in average, will happen.

In a more mathematical way, all events will happen as samples from a exponential distribution, where the mean is the expression result. I.e., if the expression result is 0.5, in average, 1 event will happen every 2 time units. If the expression result is 0.1, in average, 1 event will happen every 10 time units.

## Agents

Agents are the main concept of the engine. They are the ones that will generate the events. They have a set of parameters that will dictate how they will behave. The parameters are simply a set of key-value pairs, where the key is a string and the value is a float number. I hardly recommend that the values are between 0 and 1, although it is not mandatory.

What exactly each parameter means is up to the user. The engine will only use the parameters to generate the events. I.e., the user can create a parameter called "hunger" and use it to generate events related to the agent being hungry.

Agents can also have a set of key-value pairs of labels. Labels are used to identify the agent. I.e., the user can create a label called "name" and use it to identify the agent. Without labels, the only identifier an agent has is its id.

Agents also have a set of parameters for each other agent. I.e., if agent A has a parameter called "friendship". These parameters can't have the same name as the agent's parameters. I.e., if agent A has a parameter called "friendship", it can't have a parameter called "friendship" used for relations.

## Event Templates

If there is no event template, there can't be any event being generated. Event templates are defined by the user, and they are composed always by three arguments:

#### Name
This simply is the name of the event. I.e., "peaceful_thoughts" or "fight".

#### Type
There is only two types of events: A "self" event, which is an event that is generated by the agent itself, or a "relation" event, which is an event that is generated by the relation between two agents. I.e., an agent with parameter "angry=0" could generate a "self" event called "peaceful_thoughts". If this agent has "angry=0.92", it could generate a "relation" event called "fight" with another agent, especially if this other agent has "angry=0.78" and the relation parameter "friendship" from the first agent to the second is "0.02".

#### Expression
This is the most important part of the event template. It is a mathematical expression that will dictate if the event will be generated or not. It is composed by a set of parameters and mathematical operators. Examples of expressions are:

```
 "hunger"
 "hunger + 0.2"
 "angry * hunger - 0.2"
 "angry * (hunger-0.2) - 0.2"
 "10*(self.angry * other.angry)"
```

This can look a little bit confusing at first, but it is actually very simple: the higher the expression result, the higher the chance of the event being generated. The expression result, if positive, is the **AVERAGE** interval of time that will pass until the event is generated (remember that the time is a exponential distribution).

Expressions access the agent parameters by simply using the parameter name. I.e., if the agent has a parameter called "hunger" and another called "angry", the expression can use both by simply using "hunger" and "angry". If the agent has a parameter called "hunger" and another called "angry", and the event is a "relation" event, the expression can use both by simply using "self.hunger", "self.angry", "other.hunger" and "other.angry".

There is also a set of mathematical operators and functions that can be used in the expression:
```
TO-DO
```

## Events

Events are simply composed by the time they were generated, the ID of the Event Template that generated them, and the ID of the agent that generated them. If the event is a "relation" event, it also has the ID of the other agent.

So, a tipical result for the "POP_EVENT 3" (pop up to 2 next events, if available) command would be something like this:
```
0.004 1 1
0.155 2 10 11
```

It means that in time 0.004, the agent with ID 1 generated a self event from the event template with ID=1. In time 0.155, the agent with ID 2 generated a relation event from the event template with ID 10 towards the agent with ID 11.

# How to use
The engine is a REPL. More documentation will be added in the future, but for now, you can try the `HELP` command to see the available commands.

It can be started as a TCP server, if the program is initialized with the argument `--tcp` or `-t`.<br>
To specify the port, use the argument `--port` or `-p` (default is 7776).<br>
I.e., `./macondogen --tcp --port 1234`.

## Building a story with a set of agent and event templates
As for now, the only way to properly build a story is by passing a JSON string with the "JSON \<string>" command.
A simple valid JSON is:
``` json
{
  "version": "0.0",
  "agents": [
    {
      "labels": {
        "name": "poor_man"
      },
      "parameters": {
        "envyness": 0.5,
        "wealth": 0.0,
        "dresscode": 0.0,
      }
    },
    {
      "labels": {
        "name": "rich_man"
      },
      "parameters": {
        "envyness": 0.7,
        "wealth": 1.0,
        "dresscode": 1.0,
      }
    }
  ],
  "parameter_aliases": [
    {
      "name": "rebelness",
      "expression": "(1.0-wealth)*envyness"
    }
  ],
  "relations": [
    {
      "name": "friendship",
      "default": 0.5
    },
    {
      "name": "respect",
      "default": 0.1
    }
  ],
  "event_templates": [
    {
      "name": "pitiful_speech",
      "type": "relation",
      "expression": "(self.wealth - other.wealth)"
    },
    {
      "name": "flipped",
      "type": "self",
      "expression": "(1.0 - wealth)"
    }
  ]
}
```
Look at the "stories/" folder for more examples.

## TODO
- There is some basic commands that are not implemented yet (like creating agents runtime)
- Add more documentation on commands and features
- Add more mathematical operators and functions
- Add more examples
- Add more tests
- Create a proper library
- Feature: Automatically running update commands based on events, if desired
- Feature: Configuration options for default parameters and relations, as well as support for defining default parameters and relations for a class of agents
- Feature: More options for skipping calculations, i.e., if the agent is not in the same place as the other agent, it doesn't need to calculate the relation between them. This is a must for performance.