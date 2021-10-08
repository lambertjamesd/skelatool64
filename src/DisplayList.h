#ifndef _DISPLAY_LIST_H
#define _DISPLAY_LIST_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>

enum class DisplayListCommandType {
    G_VTX,
    G_TRI1,
    G_TRI2,
};

class DisplayList;
class CFileDefinition;

struct DisplayListCommand {
    DisplayListCommand(DisplayListCommandType type);

    DisplayListCommandType mType;

    virtual void GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output) = 0;
};

struct VTXCommand : DisplayListCommand {
    VTXCommand(
        int numVerts, 
        int indexBufferStart, 
        int vertexBufferID,
        int vertexBufferOffset
    );

    int mNumVerts;
    int mIndexBufferStart;
    int mVertexBufferID;
    int mVertexBufferOffset;

    void GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output);
};

struct TRI1Command : DisplayListCommand {
    TRI1Command(int a, int b, int c);

    int mA;
    int mB;
    int mC;

    void GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output);
};

struct TRI2Command : DisplayListCommand {
    TRI2Command(int a0, int b0, int c0, int a1, int b1, int c1);

    int mA0;
    int mB0;
    int mC0;

    int mA1;
    int mB1;
    int mC1;

    void GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output);
};

struct CallDisplayListCommand {
    CallDisplayListCommand(int targetDL, int offset);

    int mTargetDL;
    int mOffset;

    std::string GenerateCommand(CFileDefinition& fileDefinition);
};

class DisplayList {
public:
    void AddCommand(std::unique_ptr<DisplayListCommand> command);
private:
    std::string mName;
    std::vector<std::unique_ptr<DisplayListCommand>> mDisplayList;
};

#endif