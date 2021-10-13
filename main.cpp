#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <iostream>
#include <fstream>

#include "src/SceneWriter.h"
#include "src/SceneModification.h"
#include "src/CommandLineParser.h"

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

    splitSceneByBones(const_cast<aiScene*>(scene));

    DisplayListSettings settings = DisplayListSettings();

    settings.mScale = args.mScale;
    settings.mPrefix = args.mPrefix;

    // TOOO - change for other versions of f3d
    importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, settings.mVertexCacheSize);
    importer.ApplyPostProcessing(aiProcess_ImproveCacheLocality);

    // importer.ApplyCustomizedPostProcessing();

    if (scene == nullptr) {
        std::cout << "Error loading input file: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    generateMeshFromSceneToFile(scene, args.mOutputFile, settings);
    
    return 0;
}