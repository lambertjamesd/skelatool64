
#include "LevelWriter.h"
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>

#include "FileUtils.h"
#include "CFileDefinition.h"
#include "MeshWriter.h"

void populateLevelRecursive(const aiScene* scene, class LevelDefinition& levelDef, aiNode* node, const aiMatrix4x4& transform) {
    std::string nodeName = node->mName.C_Str();

    if (nodeName.rfind("Base", 0) == 0) {
        BaseDefinition base;
        base.team = atoi(nodeName.c_str() + 5);

        aiQuaternion rotation;
        transform.DecomposeNoScaling(rotation, base.position);

        levelDef.bases.push_back(base);

        if (base.team >= 0 && base.team < MAX_PLAYERS) {
            levelDef.startPosition[base.team] = base.position;
            levelDef.maxPlayerCount = std::max(levelDef.maxPlayerCount, base.team + 1);
        }

    }

    if (nodeName.rfind("Geometry", 0) == 0) {
        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            levelDef.geometryMeshes.push_back(scene->mMeshes[node->mMeshes[i]]);
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i) {
        populateLevelRecursive(scene, levelDef, node->mChildren[i], transform * node->mChildren[i]->mTransformation);
    }
}

void populateLevel(const aiScene* scene, class LevelDefinition& levelDef) {
    populateLevelRecursive(scene, levelDef, scene->mRootNode, aiMatrix4x4());
}

void generateLevelFromScene(const aiScene* scene, std::string headerFilename, DisplayListSettings& settings, std::ostream& headerFile, std::ostream& fileContent) {
    LevelDefinition levelDef;
    levelDef.maxPlayerCount = 0;
    populateLevel(scene, levelDef);
    CFileDefinition fileDefinition(settings.mPrefix);
    BoneHierarchy blankBones;

    headerFile << "#ifndef _" << settings.mPrefix << "_H" << std::endl;
    headerFile << "#define _" << settings.mPrefix << "_H" << std::endl;
    headerFile << std::endl;
    headerFile << "#include \"scene/leveldefinition.h\"" << std::endl;
    headerFile << std::endl;

    std::string definitionName = fileDefinition.GetUniqueName("Definition");

    headerFile << "extern struct LevelDefinition " << definitionName << ";" << std::endl;

    headerFile << std::endl;
    headerFile << "#endif";

    fileContent << "#include \"" << getBaseName(headerFilename) << "\"" << std::endl;
    fileContent << "#include <ultra64.h>" << std::endl;
    fileContent << std::endl;

    std::vector<RenderChunk> chunks;
    std::vector<ExtendedMesh> meshes;

    for (auto it = levelDef.geometryMeshes.begin(); it != levelDef.geometryMeshes.end(); ++it) {
        meshes.push_back(ExtendedMesh(*it, blankBones));
        chunks.push_back(RenderChunk(std::pair<Bone*, Bone*>(nullptr, nullptr), &*meshes.rbegin(), VertexType::PosUVColor));
    }

    std::string geometryName = generateMesh(fileDefinition, chunks, settings, fileContent);

    std::string basesName = fileDefinition.GetUniqueName("Bases");

    fileContent << "struct BaseDefinition " << basesName << "[] = {" << std::endl;
    for (auto it = levelDef.bases.begin(); it != levelDef.bases.end(); ++it) {
        fileContent << "    {.position = {" << it->position.x << ", " << it->position.z << "}, .startingTeam = ";

        if (it->team >=0 && it->team < MAX_PLAYERS) {
            fileContent << "TEAM(" << it->team << ")}," << std::endl;
        } else {
            fileContent << "TEAM_NONE}," << std::endl;
        }
    }
    fileContent << "};" << std::endl;
    fileContent << std::endl;

    std::string startingPositions = fileDefinition.GetUniqueName("StartingPositions");
    fileContent << "struct Vector2 " << startingPositions << "[] = {" << std::endl;
    for (unsigned i = 0; i < levelDef.maxPlayerCount; ++i) {
        fileContent << "    {" << levelDef.startPosition[i].x << ", " << levelDef.startPosition[i].z << "}," << std::endl;
    }
    fileContent << "};" << std::endl;
    fileContent << std::endl;

    fileContent << "struct LevelDefinition " << definitionName << " = {" << std::endl;
    fileContent << "    .maxPlayerCount = " << levelDef.maxPlayerCount << "," << std::endl;
    fileContent << "    .playerStartLocations = " << startingPositions << "," << std::endl;
    fileContent << "    .baseCount = " << levelDef.bases.size() << "," << std::endl;
    fileContent << "    .bases = " << basesName << "," << std::endl;
    fileContent << "    .sceneRender = " << geometryName << "," << std::endl;
    fileContent << "};" << std::endl;
    fileContent << std::endl;
}

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings) {
    std::ostringstream headerContent;
    std::ostringstream fileContent;

    std::string headerFilename = replaceExtension(filename, ".h");

    generateLevelFromScene(scene, headerFilename, settings, headerContent, fileContent);

    std::ofstream outputHeader;
    outputHeader.open(headerFilename, std::ios_base::out | std::ios_base::trunc);
    outputHeader << headerContent.str();
    outputHeader.close();

    std::ofstream outputContent;
    outputContent.open(replaceExtension(filename, ".c"), std::ios_base::out | std::ios_base::trunc);
    outputContent << fileContent.str();
    outputContent.close();

}