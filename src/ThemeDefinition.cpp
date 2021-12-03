 #include "ThemeDefinition.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include "StringUtils.h"
#include "FileUtils.h"

void parseLevelThemeDefintionFromYaml(const YAML::Node& node, const std::string& relativeDir, LevelThemeDefinition& output) {
    output.mName = node["Name"].Scalar();
    output.mCName = output.mName;
    makeCCompatible(output.mCName);
    output.mFilename = Join(relativeDir, node["Filename"].Scalar());
    output.mOutput = Join(relativeDir, node["Output"].Scalar());
    
    output.mMaxPlayers = atoi(node["MaxPlayers"].Scalar().c_str());

    if (node["Campaign"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsCampaign");
    }

    if (node["Multiplayer"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsMultiplayer");
    }

    if (node["Unlocked"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsUnlocked");
    }

    if (node["Tutorial"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsTutorial");
    }

    if (node["Tutorial2"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsTutorial2");
    }

    if (node["DisableItems"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsDisableItems");
    }

    if (node["DisallowUpgrade"].IsDefined()) {
        output.mFlags.push_back("LevelMetadataFlagsDisallowUpgrade");
    }

    if (node["AIDifficulty"].IsDefined()) {
        output.mAIDifficulty = atof(node["AIDifficulty"].Scalar().c_str());
    } else {
        output.mAIDifficulty = 1.0f;
    }
}

void parseSingleThemeDefinitionFromYaml(const YAML::Node& node, const std::string& relativeDir, ThemeDefinition& output) {
    output.mName = node["Name"].Scalar();
    output.mCName = output.mName;
    makeCCompatible(output.mCName);
    output.mOutput = Join(relativeDir, node["Output"].Scalar());

    const YAML::Node& levels = node["Levels"];

    for (unsigned i = 0; i < levels.size(); ++i) {
        LevelThemeDefinition level;
        parseLevelThemeDefintionFromYaml(levels[i], relativeDir, level);
        output.mLevels.push_back(level);
    }
}

void parseThemeDefinitionFromYaml(const YAML::Node& node, const std::string& relativeDir, ThemeDefinitionList& output) {
    output.mLevelList = Join(relativeDir, node["LevelList"].Scalar());
    output.mThemeList = Join(relativeDir, node["ThemeList"].Scalar());
    const YAML::Node& themes = node["Themes"];

    for (unsigned i = 0; i < themes.size(); ++i) {
        ThemeDefinition themeDef;
        parseSingleThemeDefinitionFromYaml(themes[i], relativeDir, themeDef);
        output.mThemes.push_back(themeDef);
    }
}

void parseThemeDefinition(const std::string& filename, ThemeDefinitionList& output) {
    std::fstream file(filename, std::ios::in);

    try {
        YAML::Node doc = YAML::Load(file);
        parseThemeDefinitionFromYaml(doc, DirectoryName(filename), output);
    } catch (YAML::ParserException& e) {
        std::cerr << "Error parsing '" << filename << "' "  << e.what() << std::endl;
    }
}