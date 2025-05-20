#include "assistant_definitions.h"

// Assistant model & name
String assistantModel = "gpt-4o-mini-2024-07-18";  // "gpt-4o";
String assistantName = "TimeFountainPatternAssistant";

String assistantDescription = R"(
You are a friendly assistant that helps the user creating, modifying, deleting and moving light patterns running on his Time Fountain device.
You will listen to the users wishes and fullfill them by calling the tools provided to you. Never send any JSONs to the user directly. Only briefly mention what you did.
Speak the language in which you are adressed by the user. Talk like you are the Time Fountain.
If the 'retrievePatternFlag' is added to the user message, it indicates the user has changed the pattern manually and you shall always call the 'getSelectedPatern' tool first. Otherwise it is up to your own choice.

The patterns are descibed in JSON files and use abbreviations for objects and arrays. 
Always use these abbreviations when creating or modifying a pattern. 
Each pattern consists of one or multiple WaterStreams(WS) which might also be called waves or lightwaves by the user. 
A waterstream is defined by a DutyCycle(DC) and an InitialPhaseShift(IPS) and has at least one TopColorPhase(TCP), one BottomColorPhase(BCP), one MotionPhases(MP) and one VisibilityPhases(VP).
All phases consist of three parameters as shown below. The examplary Json is commented for better understanding.
If properties of a new WaterStream are not specified, try to copy them from the existing Waterstreams.
Never create a WaterStream with the same InitailPhaseShift as an existing WaterStream unless you are told to.
Develop your own patterns accordingly, but never comment them!

[...hier bleibt dein JSON-Beispiel unverändert...]
)";

String toolGetSelectedPattern = R"(
{
  "type": "function",
  "function": {
    "name": "getSelectedPattern",
    "description": "Returns the JSON string decribing the selected Pattern",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {},
      "required": [],
      "additionalProperties": false
    }
  }
}
)";

String toolCreatePattern = R"(
{
  "type": "function",
  "function": {
    "name": "createPattern",
    "description": "Creates a new pattern based on the provided JSON string",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {
        "patternJson": {
          "type": "string",
          "description": "A JSON string folllowing the gíven format that defines the pattern to create"
        }
      },
      "required": ["patternJson"],
      "additionalProperties": false
    }
  }
}
)";

String toolModifySelectedPattern = R"(
{
  "type": "function",
  "function": {
    "name": "modifySelectedPattern",
    "description": "Modifies the selected pattern based on the provided JSON string",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {
        "patternJson": {
          "type": "string",
          "description": "A JSON string folllowing the gíven format that defines the new pattern"
        }
      },
      "required": ["patternJson"],
      "additionalProperties": false
    }
  }
}
)";

String toolDeleteSelectedPattern = R"(
{
  "type": "function",
  "function": {
    "name": "deleteSelectedPattern",
    "description": "Deletes the selected Pattern",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {},
      "required": [],
      "additionalProperties": false
    }
  }
}
)";

String toolMoveSelectedPattern = R"(
{
  "type": "function",
  "function": {
    "name": "moveSelectedPattern",
    "description": "Moves a pattern in a list by the passed number of steps. Positive is forwards, Negative is backwards",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {
        "steps": {
          "type": "number",
          "description": "Number of steps"
        }
      },
      "required": ["steps"],
      "additionalProperties": false
    }
  }
}
)";

String toolUndo = R"(
{
  "type": "function",
  "function": {
    "name": "undo",
    "description": "Undoes the last changes and reverts to the last version of the pattern. Can be called mutiple times",
    "strict": true,
    "parameters": {
      "type": "object",
      "properties": {},
      "required": [],
      "additionalProperties": false
    }
  }
}
)";
