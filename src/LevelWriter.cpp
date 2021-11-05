
#include "LevelWriter.h"
#include <string>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <math.h>

#include "FileUtils.h"
#include "CFileDefinition.h"
#include "MeshWriter.h"
#include "Collision.h"

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

        levelDef.minBoundary.x = std::min(levelDef.minBoundary.x, base.position.x);
        levelDef.minBoundary.z = std::min(levelDef.minBoundary.z, base.position.z);

        levelDef.maxBoundary.x = std::max(levelDef.maxBoundary.x, base.position.x);
        levelDef.maxBoundary.z = std::max(levelDef.maxBoundary.z, base.position.z);
    }

    if (nodeName.rfind("Geometry", 0) == 0) {
        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            levelDef.geometryMeshes.push_back(scene->mMeshes[node->mMeshes[i]]);
        }
    }

    if (nodeName.rfind("Boundary", 0) == 0) {
        if (node->mNumMeshes > 0) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];
            extractMeshBoundary(mesh, transform, levelDef.boundary);
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i) {
        populateLevelRecursive(scene, levelDef, node->mChildren[i], transform * node->mChildren[i]->mTransformation);
    }
}

void populateLevel(const aiScene* scene, class LevelDefinition& levelDef, DisplayListSettings& settings) {
    populateLevelRecursive(scene, levelDef, scene->mRootNode, aiMatrix4x4());

    for (unsigned i = 0; i < levelDef.boundary.size(); ++i) {
        aiVector3D boundaryPoint = levelDef.boundary[i];

        levelDef.minBoundary.x = std::min(levelDef.minBoundary.x, boundaryPoint.x);
        levelDef.minBoundary.z = std::min(levelDef.minBoundary.z, boundaryPoint.z);

        levelDef.maxBoundary.x = std::max(levelDef.maxBoundary.x, boundaryPoint.x);
        levelDef.maxBoundary.z = std::max(levelDef.maxBoundary.z, boundaryPoint.z);
    }
}

bool generateBoundaryEdge(const aiVector3D& from, const aiVector3D& to, std::ostream& fileContent) {
    aiVector3D at = (from + to) * 0.5f;
    aiVector3D offset = to - from;

    if (offset.SquareLength() < 0.1f) {
        return false;
    }

    at.y = 0.0f;
    offset.y = 0.0f;

    float tmp = offset.x;
    offset.x = offset.z;
    offset.z = -tmp;

    if (at * offset > 0) {
        offset = -offset;
    }

    offset.Normalize();

    fileContent << "    {{" << at.x << ", " << at.z << "}, {" << offset.x << ", " << offset.z << "}}," << std::endl;
    return true;
}

void generateLevelFromScene(const aiScene* scene, std::string headerFilename, DisplayListSettings& settings, std::ostream& headerFile, std::ostream& fileContent) {
    LevelDefinition levelDef;
    levelDef.maxPlayerCount = 0;
    populateLevel(scene, levelDef, settings);
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

    std::string geometryName = generateMesh(scene, fileDefinition, chunks, settings, fileContent);

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

    std::string boundary = fileDefinition.GetUniqueName("Boundary");
    fileContent << "struct SceneBoundary " << boundary << "[] = {" << std::endl;
    unsigned actualBoundaryCount = 0;
    for (unsigned i = 0; i < levelDef.boundary.size(); ++i) {
        if (generateBoundaryEdge(levelDef.boundary[i], levelDef.boundary[(i + 1) % levelDef.boundary.size()], fileContent)) {
            ++actualBoundaryCount;
        }
    }
    fileContent << "};" << std::endl;
    fileContent << std::endl;

    fileContent << "struct LevelDefinition " << definitionName << " = {" << std::endl;
    fileContent << "    .maxPlayerCount = " << levelDef.maxPlayerCount << "," << std::endl;
    fileContent << "    .playerStartLocations = " << startingPositions << "," << std::endl;
    fileContent << "    .baseCount = " << levelDef.bases.size() << "," << std::endl;
    fileContent << "    .bases = " << basesName << "," << std::endl;
    fileContent << "    .levelBoundaries = {{" << levelDef.minBoundary.x << ", " << levelDef.minBoundary.z << "}, {" << levelDef.maxBoundary.x << ", " << levelDef.maxBoundary.z << "}}," << std::endl;
    fileContent << "    .sceneRender = " << geometryName << "," << std::endl;
    fileContent << "    .staticScene = {" << boundary << ", " << actualBoundaryCount << "}," << std::endl;
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