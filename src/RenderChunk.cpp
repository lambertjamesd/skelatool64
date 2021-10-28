
#include "RenderChunk.h"
#include <algorithm>

RenderChunk::RenderChunk(std::pair<Bone*, Bone*> bonePair, ExtendedMesh* mesh, VertexType vertexType): 
    mBonePair(bonePair),
    mMesh(mesh),
    mVertexType(vertexType) {

}


const std::vector<aiFace*>& RenderChunk::GetFaces() {
    if (mBonePair.first == mBonePair.second) {
        auto result = mMesh->mFacesForBone.find(mBonePair.first);
        return result->second;
    } else {
        auto result = mMesh->mBoneSpanningFaces.find(mBonePair);
        return result->second;
    }
}

void extractChunks(std::vector<std::unique_ptr<ExtendedMesh>>& meshes, std::vector<RenderChunk>& result) {
    for (auto it = meshes.begin(); it != meshes.end(); ++it) {
        for (auto boneSegment = (*it)->mFacesForBone.begin(); boneSegment != (*it)->mFacesForBone.end(); ++boneSegment) {
            result.push_back(RenderChunk(
                std::make_pair(boneSegment->first, boneSegment->first),
                it->get(),
                VertexType::PosUVNormal
            ));
        }

        for (auto pairSegment = (*it)->mBoneSpanningFaces.begin(); pairSegment != (*it)->mBoneSpanningFaces.end(); ++pairSegment) {
            result.push_back(RenderChunk(pairSegment->first, it->get(), VertexType::PosUVNormal));
        }
    }
}

void orderChunks(std::vector<RenderChunk>& result) {
    // TODO solve the traveling salesman algorithm
    std::sort(result.begin(), result.end(), 
        [](const RenderChunk& a, const RenderChunk& b) -> bool {
            int aSecondScore = Bone::GetBoneIndex(a.mBonePair.second);
            int bSecondScore = Bone::GetBoneIndex(b.mBonePair.second);

            if (aSecondScore == bSecondScore) {
                return Bone::GetBoneIndex(a.mBonePair.first) < Bone::GetBoneIndex(b.mBonePair.first);
            }
            
            return aSecondScore < bSecondScore;
    });
}