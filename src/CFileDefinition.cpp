#include "CFileDefinition.h"
#include <stdio.h>
#include "StringUtils.h"

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

 ErrorCode VertexBufferDefinition::Generate(float scale, aiQuaternion rotate, std::unique_ptr<FileDefinition>& output, const std::string& fileSuffix) {
    std::unique_ptr<StructureDataChunk> dataChunk(new StructureDataChunk());
    
    for (unsigned int i = 0; i < mTargetMesh->mMesh->mNumVertices; ++i) {
        std::unique_ptr<StructureDataChunk> vertex(new StructureDataChunk());

        aiVector3D pos = mTargetMesh->mMesh->mVertices[i];

        if (mTargetMesh->mPointInverseTransform[i]) {
            pos = (*mTargetMesh->mPointInverseTransform[i]) * pos;
        } else {
            pos = rotate.Rotate(pos);
        }

        pos = pos * scale;

        short converted;

        std::unique_ptr<StructureDataChunk> posVertex(new StructureDataChunk());

        ErrorCode code = convertToShort(pos.x, converted);
        if (code != ErrorCode::None) return code;
        posVertex->AddPrimitive(converted);
        
        code = convertToShort(pos.y, converted);
        if (code != ErrorCode::None) return code;
        posVertex->AddPrimitive(converted);

        code = convertToShort(pos.z, converted);
        if (code != ErrorCode::None) return code;
        posVertex->AddPrimitive(converted);

        vertex->Add(std::move(posVertex));
        vertex->AddPrimitive(0);


        std::unique_ptr<StructureDataChunk> texCoords(new StructureDataChunk());

        if (mTargetMesh->mMesh->mTextureCoords[0] == nullptr) {
            texCoords->AddPrimitive(0);
            texCoords->AddPrimitive(0);
        } else {
            aiVector3D uv = mTargetMesh->mMesh->mTextureCoords[0][i];

            code = convertToShort(uv.x * (1 << 11), converted);
            if (code != ErrorCode::None) return code;
            texCoords->AddPrimitive(converted);

            code = convertToShort((1.0f - uv.y) * (1 << 11), converted);
            if (code != ErrorCode::None) return code;
            texCoords->AddPrimitive(converted);
        }

        vertex->Add(std::move(texCoords));

        std::unique_ptr<StructureDataChunk> vertexNormal(new StructureDataChunk());

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

                vertexNormal->AddPrimitive(convertNormalizedRange(normal.x));
                vertexNormal->AddPrimitive(convertNormalizedRange(normal.y));
                vertexNormal->AddPrimitive(convertNormalizedRange(normal.z));
                vertexNormal->AddPrimitive(255);
            } else {
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(255);
            }
            break;
        case VertexType::PosUVColor:
            if (mTargetMesh->mMesh->mColors[0] != nullptr) {
                aiColor4D color = mTargetMesh->mMesh->mColors[0][i];
                vertexNormal->AddPrimitive(color.r);
                vertexNormal->AddPrimitive(color.g);
                vertexNormal->AddPrimitive(color.b);
                vertexNormal->AddPrimitive(color.a);
            } else {
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(0);
                vertexNormal->AddPrimitive(255);
            }
            break;
        }

        vertex->Add(std::move(vertexNormal));

        dataChunk->Add(std::move(vertex));
    }

    output = std::unique_ptr<FileDefinition>(new DataFileDefinition("Vtx", mName, true, fileSuffix, std::move(dataChunk)));

    return ErrorCode::None;
}

CFileDefinition::CFileDefinition(std::string prefix, float modelScale, aiQuaternion modelRotate, const std::string& modelFileSuffix): 
    mPrefix(prefix),
    mModelScale(modelScale),
    mModelRotate(modelRotate),
    mModelFileSuffix(modelFileSuffix) {

}

void CFileDefinition::AddDefinition(std::unique_ptr<FileDefinition> definition) {
    mDefinitions.push_back(std::move(definition));
}

void CFileDefinition::AddMacro(const std::string& name, const std::string& value) {
    mMacros.push_back(name + " " + value);
}

std::string CFileDefinition::GetVertexBuffer(ExtendedMesh* mesh, VertexType vertexType) {
    for (auto existing = mVertexBuffers.begin(); existing != mVertexBuffers.end(); ++existing) {
        if (existing->second.mTargetMesh == mesh && existing->second.mVertexType == vertexType) {
            return existing->first;
        }
    }

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



    std::string name = GetUniqueName(requestedName);


    mVertexBuffers.insert(std::pair<std::string, VertexBufferDefinition>(name, VertexBufferDefinition(
        mesh, 
        name, 
        vertexType
    )));

    std::unique_ptr<FileDefinition> vtxDef;
    // TODO scale/rotate
    mVertexBuffers.find(name)->second.Generate(mModelScale, mModelRotate, vtxDef, mModelFileSuffix);
    AddDefinition(std::move(vtxDef));

    return name;
}


	// {{{-226, 0, -226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{-226, 0, 226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{-226, 40, 226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{-226, 40, -226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{226, 0, -226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{226, 0, 226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{226, 40, 226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},
	// {{{226, 40, -226},0, {-16, -16},{0x0, 0x0, 0x0, 0x0}}},

std::string CFileDefinition::GetCullingBuffer(const std::string& name, const aiVector3D& min, const aiVector3D& max) {
    aiMesh* mesh = new aiMesh();

    mesh->mName = name;
    mesh->mNumVertices = 8;
    mesh->mVertices = new aiVector3D[8];
    mesh->mVertices[0] = aiVector3D(min.x, min.y, min.z);
    mesh->mVertices[1] = aiVector3D(min.x, min.y, max.z);
    mesh->mVertices[2] = aiVector3D(min.x, max.y, min.z);
    mesh->mVertices[3] = aiVector3D(min.x, max.y, max.z);
    mesh->mVertices[4] = aiVector3D(max.x, min.y, min.z);
    mesh->mVertices[5] = aiVector3D(max.x, min.y, max.z);
    mesh->mVertices[6] = aiVector3D(max.x, max.y, min.z);
    mesh->mVertices[7] = aiVector3D(max.x, max.y, max.z);

    BoneHierarchy boneHierarchy;
    return GetVertexBuffer(new ExtendedMesh(mesh, boneHierarchy), VertexType::PosUVColor);
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

void CFileDefinition::Generate(std::ostream& output, const std::string& location, const std::string& headerFileName) {
    output << "#include \"" << headerFileName << "\"" << std::endl;

    for (auto it = mDefinitions.begin(); it != mDefinitions.end(); ++it) {

        if ((*it)->GetLocation() == location) {
            (*it)->Generate(output);

            output << ";\n\n";
        }
    }
}

void CFileDefinition::GenerateHeader(std::ostream& output, const std::string& headerFileName) {
    std::string infdef = std::string("__") + headerFileName + "_H__";

    makeCCompatible(infdef);
    std::transform(infdef.begin(), infdef.end(), infdef.begin(), ::toupper);

    output << "#ifndef " << infdef << std::endl;
    output << "#define " << infdef << std::endl;
    output << std::endl;

    std::set<std::string> includes;

    for (auto it = mDefinitions.begin(); it != mDefinitions.end(); ++it) {
        auto headers = (*it)->GetTypeHeaders();

        for (auto header : headers) {
            includes.insert(header);
        }
    }

    std::vector<std::string> includesSorted(includes.size());
    std::copy(includes.begin(), includes.end(), includesSorted.begin());

    // std::sort(includes.begin(), includes.end());

    for (auto include : includesSorted) {
        output << "#include " << include << std::endl;
    }

    output << std::endl;

    if (mMacros.size()) {
        for (auto macro : mMacros) {
            output << "#define " << macro << std::endl;
        }
        output << std::endl;
    }

    for (auto it = mDefinitions.begin(); it != mDefinitions.end(); ++it) {
            (*it)->GenerateDeclaration(output);
            output << ";\n";
    };

    output << std::endl;
    output << "#endif" << std::endl;
}