#include "./DisplayList.h"

#include "./CFileDefinition.h"

#include <sstream>

DisplayListCommand::DisplayListCommand(DisplayListCommandType type): mType(type) {}

VTXCommand::VTXCommand(
        int numVerts, 
        int indexBufferStart, 
        int vertexBufferID,
        int vertexBufferOffset
    ) :
    DisplayListCommand(DisplayListCommandType::G_VTX),
    mNumVerts(numVerts),
    mIndexBufferStart(indexBufferStart),
    mVertexBufferID(vertexBufferID),
    mVertexBufferOffset(vertexBufferOffset) {

    }

void VTXCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output) {
    output
        << "gsSPVertex(&"
        << fileDefinition.GetVertexBufferName(mVertexBufferID) 
        << "[" << mVertexBufferOffset << "], "
        << mNumVerts << ", "
        << mIndexBufferStart << ")";
}

TRI1Command::TRI1Command(int a, int b, int c) :
    DisplayListCommand(DisplayListCommandType::G_TRI1),
    mA(a),
    mB(b),
    mC(c) {

    }

void TRI1Command::GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output) {
    output << "gsSP1Triangle(" << mA << ", " << mB << ", " << mC << ", 0)";
}

TRI2Command::TRI2Command(int a0, int b0, int c0, int a1, int b1, int c1) :
    DisplayListCommand(DisplayListCommandType::G_TRI2),
    mA0(a0),
    mB0(b0),
    mC0(c0),
    mA1(a1),
    mB1(b1),
    mC1(c1) {

    }

void TRI2Command::GenerateCommand(CFileDefinition& fileDefinition, std::ostringstream& output) {
    output << "gSP2Triangles(" << mA0 << ", " << mB0 << ", " << mB0 << ", 0, " << mA1 << ", " << mB1 << ", " << mB1 << ", 0)";
}

void DisplayList::AddCommand(std::unique_ptr<DisplayListCommand> command) {
    mDisplayList.push_back(std::move(command));
}