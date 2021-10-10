#ifndef _DISPLAY_LIST_H
#define _DISPLAY_LIST_H

#include <memory>
#include <vector>
#include <string>
#include <iostream>

enum class DisplayListCommandType {
    COMMENT,
    G_VTX,
    G_TRI1,
    G_TRI2,
    G_MTX,
    G_POPMTX,
};

class DisplayList;
class CFileDefinition;

struct DisplayListCommand {
    DisplayListCommand(DisplayListCommandType type);
    virtual ~DisplayListCommand();

    DisplayListCommandType mType;

    virtual bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) = 0;
};


struct CommentCommand : DisplayListCommand {
    CommentCommand(std::string comment);

    std::string mComment;

    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);
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

    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);
};

struct TRI1Command : DisplayListCommand {
    TRI1Command(int a, int b, int c);

    int mA;
    int mB;
    int mC;

    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);
};

struct TRI2Command : DisplayListCommand {
    TRI2Command(int a0, int b0, int c0, int a1, int b1, int c1);

    int mA0;
    int mB0;
    int mC0;

    int mA1;
    int mB1;
    int mC1;

    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);
};

struct CallDisplayListCommand {
    CallDisplayListCommand(int targetDL, int offset);

    int mTargetDL;
    int mOffset;

    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);
};

struct PushMatrixCommand : DisplayListCommand {
    PushMatrixCommand(unsigned int matrixOffset, bool replace);
    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);

    unsigned int mMatrixOffset;
    bool mReplace;
};

struct PopMatrixCommand : DisplayListCommand {
    PopMatrixCommand(unsigned int popCount);
    bool GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output);

    unsigned int mPopCount;
};

class DisplayList {
public:
    DisplayList(std::string name);
    void AddCommand(std::unique_ptr<DisplayListCommand> command);

    const std::string& GetName();
    void Generate(CFileDefinition& fileDefinition, std::ostream& output);
private:
    std::string mName;
    std::vector<std::unique_ptr<DisplayListCommand>> mDisplayList;
};

#endif