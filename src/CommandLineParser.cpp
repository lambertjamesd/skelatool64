
#include "CommandLineParser.h"

bool parseCommandLineArguments(int argc, char *argv[], struct CommandLineArguments& output) {
    output.mInputFile = "";
    output.mOutputFile = "";
    output.mPrefix = "output";
    output.mScale = 256.0f;

    char lastParameter = '\0';
    bool hasError = false;

    for (int i = 1; i < argc; ++i) {
        char* curr = argv[i];

        if (lastParameter != '\0') {
            switch (lastParameter) {
                case 'o':
                    if (output.mOutputFile != "") {
                        std::cerr << "You can only specify a single output file" << std::endl;
                        hasError = true;
                    }

                    output.mOutputFile = curr;
                    break;
                case 'n':
                    output.mPrefix = curr;
                    break;
                case 's':
                    output.mScale = (float)atof(curr);
                    break;
                case 'm':
                    output.mMaterialFiles.push_back(curr);
                    break;
            }

            lastParameter = '\0';
        } else if (
            strcmp(curr, "-o") == 0 || 
            strcmp(curr, "--output") == 0) {
            lastParameter = 'o';
        } else if (
            strcmp(curr, "-n") == 0 || 
            strcmp(curr, "--name") == 0) {
            lastParameter = 'n';
        } else if (
            strcmp(curr, "-s") == 0 || 
            strcmp(curr, "--scale") == 0) {
            lastParameter = 's';
        } else if (
            strcmp(curr, "-m") == 0 || 
            strcmp(curr, "--materials") == 0) {
            lastParameter = 'm';
        } else {
            if (curr[0] == '-') {
                hasError = true;
                std::cerr << "Unrecognized argument '" << curr << '"' << std::endl;
            } else if (output.mInputFile == "") {
                output.mInputFile = curr;
            } else {
                hasError = true;
                std::cerr << "Only one input file allowed. " << 
                    "Already gave '" << output.mInputFile << "'" <<
                    ". And then got '" << curr << "'" << std::endl;
            }
        }
    }

    if (output.mInputFile == "" || output.mOutputFile == "") {
        std::cerr << "Input and output file are both required" << std::endl;
        hasError = true;
    }

    if (hasError) {
        std::cerr << "usage " << argv[0] << " [ARGS] -o [output-file] [input-file]" << std::endl;
    }

    if (output.mOutputFile.length() > 2 && output.mOutputFile.substr(output.mOutputFile.length() - 2) == ".h") {
        output.mOutputFile = output.mOutputFile.substr(0, output.mOutputFile.length() - 2);
    }

    return !hasError;
}