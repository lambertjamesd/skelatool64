
#include <assimp/mesh.h>

#include <iostream>
#include <fstream>

#include "src/SceneWriter.h"
#include "src/CommandLineParser.h"
#include "src/MaterialParser.h"
#include "src/SceneLoader.h"

#include "src/definition_generator/MeshDefinitionGenerator.h"
#include "src/definition_generator/CollisionGenerator.h"

bool parseMaterials(const std::string& filename, DisplayListSettings& output) {
    std::fstream file(filename, std::ios::in);

    struct ParseResult parseResult;
    parseMaterialFile(file, parseResult);
    output.mMaterials.insert(parseResult.mMaterialFile.mMaterials.begin(), parseResult.mMaterialFile.mMaterials.end());

    for (auto err : parseResult.mErrors) {
        std::cerr << err.mMessage << std::endl;
    }

    return parseResult.mErrors.size() == 0;
}

bool getVectorByName(const aiScene* scene, const std::string name, aiVector3D& result) {
    int axis;
    if (!scene->mMetaData->Get(name, axis)) {
        return false;
    }

    switch (axis) {
        case 0: result = aiVector3D(1.0f, 0.0f, 0.0f); break;
        case 1: result = aiVector3D(0.0f, 1.0f, 0.0f); break;
        case 2: result = aiVector3D(0.0f, 0.0f, 1.0f); break;
        default: return false;
    }

    int upSign;
    if (scene->mMetaData->Get(name + "Sign", upSign)) {
        if (upSign < 0) {
            result = -result;
        }
    }

    return true;
}

float angleBetween(const aiVector3D& a, const aiVector3D& b) {
    return acos(a * b / (a - b).SquareLength());
}

aiQuaternion getUpRotation(const aiVector3D& euler) {
    return aiQuaternion(euler.y * M_PI / 180.0f, euler.z * M_PI / 180.0f, euler.x * M_PI / 180.0f);
}

/**
 * F3DEX2 - 32 vertices in buffer
 * F3D - 16 vetcies in buffer
 */

int main(int argc, char *argv[]) {
    CommandLineArguments args;

    if (!parseCommandLineArguments(argc, argv, args)) {
        return 1;
    }

    DisplayListSettings settings = DisplayListSettings();

    settings.mGraphicsScale = args.mGraphicsScale;
    settings.mCollisionScale = args.mCollisionScale;
    settings.mRotateModel = getUpRotation(args.mEulerAngles);
    settings.mPrefix = args.mPrefix;
    settings.mExportAnimation = args.mExportAnimation;
    settings.mExportGeometry = args.mExportGeometry;

    bool hasError = false;

    for (auto materialFile = args.mMaterialFiles.begin(); materialFile != args.mMaterialFiles.end(); ++materialFile) {
        if (!parseMaterials(*materialFile, settings)) {
            hasError = true;
        }
    }

    if (hasError) {
        return 1;
    }

    std::cout << "Generating from mesh "  << args.mInputFile << std::endl;
    const aiScene* scene = loadScene(args.mInputFile, args.mIsLevel, settings.mVertexCacheSize);

    if (!scene) {
        return 1;
    }
    std::cout << "Saving to "  << args.mOutputFile << std::endl;
    // generateMeshFromSceneToFile(scene, args.mOutputFile, settings);

    MeshDefinitionGenerator meshGenerator(settings);

    meshGenerator.TraverseScene(scene);
    CFileDefinition fileDef(settings.mPrefix, settings.mGraphicsScale, settings.mRotateModel);
    meshGenerator.GenerateDefinitions(scene, fileDef);

    CollisionGenerator colliderGenerator(settings);
    colliderGenerator.TraverseScene(scene);
    colliderGenerator.GenerateDefinitions(scene, fileDef);

    fileDef.GenerateAll(args.mOutputFile);
    
    return 0;
}