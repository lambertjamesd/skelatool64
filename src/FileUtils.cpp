
#include "FileUtils.h"
#include <math.h>

std::string replaceExtension(const std::string& input, const std::string& newExt) {
    std::size_t extPos = input.rfind('.');

    if (extPos == std::string::npos) {
        return input + newExt;
    } else {
        return input.substr(0, extPos) + newExt;
    }
}

std::string getBaseName(const std::string& input) {
    std::size_t pathPos = input.rfind('/');
    std::size_t wrongPathPos = input.rfind('\\');

    if (wrongPathPos != std::string::npos && pathPos != std::string::npos) {
        pathPos = std::max(pathPos, wrongPathPos);
    } else if (wrongPathPos != std::string::npos) {
        pathPos = wrongPathPos;
    }

    if (pathPos == std::string::npos) {
        return input;
    } else {
        return input.substr(pathPos + 1);
    }
}