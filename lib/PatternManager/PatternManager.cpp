#include "PatternManager.h"

PatternManager::PatternManager(size_t nMaxBackups) : _nMaxBackups(nMaxBackups) {}

PatternManager::~PatternManager() {
  delete _pActivePattern;
}

// Creates and initializes the current active (selected) pattern
void PatternManager::_createActivePattern(Pattern*& pActivePattern) {
  if (_patternInfos.empty() || _iActivePattern >= _patternInfos.size()) {
    delete _pActivePattern;
    _pActivePattern = nullptr;
    return;
  }

  Pattern* pNewPattern = new Pattern(Pattern::fromStruct(_patternInfos[_iActivePattern]));
  pNewPattern->setPeriod(_tUsPeriod);
  pNewPattern->init();

  pActivePattern = pNewPattern;

  delete _pActivePattern;
  _pActivePattern = pNewPattern;
}

bool PatternManager::selectPattern(size_t iNewPattern, Pattern*& pActivePattern) {
  if (_frozen || iNewPattern >= _patternInfos.size()) return false;

  _iActivePattern = iNewPattern;
  _createActivePattern(pActivePattern);
  _patternSelectionChanged = true;

  return true;
}

bool PatternManager::nextPattern(Pattern*& pActivePattern) {
  if (_frozen || _patternInfos.size() < 2) return false;

  _iActivePattern = (_iActivePattern + 1) % _patternInfos.size();
  _createActivePattern(pActivePattern);
  _patternSelectionChanged = true;

  return true;
}

bool PatternManager::previousPattern(Pattern*& pActivePattern) {
  if (_frozen || _patternInfos.size() < 2) return false;

  _iActivePattern = (_iActivePattern + _patternInfos.size() - 1) % _patternInfos.size();
  _createActivePattern(pActivePattern);
  _patternSelectionChanged = true;

  return true;
}

void PatternManager::setPeriod(uint16_t tUsPeriod) {
  _tUsPeriod = tUsPeriod;
  if (_pActivePattern) {
    _pActivePattern->setPeriod(tUsPeriod);
  }
}

bool PatternManager::restorePatterns(Pattern*& pActivePattern) {
  if (_patternInfoBackups.empty()) return false;

  _patternInfos = _patternInfoBackups.back();
  _patternInfoBackups.pop_back();

  _iActivePattern %= _patternInfos.size();
  _createActivePattern(pActivePattern);
  _patternSelectionChanged = true;

  return true;
}

bool PatternManager::hasPatternSelectionChanged() {
  bool tmp = _patternSelectionChanged;
  _patternSelectionChanged = false;
  return tmp;
}

bool PatternManager::insertPattern(const PatternInfo& newPatternInfo, Pattern*& pActivePattern) {
  _addBackup(_patternInfos);
  _patternInfos.insert(_patternInfos.begin() + _iActivePattern, newPatternInfo);
  _createActivePattern(pActivePattern);
  return true;
}

bool PatternManager::modifyPattern(const PatternInfo& newPatternInfo, Pattern*& pActivePattern) {
  _addBackup(_patternInfos);
  _patternInfos[_iActivePattern] = newPatternInfo;
  _createActivePattern(pActivePattern);
  return true;
}

bool PatternManager::moveActivePatternBy(int8_t steps, Pattern*& pActivePattern) {
  if (_patternInfos.empty()) return false;

  _addBackup(_patternInfos);

  int newIndex = (_iActivePattern + _patternInfos.size() + steps) % _patternInfos.size();

  PatternInfo currentPattern = _patternInfos[_iActivePattern];
  _patternInfos.erase(_patternInfos.begin() + _iActivePattern);
  _patternInfos.insert(_patternInfos.begin() + newIndex, currentPattern);

  _iActivePattern = newIndex;
  _createActivePattern(pActivePattern);

  return true;
}

bool PatternManager::deleteActivePattern(Pattern*& pActivePattern) {
  if (_patternInfos.size() < 2) return false;

  _addBackup(_patternInfos);

  _patternInfos.erase(_patternInfos.begin() + _iActivePattern);
  _iActivePattern %= _patternInfos.size();
  _createActivePattern(pActivePattern);
  _patternSelectionChanged = true;

  return true;
}

// Backs up current pattern state, maintaining backup size limit
void PatternManager::_addBackup(const std::vector<PatternInfo>& patternInfo) {
  if (_patternInfoBackups.size() >= _nMaxBackups) {
    _patternInfoBackups.erase(_patternInfoBackups.begin());
  }
  _patternInfoBackups.push_back(patternInfo);
}

void PatternManager::freezeActivePattern() {
  _frozen = true;
}

void PatternManager::unfreezeActivePattern() {
  _frozen = false;
}

// Estimates RAM usage of a pattern (for debugging)
size_t calculateMemoryUsage(const PatternInfo& pattern) {
  size_t totalSize = sizeof(pattern);

  for (const auto& stream : pattern.streams) {
    totalSize += sizeof(stream);
    totalSize += stream.topColorPhases.capacity() * sizeof(ColorPhaseInfo);
    totalSize += stream.motionPhases.capacity() * sizeof(MotionPhaseInfo);
    totalSize += stream.visibilityPhases.capacity() * sizeof(VisibilityPhaseInfo);
    totalSize += stream.botColorPhases.capacity() * sizeof(ColorPhaseInfo);
  }

  return totalSize;
}

bool PatternManager::loadPatterns(const String filename) {
  if (!LittleFsHelper::fileExists(filename))
    return false;

  String fileContent = LittleFsHelper::readFile(filename);

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, fileContent);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }

  JsonArray patterns = doc["Patterns"];
  if (patterns.size() < 1) {
    Serial.println("Pattern file is empty");
    return false;
  }

  _patternInfos.clear();
  _patternInfoBackups.clear();

  for (JsonObjectConst patternObj : patterns) {
    PatternInfo patternInfo = PatternInfo::fromJson(patternObj);
    _patternInfos.push_back(patternInfo);

    // Uncomment to debug memory usage
    // size_t totalMemory = calculateMemoryUsage(patternInfo);
    // Serial.println(totalMemory);
  }

  _patternSelectionChanged = true;
  return true;
}

bool PatternManager::savePatterns(const String filename) {
  if (_patternInfos.empty()) return false;

  JsonDocument doc;
  JsonArray patternsArray = doc["Patterns"].to<JsonArray>();

  for (const auto& patternInfo : _patternInfos) {
    JsonObject patternObj = patternsArray.add<JsonObject>();
    patternInfo.toJson(patternObj);
  }

  String jsonString;
  serializeJson(doc, jsonString);
  return LittleFsHelper::writeFile(filename, jsonString);
}

String PatternManager::getActivePatternJson() {
  JsonDocument doc;

  if (_pActivePattern && _iActivePattern < _patternInfos.size()) {
    JsonObject patternObj = doc.to<JsonObject>();
    _patternInfos[_iActivePattern].toJson(patternObj);
  }

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}
