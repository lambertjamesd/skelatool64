#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>

#include "src/SceneWriter.h"
#include "src/SceneModification.h"
#include "src/CommandLineParser.h"
#include "src/MaterialParser.h"

bool parseMaterials(const std::string& filename, DisplayListSettings& output) {
    std::fstream file(filename, std::ios::in);

    struct ParseResult parseResult;
    parseMaterialFile(file, parseResult);
    output.mMaterials.insert(parseResult.mMaterialFile.mMaterials.begin(), parseResult.mMaterialFile.mMaterials.end());

    return true;
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

aiQuaternion getUpRotation(const aiScene* scene) {
    // aiVector3D upVector(0.0f, 0.0f, 0.0f);
    // aiVector3D frontVector(0.0f, 0.0f, 0.0f);
    // aiVector3D originalUpVector(0.0f, 0.0f, 1.0f);
    // aiVector3D originalFrontVector(0.0f, 1.0f, 0.0f);

    // aiQuaternion result;

    // if (getVectorByName(scene, "UpAxis", upVector)) {
    //     getVectorByName(scene, "OriginalUpAxis", originalUpVector);

    //     aiVector3D modifiedUp = result.Rotate(originalUpVector);

    //     aiVector3D upRotateVector = modifiedUp ^ upVector;
    //     float vectorAngles = angleBetween(originalFrontVector, frontVector);

    //     if (upRotateVector.SquareLength() > 0.01f) {
    //         result = aiQuaternion(upRotateVector, vectorAngles) * result;
    //     } else if (vectorAngles < -0.5f) {
    //         result = aiQuaternion(M_PI, 0.0f, 0.0f) * result;
    //     }
    // }

    // if (getVectorByName(scene, "FrontAxis", frontVector)) {
    //     getVectorByName(scene, "OriginalFrontAxis", frontVector);

    //     aiVector3D forwardRotateVector = originalFrontVector ^ frontVector;
    //     float vectorAngles = angleBetween(originalFrontVector, frontVector);
    //     aiQuaternion result;
        
    //     if (forwardRotateVector.SquareLength() > 0.01f) {
    //         result = aiQuaternion(forwardRotateVector, vectorAngles) * result;
    //     } else if (vectorAngles < -0.5f) {/*  */
    //         result = aiQuaternion(M_PI, 0.0f, 0.0f) * result;
    //     }
    // }

    // return result;

    return aiQuaternion(aiVector3D(1.0f, 0.0f, 0.0f), -M_PI * 0.5f) * aiQuaternion(aiVector3D(0.0f, 0.0f, 1.0f), M_PI * 0.5f);
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

    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 1);
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

    const aiScene* scene = importer.ReadFile(
        args.mInputFile, 
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_LimitBoneWeights |
        aiProcess_SortByPType |
        aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph
    );

    if (scene == nullptr) {
        std::cerr << "Error loading input file: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    splitSceneByBones(const_cast<aiScene*>(scene));

    DisplayListSettings settings = DisplayListSettings();

    settings.mScale = args.mScale;
    settings.mPrefix = args.mPrefix;
    settings.mRotateModel = getUpRotation(scene);

    // TOOO - change for other versions of f3d
    importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, settings.mVertexCacheSize);
    importer.ApplyPostProcessing(aiProcess_ImproveCacheLocality);

    bool hasError = false;

    for (auto materialFile = args.mMaterialFiles.begin(); materialFile != args.mMaterialFiles.end(); ++materialFile) {
        if (!parseMaterials(*materialFile, settings)) {
            hasError = true;
        }
    }

    if (hasError) {
        return 1;
    }

    generateMeshFromSceneToFile(scene, args.mOutputFile, settings);
    
    return 0;
}