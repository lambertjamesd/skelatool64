
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

std::string DirectoryName(const std::string& filename) {
    std::size_t correctSlash = filename.rfind('/');
    std::size_t wrongSlash = filename.rfind('\\');

    if (correctSlash != std::string::npos && wrongSlash != std::string::npos) {
        correctSlash = std::max(correctSlash, wrongSlash);
    }

    if (correctSlash == std::string::npos) {
        return "";
    }

    return filename.substr(0, correctSlash);
}

std::string Join(const std::string& a, const std::string& b) {
    if (b.length() == 0) {
        return a;
    }

    if (b[0] == '\\' || b[0] == '/') {
        return b;
    }

    std::string firstPath = a;
    std::string secondPath = b;

    while (secondPath.rfind("../", 0) == 0 || secondPath.rfind("..\\", 0) == 0) {
        firstPath = DirectoryName(firstPath);
        secondPath = secondPath.substr(3);
    }

    return firstPath + "/" + secondPath;
}