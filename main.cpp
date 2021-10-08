#include <assimp/Importer.hpp>
#include <assimp/mesh.h>
#include <assimp/postprocess.h>

#include <iostream>

/**
 * F3DEX2 - 32 vertices in buffer
 * F3D - 16 vetcies in buffer
 */

int main() {
    Assimp::Importer importer;

    importer.SetPropertyInteger(AI_CONFIG_PP_LBW_MAX_WEIGHTS, 1);
    importer.SetPropertyInteger(AI_CONFIG_PP_ICL_PTCACHE_SIZE, 32);
    importer.SetPropertyInteger(AI_CONFIG_PP_SBP_REMOVE, aiPrimitiveType_POINT | aiPrimitiveType_LINE);

    const aiScene* scene = importer.ReadFile(
        "/home/james/Documents/Blender/OnlyCube.fbx", 
        aiProcess_JoinIdenticalVertices |
        aiProcess_Triangulate |
        aiProcess_LimitBoneWeights |
        aiProcess_ImproveCacheLocality |
        aiProcess_SortByPType |
        aiProcess_OptimizeMeshes |
        aiProcess_OptimizeGraph
    );

    if (scene == nullptr) {
        std::cout << "Error loading input file: " << importer.GetErrorString() << std::endl;
        return 1;
    }

    std::cout << "Hello World!" << (size_t)scene << std::endl;
    
    return 0;
}