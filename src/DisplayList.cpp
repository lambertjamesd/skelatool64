#include "./DisplayList.h"

#include "./CFileDefinition.h"

#include <sstream>

DisplayListCommand::DisplayListCommand(DisplayListCommandType type): mType(type) {}

DisplayListCommand::~DisplayListCommand() {}

CommentCommand::CommentCommand(std::string comment):
    DisplayListCommand(DisplayListCommandType::COMMENT),
    mComment(comment) {

}

bool CommentCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "// " << mComment;
    return false;
}

RawContentCommand::RawContentCommand(std::string content):
    DisplayListCommand(DisplayListCommandType::RAW),
    mContent(content) {

}

bool RawContentCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << mContent;
    return false;
}

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

bool VTXCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output
        << "gsSPVertex(&"
        << fileDefinition.GetVertexBufferName(mVertexBufferID) 
        << "[" << mVertexBufferOffset << "], "
        << mNumVerts << ", "
        << mIndexBufferStart << ")";
    return true;
}

TRI1Command::TRI1Command(int a, int b, int c) :
    DisplayListCommand(DisplayListCommandType::G_TRI1),
    mA(a),
    mB(b),
    mC(c) {

    }

bool TRI1Command::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "gsSP1Triangle(" << mA << ", " << mB << ", " << mC << ", 0)";
    return true;
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

bool TRI2Command::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "gsSP2Triangles(" << mA0 << ", " << mB0 << ", " << mC0 << ", 0, " << mA1 << ", " << mB1 << ", " << mC1 << ", 0)";
    return true;
}

CallDisplayListByNameCommand::CallDisplayListByNameCommand(const std::string& dlName): 
    DisplayListCommand(DisplayListCommandType::G_DL),
    mDLName(dlName) {

}

bool CallDisplayListByNameCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "gsSPDisplayList(" << mDLName << ")";
    return true;
}

PushMatrixCommand::PushMatrixCommand(unsigned int matrixOffset, bool replace): 
    DisplayListCommand(DisplayListCommandType::G_MTX),
    mMatrixOffset(matrixOffset),
    mReplace(replace) {

}

bool PushMatrixCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "gsSPMatrix((Mtx*)MATRIX_TRANSFORM_SEGMENT_ADDRESS + " << mMatrixOffset << ", ";
    
    output << "G_MTX_MODELVIEW | G_MTX_MUL | ";
    
    if (mReplace) {
        output << "G_MTX_NOPUSH";
    } else {
        output << "G_MTX_PUSH";
    }

    output << ")";
    return true;
}

PopMatrixCommand::PopMatrixCommand(unsigned int popCount): 
    DisplayListCommand(DisplayListCommandType::G_POPMTX),
    mPopCount(popCount) {
        
}

bool PopMatrixCommand::GenerateCommand(CFileDefinition& fileDefinition, std::ostream& output) {
    if (mPopCount == 0) {
        return false;
    }

    if (mPopCount > 1) {
        output << "gsSPPopMatrixN(G_MTX_MODELVIEW, " << mPopCount << ")";
    } else {
        output << "gsSPPopMatrix(G_MTX_MODELVIEW)";
    }
    return true;
}

DisplayList::DisplayList(std::string name):
    mName(name) {
    
}

void DisplayList::AddCommand(std::unique_ptr<DisplayListCommand> command) {
    mDisplayList.push_back(std::move(command));
}

const std::string& DisplayList::GetName() {
    return mName;
}

void DisplayList::Generate(CFileDefinition& fileDefinition, std::ostream& output) {
    output << "Gfx " << mName << "[] = {" << std::endl;

    for (auto command = mDisplayList.begin(); command != mDisplayList.end(); ++command) {
        output << "    ";
        if ((*command)->GenerateCommand(fileDefinition, output)) {
            output << ",";
        }
        output << std::endl;
    }

    output << "    gsSPEndDisplayList()," << std::endl;
    output << "};" << std::endl;
}