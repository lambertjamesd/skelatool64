#include "StringUtils.h"

#include <sstream>

bool IsWordCharacter(char input) {
    return isalnum(input) || input == '_';
}

std::string FindAndReplace(const std::string& source, const std::string& searchString, const std::string& replaceString, bool wholeWord) {
    std::ostringstream result;

    unsigned next = source.find(searchString);
    unsigned last = 0;

    while (next < source.length()) {
        result << source.substr(last, next - last);
        
        bool shouldReplace = true;
        unsigned afterNext = next + searchString.length();

        if (wholeWord) {
            if (next > 0 && IsWordCharacter(source[next - 1])) {
                shouldReplace = false;
            }

            if (afterNext < source.length() && IsWordCharacter(source[afterNext])) {
                shouldReplace = false;
            }
        }

        if (shouldReplace) {
            result << replaceString;
        } else {
            result << searchString;
        }

        last = afterNext;
        next = source.find(searchString, last);
    }

    result << source.substr(last, source.length() - last);

    return result.str();
}

std::string Indent(const std::string& input, const std::string& whitespace) {
    std::ostringstream result;

    unsigned lineStart = 0;
    bool lookingForLineStart = true;

    for (unsigned curr = 0; curr <= input.length(); ++curr) {
        char currentChar = curr < input.length() ? input[curr] : '\0';

        if (lookingForLineStart) {
            if (!isspace(currentChar)) {
                lineStart = curr;
                lookingForLineStart = false;
                result << whitespace;
            }
        } else {
            if (currentChar == '\n' || currentChar == '\r' || currentChar == '\0') {
                result << input.substr(lineStart, curr - lineStart) << std::endl;
                lookingForLineStart = true;
            }
        }
    }

    return result.str();
}

std::string Trim(const std::string& input) {
    int start = 0;
    
    while (start < (int)input.length() && isspace(input[start])) {
        ++start;
    }

    int end = input.length() - 1;

    while (end >= 0 && isspace(input[end])) {
        --end;
    }

    ++end;

    if (start >= end) {
        return "";
    }

    return input.substr(start, end - start);
}

void makeCCompatible(std::string& target) {
    for (unsigned int i = 0; i < target.length(); ++i) {
        char curr = target[i];

        if (!(curr >= 'a' && curr <= 'z') && !(curr >= 'A' && curr <= 'Z') && !(curr >= '0' && curr <= '9') && curr != '_') {
            target[i] = '_';
        }
    }

    if (target.length() > 0 && target[0] >= '0' && target[0] <= '9') {
        target = '_' + target;
    }
}