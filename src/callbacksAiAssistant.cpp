#include "callbacksAiAssistant.h"

// general tool callback for the AI interface, all possible tool calls have to be handled here
// To do: still far from perfect as the tools have to be defined seperately in assistant_definitions.h
// modify, delete and move tools allways apply to the running pattern
String toolCallback(const String& toolName, const String& parameters) {

  JsonDocument jsonDoc;

  DeserializationError error = deserializeJson(jsonDoc, parameters);
  if (error) {
    return "Error parsing JSON: " + String(error.c_str());
  }

  if (toolName == "getSelectedPattern") {

    if (pActivePattern) {
      return patternManagerPlay.getActivePatternJson();
    }
    else {
      return "fail";
    }
  }

  else if (toolName == "createPattern") {
    if (!jsonDoc["patternJson"].is<String>()) {
      return "Error: Missing required parameter 'patternJson' for createPattern";
    }

    String patternJson = jsonDoc["patternJson"];

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, patternJson);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return "Json invalid! Try again!";
    }

    PatternInfo patternInfo = PatternInfo::fromJson(jsonDoc.as<JsonObject>());
    if (patternManagerPlay.insertPattern(patternInfo, pActivePattern)) {
      restartLights();
      return "ok";
    }

    return "fail";
  }

  else if (toolName == "modifySelectedPattern") {
    if (!jsonDoc["patternJson"].is<String>()) {
      return "Error: Missing required parameter 'patternJson' for modifySelectedPattern";
    }

    String patternJson = jsonDoc["patternJson"];

    JsonDocument jsonDoc;
    DeserializationError error = deserializeJson(jsonDoc, patternJson);

    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.c_str());
      return "Json invalid! Try again!";
    }

    PatternInfo patternInfo = PatternInfo::fromJson(jsonDoc.as<JsonObject>());
    if (patternManagerPlay.modifyPattern(patternInfo, pActivePattern)) {
      restartLights();
      Serial.println("TEST");
      return "ok";
    }

    return "fail";
  }

  else if (toolName == "deleteSelectedPattern") {

    if (patternManagerPlay.deleteActivePattern(pActivePattern)) {
      restartLights();
      return "ok";
    }

    return "fail";
  }

  else if (toolName == "moveSelectedPattern") {
    if (!jsonDoc["steps"].is<signed int>()) {
      return "Error: Missing required parameter 'steps' for movePattern";
    }

    int steps = jsonDoc["steps"];

    if (patternManagerPlay.moveActivePatternBy(steps, pActivePattern)) {
      restartLights();
      return "ok";
    }

    return "fail";
  }

  else if (toolName == "undo") {
    // To Do: same as in undoLastChangesCallback() that is triggered by the Undo button on Webpage
    if (patternManagerPlay.restorePatterns(pActivePattern)) {
      restartLights();
      return "ok";
    }

    return "fail";
  }

  else {
    return "Unknown tool: " + toolName;
  }
}

// button callback for web interface
bool undoLastChangesCallback() {
  if (patternManagerPlay.restorePatterns(pActivePattern)) {
    restartLights();
    return true;
  }
  return false; 
}

// button callback for web interface
void saveChangesCallback() {
  elMagnet.setState(OFF);
  while (!elMagnet.isTargetPowerReached()) {
      delay(500);
  }
  patternManagerPlay.savePatterns(PLAY_PATTERNS_FILE);
  elMagnet.setState(ON);
}

// callback for received Web interface messages
// To Do: This whole solution is far from ideal, Webpage interaction and AI messages are still mixed. That has to be changed
void aiChatMessageCallback(String message) {

  if (message == "<connect>") {
    Serial.println("Connecting to AI assistant...");
    WebPageAiChat.sendResponseToClient("Connecting to AI assistant...");

    if (!openAiApi.searchAssistant(assistantName, pAssistant)) {
      Serial.println("Assistant not found. Creating...");
      WebPageAiChat.sendResponseToClient("Assistant not found. Creating...");

      if (openAiApi.createAssistant(assistantModel, assistantName, pAssistant)) {
        pAssistant->setInstructions(assistantDescription);
        pAssistant->setTemperature(0.2);
        pAssistant->setTopP(1.0);
        pAssistant->addTool(toolGetSelectedPattern);
        pAssistant->addTool(toolCreatePattern);
        pAssistant->addTool(toolModifySelectedPattern);
        pAssistant->addTool(toolDeleteSelectedPattern);
        pAssistant->addTool(toolMoveSelectedPattern);
      }
    }

    if (!pAssistant) {
      Serial.println("Assistant connection failed");
      WebPageAiChat.sendResponseToClient("Assistant connection failed");
      return;
    }

    pAssistant->setToolCallback(toolCallback);

    Serial.println("Assistant Connected");
    WebPageAiChat.sendResponseToClient("Assistant Connected");

    String runResult = pAssistant->sendMessage("Hi");
    WebPageAiChat.sendResponseToClient("<clear>");
    WebPageAiChat.sendResponseToClient(runResult);
    WebPageAiChat.sendResponseToClient("<ready>");
  }
  else {
    patternManagerPlay.freezeActivePattern();
    if (patternManagerPlay.hasPatternSelectionChanged()) {
      message += " (retrievePatternFlag=true)";              // if the Pattern was changed by the user, the LLM has to retrieve the active pattern before continuing
    }
    String runResult = pAssistant->sendMessage(message);
    WebPageAiChat.sendResponseToClient(runResult);
    WebPageAiChat.sendResponseToClient("<ready>");
    patternManagerPlay.unfreezeActivePattern();
  }
}
