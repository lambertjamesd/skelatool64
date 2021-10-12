#ifndef _COMMAND_LINE_PARSER_H
#define _COMMAND_LINE_PARSER_H

#include <string>
#include <string.h>
#include <iostream>

struct CommandLineArguments {
    std::string mInputFile;
    std::string mOutputFile;
    std::string mPrefix;
};

bool parseCommandLineArguments(int argc, char *argv[], struct CommandLineArguments& output);

#endif