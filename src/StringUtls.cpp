#include "StringUtils.h"

#include <sstream>

std::string FindAndReplace(const std::string& source, const std::string& searchString, const std::string& replaceString) {
    std::ostringstream result;

    unsigned next = source.find(searchString);
    unsigned last = 0;

    while (next < source.length()) {
        result << source.substr(last, next - last);
        result << replaceString;
        last = next + searchString.length();
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