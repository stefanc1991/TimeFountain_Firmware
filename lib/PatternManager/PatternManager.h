#pragma once

#include "Pattern.h"
#include <ArduinoJson.h>
#include "LittleFsHelper.h"
#include <vector>
#include <PatternInfoStructs.h>

class PatternManager {

public:
  PatternManager(size_t nMaxBackups);
  ~PatternManager();

  bool loadPatterns(const String filename);
  bool savePatterns(const String filename);

  String getActivePatternJson();
  
  bool selectPattern(size_t iPattern, Pattern*& pActivePattern);
  bool nextPattern(Pattern*& pActivePattern);
  bool previousPattern(Pattern*& pActivePattern);
  
  void freezeActivePattern();
  void unfreezeActivePattern();

  void setPeriod(uint16_t period);
  bool restorePatterns(Pattern*& pActivePattern);
  bool hasPatternSelectionChanged();

  bool insertPattern(const PatternInfo& newPatternInfo, Pattern*& pActivePattern);
  bool modifyPattern(const PatternInfo& newPatternInfo, Pattern*& pActivePattern);
  bool moveActivePatternBy(int8_t steps, Pattern*& pActivePattern);
  bool deleteActivePattern(Pattern*& pActivePattern);

private:
  
  void _addBackup(const std::vector<PatternInfo>& patternInfo);
  void _createActivePattern(Pattern*& pActivePattern);

  std::vector<PatternInfo> _patternInfos;
  std::vector<std::vector<PatternInfo>> _patternInfoBackups;
  Pattern* _pActivePattern;
  size_t _iActivePattern = 0;
  bool _frozen = false;
  uint16_t _tUsPeriod;
  size_t _nMaxBackups = 1;
  bool _patternSelectionChanged = false;
};