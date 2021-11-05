#ifndef _FILE_UTILS_H
#define _FILE_UTILS_H

#include <string>

std::string replaceExtension(const std::string& input, const std::string& newExt);
std::string getBaseName(const std::string& input);

std::string DirectoryName(const std::string& filename);
std::string Join(const std::string& a, const std::string& b);

#endif