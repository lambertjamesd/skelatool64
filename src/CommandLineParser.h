#ifndef _COMMAND_LINE_PARSER_H
#define _COMMAND_LINE_PARSER_H

#include <string>
#include <vector>
#include <string.h>
#include <iostream>

struct CommandLineArguments {
    std::string mInputFile;
    std::string mOutputFile;
    std::string mPrefix;
    std::vector<std::string> mMaterialFiles;
    float mScale;
    bool mExportAnimation;
    bool mExportGeometry;
    bool mIsLevel;
    bool mIsLevelDef;
};

bool parseCommandLineArguments(int argc, char *argv[], struct CommandLineArguments& output);

#endif