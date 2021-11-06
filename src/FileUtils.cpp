
#include "FileUtils.h"
#include <math.h>
#include <vector>
#include <sstream>

bool isPathCharacter(char chr) {
    return chr == '\\' || chr == '/';
}

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

    if (isPathCharacter(b[0])) {
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

std::vector<std::string> SplitOnFirstPath(const std::string& path) {
    std::size_t lastStart = 0;

    std::vector<std::string> result;

    bool hasMore = true;

    while (hasMore) {
        std::size_t pathPos = path.find('/', lastStart);
        std::size_t wrongPathPos = path.find('\\', lastStart);

        if (pathPos != std::string::npos && wrongPathPos != std::string::npos) {
            pathPos = std::min(pathPos, wrongPathPos);
        }
        
        if (pathPos == std::string::npos) {
            hasMore = false;
            result.push_back(path.substr(lastStart));
        } else {
            if (pathPos != lastStart) {
                result.push_back(path.substr(lastStart, pathPos - lastStart));
            }
            lastStart = pathPos + 1;
        }
    }

    return result;
}

std::string Relative(const std::string& from, const std::string& to) {
    std::vector<std::string> fromPathSplit = SplitOnFirstPath(DirectoryName(from));
    std::vector<std::string> toPathSplit = SplitOnFirstPath(to);

    unsigned commonStart = 0;

    while (commonStart < fromPathSplit.size() && commonStart < toPathSplit.size() && fromPathSplit[commonStart] == toPathSplit[commonStart]) {
        ++commonStart;
    }

    std::ostringstream result;

    for (unsigned i = commonStart; i < fromPathSplit.size(); ++i) {
        result << "../";
    }

    for (unsigned i = commonStart; i < toPathSplit.size(); ++i) {
        result << toPathSplit[i];

        if (i+1 != toPathSplit.size()) {
            result << '/';
        }
    }

    return result.str();
}