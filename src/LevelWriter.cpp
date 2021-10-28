
#include "LevelWriter.h"
#include <string>
#include <stdlib.h>

void populateLevelRecursive(const aiScene* scene, class LevelDefinition& levelDef, aiNode* node, const aiMatrix4x4& transform) {
    std::string nodeName = node->mName.C_Str();

    if (nodeName.rfind("Base", 0) == 0) {
        BaseDefinition base;
        base.team = atoi(nodeName.c_str() + 5);

        aiQuaternion rotation;
        transform.DecomposeNoScaling(rotation, base.position);

        levelDef.bases.push_back(base);
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

void generateLevelFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings) {

}