#ifndef _THEME_DEFINITION_H
#define _THEME_DEFINITION_H

#include <string>
#include <vector>

class LevelThemeDefinition {
public:
    std::string mName;
    std::string mCName;
    std::string mFilename;
    std::string mOutput;
    unsigned mMaxPlayers;
    float mAIDifficulty;
    
    std::vector<std::string> mFlags;
};

class ThemeDefinition {
public:
    std::string mName;
    std::string mCName;
    std::string mOutput;

    std::vector<LevelThemeDefinition> mLevels;
};

class ThemeDefinitionList {
public:
    std::string mLevelList;
    std::string mThemeList;
    std::vector<ThemeDefinition> mThemes;
};

void parseThemeDefinition(const std::string& filename, ThemeDefinitionList& output);

#endif