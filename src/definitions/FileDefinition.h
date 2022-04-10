#ifndef __FILE_DEFINITION_H__
#define __FILE_DEFINITION_H__

#include <string>
#include <ostream>
#include <set>
#include "DataChunk.h"

class FileDefinition {
public:
    FileDefinition(const std::string& type, const std::string& name, bool isArray, std::string location);

    virtual ~FileDefinition();

    virtual void Generate(std::ostream& output) = 0;
    void GenerateDeclaration(std::ostream& output);

    std::string GetLocation();

    void AddTypeHeader(const std::string& typeHeader);

    const std::set<std::string>& GetTypeHeaders();
protected:
    std::string mType;
    std::string mName;
    bool mIsArray;
    std::string mLocation;

    std::set<std::string> mTypeHeaders;
};

class DataFileDefinition : public FileDefinition {
public:
    DataFileDefinition(const std::string& type, const std::string& name, bool isArray, std::string location, std::unique_ptr<DataChunk> data);

    virtual void Generate(std::ostream& output);
private:
    std::unique_ptr<DataChunk> mData;
};

class RawFileDefinition : public FileDefinition {
public:
    RawFileDefinition(const std::string& type, const std::string& name, bool isArray, std::string location, const std::string& content);

    virtual void Generate(std::ostream& output);
private:
    std::string mContent;
};

#endif