#include "CollisionGenerator.h"

#include "../StringUtils.h"

#include <algorithm>

#define SAME_TOLERANCE  0.00001f

bool bottomRightMost(const aiVector3D& a, const aiVector3D& b) { 
    if (fabs(a.x - b.x) > SAME_TOLERANCE) {
        return a.x < b.x;
    }

    if (fabs(a.y - b.y) > SAME_TOLERANCE) {
        return a.y < b.y;
    }

    return a.z - b.z;
}

CollisionQuad::CollisionQuad(aiMesh* mesh, aiMatrix4x4& transform) {
    if (mesh->mVertices) {
        aiVector3D* cornerPointer = std::min_element(mesh->mVertices, mesh->mVertices + mesh->mNumVertices, bottomRightMost);
        int cornerIndex = cornerPointer - mesh->mVertices;

        std::set<int> adjacentIndices;
        findAdjacentVertices(mesh, cornerIndex, adjacentIndices);
        
        std::vector<aiVector3D> adjacentVertices;

        for (auto index : adjacentIndices) {
            adjacentVertices.push_back(mesh->mVertices[index]);
        }

        auto edgeAPoint = std::max_element(adjacentVertices.begin(), adjacentVertices.end(), bottomRightMost);

        corner = *cornerPointer;
        edgeA = *edgeAPoint - corner;
        edgeALength = edgeA.Length();
        edgeA.Normalize();

        
    } else {
        corner = aiVector3D();
        edgeA = aiVector3D();
        edgeALength = 0.0f;
        edgeB = aiVector3D();
        edgeBLength = 0.0f;
        normal = aiVector3D();
    }
}

std::unique_ptr<DataChunk> CollisionQuad::Generate() {
    std::unique_ptr<StructureDataChunk> result(new StructureDataChunk());

    result->Add(std::unique_ptr<DataChunk>(new StructureDataChunk(corner)));
    result->Add(std::unique_ptr<DataChunk>(new StructureDataChunk(edgeA)));
    result->AddPrimitive(edgeALength);
    result->Add(std::unique_ptr<DataChunk>(new StructureDataChunk(edgeB)));
    result->AddPrimitive(edgeBLength);

    std::unique_ptr<StructureDataChunk> plane(new StructureDataChunk(corner));
    plane->Add(std::unique_ptr<DataChunk>(new StructureDataChunk(normal)));
    plane->AddPrimitive(-(corner * normal));
    result->Add(std::move(plane));

    result->AddPrimitive(0xF);

    return result;
}

CollisionGenerator::CollisionGenerator(const DisplayListSettings& settings) : 
    DefinitionGenerator(), 
    mSettings(settings) {}


bool CollisionGenerator::ShouldIncludeNode(aiNode* node) {
    return StartsWith(node->mName.C_Str(), "@collision");
}

void CollisionGenerator::GenerateDefinitions(const aiScene* scene, CFileDefinition& fileDefinition) {
    std::unique_ptr<StructureDataChunk> collidersChunk(new StructureDataChunk());

    for (auto node = mIncludedNodes.begin(); node != mIncludedNodes.end(); ++node) {
        for (unsigned i = 0; i < (*node)->mNumMeshes; ++i) {
            aiMesh* mesh = scene->mMeshes[(*node)->mMeshes[i]];

            CollisionQuad collider(mesh, (*node)->mTransformation);
            collidersChunk->Add(std::move(collider.Generate()));
        }
    }
    
    fileDefinition.AddDefinition(std::unique_ptr<FileDefinition>(new DataFileDefinition("struct CollisionQuad", "quad_colliders", true, "_geo", std::move(collidersChunk))));
}