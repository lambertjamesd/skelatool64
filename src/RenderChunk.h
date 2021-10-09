#ifndef _RENDER_CHUNK_H
#define _RENDER_CHUNK_H

#include "ExtendedMesh.h"
#include "BoneHierarchy.h"

#include <vector>
#include <memory>

class RenderChunk {
public:
    RenderChunk(std::pair<Bone*, Bone*> bonePair, ExtendedMesh* mesh);
    // if bones are the same, chunk cooresponds to a single bone
    // the bones can be null
    std::pair<Bone*, Bone*> mBonePair;
    ExtendedMesh* mMesh;

    const std::vector<aiFace*>& GetFaces();
private:
};

void extractChunks(std::vector<std::unique_ptr<ExtendedMesh>>& meshes, std::vector<RenderChunk>& result);

void orderChunks(std::vector<RenderChunk>& result);

#endif