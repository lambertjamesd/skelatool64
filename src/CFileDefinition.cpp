#include "CFileDefinition.h"
#include <stdio.h>

VertexBufferDefinition::VertexBufferDefinition(ExtendedMesh* targetMesh, std::string name, VertexType vertexType):
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

int convertNormalizedRange(float value) {
    int result = (int)(value * 128.0f);

    if (result < std::numeric_limits<char>::min()) {
        return std::numeric_limits<char>::min();
    } else if (result > std::numeric_limits<char>::max()) {
        return std::numeric_limits<char>::max();
    } else {
        return (char)result;
    }
}

unsigned convertByteRange(float value) {
    int result = (int)(value * 256.0f);

    if (result < std::numeric_limits<unsigned char>::min()) {
        return std::numeric_limits<unsigned char>::min();
    } else if (result > std::numeric_limits<unsigned char>::max()) {
        return std::numeric_limits<unsigned char>::max();
    } else {
        return (unsigned char)result;
    }
}

ErrorCode VertexBufferDefinition::Generate(std::ostream& output, float scale, aiQuaternion rotate) {
    output << "Vtx " << mName << "[] = {" << std::endl;
    
    for (unsigned int i = 0; i < mTargetMesh->mMesh->mNumVertices; ++i) {
        output << "    {{{";

        aiVector3D pos = mTargetMesh->mMesh->mVertices[i];

        if (mTargetMesh->mPointInverseTransform[i]) {
            pos = (*mTargetMesh->mPointInverseTransform[i]) * pos;
        } else {
            pos = rotate.Rotate(pos);
        }

        pos = pos * scale;

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

        if (mTargetMesh->mMesh->mTextureCoords == nullptr) {
            output << "0, 0}, {";
        } else {
            aiVector3D uv = mTargetMesh->mMesh->mTextureCoords[0][i];

            code = convertToShort(uv.x * (1 << 10), converted);
            if (code != ErrorCode::None) return code;
            output << converted << ", ";

            code = convertToShort((1.0f - uv.y) * (1 << 10), converted);
            if (code != ErrorCode::None) return code;
            output << converted << "}, {";
        }

        switch (mVertexType) {
        case VertexType::PosUVNormal:
            if (mTargetMesh->mMesh->HasNormals()) {
                aiVector3D normal = mTargetMesh->mMesh->mNormals[i];

                if (mTargetMesh->mPointInverseTransform[i]) {
                    normal = (*mTargetMesh->mNormalInverseTransform[i]) * normal;
                    normal.Normalize();
                } else {
                    normal = rotate.Rotate(normal);
                }
                
                output 
                    << convertNormalizedRange(normal.x) << ", " 
                    << convertNormalizedRange(normal.y) << ", " 
                    << convertNormalizedRange(normal.z) << ", 255}}}";
            } else {
                output << "0, 0, 0, 255}}}";
            }
            break;
        case VertexType::PosUVColor:
            if (mTargetMesh->mMesh->mColors[0] != nullptr) {
                aiColor4D color = mTargetMesh->mMesh->mColors[0][i];
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

int CFileDefinition::GetVertexBuffer(ExtendedMesh* mesh, VertexType vertexType) {
    int result = 0;

    for (auto existing = mVertexBuffers.begin(); existing != mVertexBuffers.end(); ++existing) {
        if (existing->second.mTargetMesh == mesh && existing->second.mVertexType == vertexType) {
            return existing->first;
        }
    }

    result = GetNextID();

    std::string requestedName;

    if (mesh->mMesh->mName.length) {
        requestedName = mesh->mMesh->mName.C_Str();
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


ErrorCode CFileDefinition::GenerateVertexBuffers(std::ostream& output, float scale, aiQuaternion rotate) {
    for (auto buffer = mVertexBuffers.begin(); buffer != mVertexBuffers.end(); ++buffer) {
        buffer->second.Generate(output, scale, rotate);

        output << std::endl;
        output << std::endl;
    }

    return ErrorCode::None;
}

int CFileDefinition::GetNextID() {
    return mNextID++;
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

std::string CFileDefinition::GetUniqueName(std::string requestedName) {
    std::string result = mPrefix + "_" + requestedName;
    makeCCompatible(result);

    int index = 1;
    
    while (mUsedNames.find(result) != mUsedNames.end()) {
        char strBuffer[8];
        snprintf(strBuffer, 8, "_%d", index);
        result = mPrefix + "_" + requestedName + strBuffer;
        makeCCompatible(result);
        ++index;
    }

    mUsedNames.insert(result);

    return result;
}