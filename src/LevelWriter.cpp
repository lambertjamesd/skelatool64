
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
#include "ThemeWriter.h"
#include "Wireframe.h"

LevelDefinition::LevelDefinition() {
    hasStartPosition[0] = false;
    hasStartPosition[1] = false;
    hasStartPosition[2] = false;
    hasStartPosition[3] = false;
}

void populateLevelRecursive(const aiScene* scene, class LevelDefinition& levelDef, ThemeWriter* themeWriter, aiNode* node, const aiMatrix4x4& transform) {
    std::string nodeName = node->mName.C_Str();

    if (nodeName.rfind("Base", 0) == 0) {
        BaseDefinition base;
        base.team = atoi(nodeName.c_str() + 5);

        aiQuaternion rotation;
        transform.DecomposeNoScaling(rotation, base.position);

        levelDef.bases.push_back(base);

        if (base.team >= 0 && base.team < MAX_PLAYERS) {
            if (!levelDef.hasStartPosition[base.team]) {
                levelDef.startPosition[base.team] = base.position;
            }
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

    if (nodeName.rfind("Pathfinding", 0) == 0) {
        if (node->mNumMeshes > 0) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];

            std::vector<aiVector3D> basePositions;
            for(unsigned i = 0; i < levelDef.bases.size(); ++i) basePositions.push_back(levelDef.bases[i].position);

            class Pathfinding pathfinding;
            buildPathingFromMesh(mesh, pathfinding, transform);
            buildPathfindingDefinition(pathfinding, levelDef.pathfinding, basePositions);
        }
    }

    if (nodeName.rfind("PlayerStart ", 0) == 0) {
        int playerIndex = atoi(nodeName.c_str() + strlen("PlayerStart "));
        if (playerIndex >= 0 && playerIndex < MAX_PLAYERS) {
            aiQuaternion rotation;
            transform.DecomposeNoScaling(rotation, levelDef.startPosition[playerIndex]);
            levelDef.hasStartPosition[playerIndex] = true;
        }
    }

    std::string decorName;
    if (themeWriter && themeWriter->GetDecorName(nodeName, decorName)) {
        DecorDefinition decorDef;
        aiVector3D scaling;
        transform.Decompose(scaling, decorDef.rotation, decorDef.position);
        decorDef.decorID = decorName;
        levelDef.decor.push_back(decorDef);
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i) {
        populateLevelRecursive(scene, levelDef, themeWriter, node->mChildren[i], transform * node->mChildren[i]->mTransformation);
    }
}

void populateLevel(const aiScene* scene, class LevelDefinition& levelDef, ThemeWriter* themeWriter, DisplayListSettings& settings) {
    populateLevelRecursive(scene, levelDef, themeWriter, scene->mRootNode, aiMatrix4x4());

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

void generateDecorDL(LevelDefinition& levelDef, ThemeWriter* theme, DisplayList& dl) {
    std::vector<std::pair<unsigned, DecorDefinition>> decorCopy;

    for (unsigned i = 0; i < levelDef.decor.size(); ++i) {
        decorCopy.push_back(std::make_pair(i, levelDef.decor[i]));
    }

    std::sort(decorCopy.begin(), decorCopy.end(), [=](const std::pair<unsigned, DecorDefinition>& a, const std::pair<unsigned, DecorDefinition>& b) -> bool {
        std::string aMaterial = theme->GetDecorMaterial(a.second.decorID);
        std::string bMaterial = theme->GetDecorMaterial(b.second.decorID);

        if (aMaterial == bMaterial) {
            return a.second.decorID < b.second.decorID;
        }

        return aMaterial < bMaterial;
    });

    std::string currentMaterial = "";

    for (auto it = decorCopy.begin(); it != decorCopy.end(); ++it) {
        std::string material = theme->GetDecorMaterial(it->second.decorID);
        if (material != currentMaterial && material != "0") {
            dl.AddCommand(std::unique_ptr<DisplayListCommand>(new CallDisplayListByNameCommand(material)));
            currentMaterial = material;
        }

        dl.AddCommand(std::unique_ptr<DisplayListCommand>(new PushMatrixCommand(it->first, false)));
        dl.AddCommand(std::unique_ptr<DisplayListCommand>(new CallDisplayListByNameCommand(theme->GetDecorGeo(it->second.decorID))));
        dl.AddCommand(std::unique_ptr<DisplayListCommand>(new PopMatrixCommand(1)));
    }
}

void generateLevelFromScene(LevelDefinition& levelDef, const aiScene* scene, std::string headerFilename, ThemeWriter* theme, DisplayListSettings& settings, std::ostream& headerFile, std::ostream& fileContent) {
    CFileDefinition fileDefinition(settings.mPrefix);
    BoneHierarchy blankBones;

    headerFile << "#ifndef _" << settings.mPrefix << "_H" << std::endl;
    headerFile << "#define _" << settings.mPrefix << "_H" << std::endl;
    headerFile << std::endl;
    headerFile << "#include \"scene/leveldefinition.h\"" << std::endl;
    headerFile << std::endl;

    std::string definitionName = fileDefinition.GetUniqueName("Definition");

    std::string wireframeName = fileDefinition.GetUniqueName("wireframe_model_gfx");

    headerFile << "extern struct LevelDefinition " << definitionName << ";" << std::endl;
    headerFile << "extern Gfx " << wireframeName << "[];" << std::endl;

    headerFile << std::endl;
    headerFile << "#endif";

    fileContent << "#include \"" << getBaseName(headerFilename) << "\"" << std::endl;
    
    if (theme) {
        fileContent << "#include \"" << Relative(headerFilename, theme->GetThemeHeader()) << "\"" << std::endl;
        fileContent << "#include \"sk64/skelatool_defs.h\"" << std::endl;
    }

    fileContent << "#include <ultra64.h>" << std::endl;
    fileContent << std::endl;

    std::vector<ExtendedMesh> meshes;

    for (auto it = levelDef.geometryMeshes.begin(); it != levelDef.geometryMeshes.end(); ++it) {
        meshes.push_back(ExtendedMesh(*it, blankBones));
    }

    std::vector<RenderChunk> chunks;
    for (auto it = meshes.begin(); it != meshes.end(); ++it) {
        chunks.push_back(RenderChunk(std::pair<Bone*, Bone*>(nullptr, nullptr), &*it, VertexType::PosUVColor));
    }

    DisplayList sceneDisplayList(fileDefinition.GetUniqueName("model_gfx"));

    if (theme) {
        generateMeshIntoDLWithMaterials(scene, fileDefinition, &theme->mMaterialCollector, chunks, settings, sceneDisplayList);
        sceneDisplayList.AddCommand(std::unique_ptr<DisplayListCommand>(new CommentCommand("Begin decor")));
        generateDecorDL(levelDef, theme, sceneDisplayList);
        fileDefinition.GenerateVertexBuffers(fileContent, settings.mScale, settings.mRotateModel);
    } else {
        generateMeshIntoDL(scene, fileDefinition, chunks, settings, sceneDisplayList, fileContent);
    }

    sceneDisplayList.Generate(fileDefinition, fileContent);

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

    std::string decorList = "0";
    aiQuaternion inverseRotation = settings.mRotateModel;
    inverseRotation.Conjugate();

    if (theme) {
        decorList = fileDefinition.GetUniqueName("Decor");

        fileContent << "struct DecorDefinition " << decorList << "[] = {" << std::endl;
        for (auto it = levelDef.decor.begin(); it != levelDef.decor.end(); ++it) {
            aiQuaternion finalRotation = it->rotation * inverseRotation;

            fileContent << "    {";
            fileContent << "{" << it->position.x << ", " << it->position.y << ", " << it->position.z << "}, ";
            fileContent << "{" << finalRotation.x << ", " << finalRotation.y << ", " << finalRotation.z << ", " << finalRotation.w << "}, ";
            fileContent << theme->GetDecorID(it->decorID);
            fileContent << "}," << std::endl;
        }
        fileContent << "};" << std::endl;
    }

    std::string pathingNodePositions = fileDefinition.GetUniqueName("PathingNodes");

    fileContent << "struct Vector3 " << pathingNodePositions << "[] = {" << std::endl;
    for (auto it = levelDef.pathfinding.mNodePositions.begin(); it != levelDef.pathfinding.mNodePositions.end(); ++it) {
        fileContent << "    {" << it->x << ", 0.0f, " << it->z << "}," << std::endl;
    }
    fileContent << "};" << std::endl;

    std::string basePathNodePositions = fileDefinition.GetUniqueName("BasesPathNodes");
    fileContent << "char " << basePathNodePositions << "[] = {" << std::endl;
    for(unsigned i = 0; i < levelDef.pathfinding.baseNodes.size(); ++i){
        fileContent << "    " << levelDef.pathfinding.baseNodes[i] << "," << std::endl;
    }
    fileContent << "};" << std::endl;

    std::string baseDist = fileDefinition.GetUniqueName("basePathNodeDistnaces");
    fileContent << "struct basesDistance " << baseDist << "[] = {" << std::endl;
    for(auto it = levelDef.pathfinding.baseDistances.begin(); it != levelDef.pathfinding.baseDistances.end(); ++it){
        fileContent << "    {" << it->fromBase << ", " << it->toBase << ", " << it->distance << "}, " << std::endl;
    }
    fileContent << "};" << std::endl;

    std::string nextNode = fileDefinition.GetUniqueName("NextNode");

    fileContent << "char " << nextNode << "[] = {" << std::endl;
    unsigned currIndex = 0;
    for (unsigned y = 0; y < levelDef.pathfinding.mNodePositions.size(); ++y) {
        fileContent << "    ";
        for (unsigned x = 0; x < levelDef.pathfinding.mNodePositions.size(); ++x) {
            fileContent << levelDef.pathfinding.mNextNode[currIndex] << ", ";
            ++currIndex;
        }
        fileContent << std::endl;
    }
    fileContent << "};" << std::endl;

    fileContent << "struct LevelDefinition " << definitionName << " = {" << std::endl;
    fileContent << "    .maxPlayerCount = " << levelDef.maxPlayerCount << "," << std::endl;
    fileContent << "    .playerStartLocations = " << startingPositions << "," << std::endl;
    fileContent << "    .baseCount = " << levelDef.bases.size() << "," << std::endl;
    fileContent << "    .decorCount = " << levelDef.decor.size() << "," << std::endl;
    fileContent << "    .bases = " << basesName << "," << std::endl;
    fileContent << "    .decor = " << decorList << "," << std::endl;
    fileContent << "    .levelBoundaries = {{" << levelDef.minBoundary.x << ", " << levelDef.minBoundary.z << "}, {" << levelDef.maxBoundary.x << ", " << levelDef.maxBoundary.z << "}}," << std::endl;
    fileContent << "    .sceneRender = " << sceneDisplayList.GetName() << "," << std::endl;
    fileContent << "    .theme = ";
    if (theme) {
        fileContent << "&" << theme->GetThemeName() << "Theme";
    } else {
        fileContent << "0";
    }
    fileContent << "," << std::endl;
    fileContent << "    .staticScene = {" << boundary << ", " << actualBoundaryCount << "}," << std::endl;
    fileContent << "    .pathfinding = {.nodeCount = " << levelDef.pathfinding.mNodePositions.size() << ", .baseNodes = " << basePathNodePositions <<
        ", .baseDistances = " << baseDist << ", .nodePositions = " << pathingNodePositions << ", .nextNode = " << nextNode << "}," << std::endl;
    fileContent << "};" << std::endl;
    fileContent << std::endl;
}

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, ThemeWriter* theme, DisplayListSettings& settings) {
    LevelDefinition levelDef;
    levelDef.maxPlayerCount = 0;
    populateLevel(scene, levelDef, theme, settings);
    
    std::ostringstream headerContent;
    std::ostringstream fileContent;
    std::ostringstream wireframe;

    std::string headerFilename = replaceExtension(filename, ".h");

    generateLevelFromScene(levelDef, scene, headerFilename, theme, settings, headerContent, fileContent);
    generateLevelWireframe(levelDef, scene, theme, settings, wireframe);

    std::ofstream outputHeader;
    outputHeader.open(headerFilename, std::ios_base::out | std::ios_base::trunc);
    outputHeader << headerContent.str();
    outputHeader.close();

    std::ofstream outputContent;
    outputContent.open(replaceExtension(filename, ".c"), std::ios_base::out | std::ios_base::trunc);
    outputContent << fileContent.str();
    outputContent.close();
    
    std::ofstream outputWireframe;
    outputWireframe.open(replaceExtension(filename, ".wire.c"), std::ios_base::out | std::ios_base::trunc);
    outputWireframe << wireframe.str();
    outputWireframe.close();
}