#ifndef _EXTENDED_MESH_H
#define _EXTENDED_MESH_H

#include <assimp/mesh.h>
#include "BoneHierarchy.h"
#include <vector>
#include <map>

class ExtendedMesh {
public:
    ExtendedMesh(aiMesh* mesh, BoneHierarchy& boneHierarchy);
    aiMesh* mMesh;
    std::vector<Bone*> mUsesBones;
    std::vector<Bone*> mVertexBones;
    std::map<Bone*, std::vector<aiFace*>> mFacesForBone;
    // first bone in pair is always the parent of the second
    std::map<std::pair<Bone*, Bone*>, std::vector<aiFace*>> mBoneSpanningFaces;

    bool isFaceOneBone(aiFace* face);
    std::pair<Bone*, Bone*> findTransitionPairForFace(aiFace* face);
private:
    void PopulateFacesForBone();
};

#endif