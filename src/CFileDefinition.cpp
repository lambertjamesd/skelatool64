#include "CFileDefinition.h"
#include <stdio.h>

VertexBufferDefinition::VertexBufferDefinition(aiMesh* targetMesh, std::string name, VertexType vertexType):
    mTargetMesh(targetMesh),
    mName(name),
    mVertexType(vertexType) {

}

ErrorCode convertToShort(float value, short& output) {
    int result = (int)(value);

    if (result < (int)std::numeric_limits<short>::min() || result > (int)std::numeric_limits<short>::max()) {
        return ErrorCode::ModelTooLarge;
    }

    output = (short)result;

    return ErrorCode::None;
}

char convertNormalizedRange(float value) {
    int result = (int)(value * 128.0f);

    if (result < std::numeric_limits<char>::min()) {
        return std::numeric_limits<char>::min();
    } else if (result > std::numeric_limits<char>::max()) {
        return std::numeric_limits<char>::max();
    } else {
        return (char)result;
    }
}

unsigned char convertByteRange(float value) {
    int result = (int)(value * 256.0f);

    if (result < std::numeric_limits<unsigned char>::min()) {
        return std::numeric_limits<unsigned char>::min();
    } else if (result > std::numeric_limits<unsigned char>::max()) {
        return std::numeric_limits<unsigned char>::max();
    } else {
        return (unsigned char)result;
    }
}

ErrorCode VertexBufferDefinition::Generate(std::ostream& output, float scale) {
    output << "Vtx " << mName << "[] = {" << std::endl;
    
    for (unsigned int i = 0; i < mTargetMesh->mNumVertices; ++i) {
        output << "    {{{";

        aiVector3D pos = mTargetMesh->mVertices[i] * scale;

        short converted;

        ErrorCode code = convertToShort(pos.x, converted);
        if (code != ErrorCode::None) return code;
        output << converted << ", ";
        
        code = convertToShort(pos.y, converted);
        if (code != ErrorCode::None) return code;
        output << converted << ", ";

        code = convertToShort(pos.z, converted);
        if (code != ErrorCode::None) return code;
        output << converted << "}, 0, {";

        if (mTargetMesh->mTextureCoords == nullptr) {
            output << "0, 0}, {";
        } else {
            aiVector3D uv = mTargetMesh->mTextureCoords[0][i];

            code = convertToShort(uv.x, converted);
            if (code != ErrorCode::None) return code;
            output << converted << ", ";

            code = convertToShort(uv.y, converted);
            if (code != ErrorCode::None) return code;
            output << converted << "}, {";
        }

        switch (mVertexType) {
        case VertexType::PosUVNormal:
            if (mTargetMesh->HasNormals()) {
                aiVector3D normal = mTargetMesh->mNormals[i];
                output 
                    << convertNormalizedRange(normal.x) << ", " 
                    << convertNormalizedRange(normal.y) << ", " 
                    << convertNormalizedRange(normal.z) << "}, 0}}";
            } else {
                output << "0, 0, 0, 255}}}";
            }
            break;
        case VertexType::PosUVColor:
            if (mTargetMesh->mColors[0] != nullptr) {
                aiColor4D color = mTargetMesh->mColors[0][i];
                output 
                    << convertByteRange(color.r) << ", " 
                    << convertByteRange(color.g) << ", " 
                    << convertByteRange(color.b) << ", " 
                    << convertByteRange(color.a) << "}}}";
            } else {
                output << "0, 0, 0, 255}}}";
            }
            break;
        }

        output << "," << std::endl;
    }

    output << "};" << std::endl;

    return ErrorCode::None;
}

CFileDefinition::CFileDefinition(std::string prefix): 
    mPrefix(prefix),
    mNextID(1) {

}

int CFileDefinition::GetVertexBuffer(aiMesh* mesh, VertexType vertexType) {
    int result = 0;

    for (auto existing = mVertexBuffers.begin(); existing != mVertexBuffers.end(); ++existing) {
        if (existing->second.mTargetMesh == mesh && existing->second.mVertexType == vertexType) {
            return existing->first;
        }
    }

    result = GetNextID();

    std::string requestedName;

    if (mesh->mName.length) {
        requestedName = mesh->mName.C_Str();
    } else {
        requestedName = "_mesh";
    }

    switch (vertexType) {
        case VertexType::PosUVColor:
            requestedName += "_color";
            break;
        case VertexType::PosUVNormal:
            requestedName += "_normal";
            break;
    }

    mVertexBuffers.insert(std::pair<int, VertexBufferDefinition>(result, VertexBufferDefinition(
        mesh, 
        GetUniqueName(requestedName), 
        vertexType
    )));

    return result;
}

const std::string CFileDefinition::GetVertexBufferName(int vertexBufferID) {
    auto result = mVertexBuffers.find(vertexBufferID);
    
    if (result != mVertexBuffers.end()) {
        return result->second.mName;
    }

    return "";
}


ErrorCode CFileDefinition::GenerateVertexBuffers(std::ostream& output, float scale) {
    for (auto buffer = mVertexBuffers.begin(); buffer != mVertexBuffers.end(); ++buffer) {
        buffer->second.Generate(output, scale);

        output << std::endl;
        output << std::endl;
    }

    return ErrorCode::None;
}

int CFileDefinition::GetNextID() {
    return mNextID++;
}

std::string CFileDefinition::GetUniqueName(std::string requestedName) {
    std::string result = requestedName;

    int index = 1;
    
    while (mUsedNames.find(result) != mUsedNames.end()) {
        char strBuffer[8];
        snprintf(strBuffer, 8, "_%d", index);
        result = requestedName + strBuffer;
        ++index;
    }

    mUsedNames.insert(result);

    return mPrefix + result;
}