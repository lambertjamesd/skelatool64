#include "Collision.h"

void extractMeshBoundary(aiMesh* mesh, const aiMatrix4x4& transform, std::vector<aiVector3D>& result) {
    std::vector<aiVector3D> transformed;

    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        transformed.push_back(transform * mesh->mVertices[i]);
    }

    float minY = transformed[0].y;

    for (unsigned i = 1; i < transformed.size(); ++i) {
        minY = std::min(minY, transformed[i].y);
    }

    for (unsigned i = 0; i < transformed.size(); ++i) {
        if (fabsf(transformed[i].y - minY) < 0.1f) {
            result.push_back(transformed[i]);
        }
    }

    std::sort(result.begin(), result.end(), [](const aiVector3D& a, const aiVector3D& b) -> bool {
        float aAngle = atan2f(a.z, a.x);
        float bAngle = atan2f(b.z, b.x);

        return aAngle < bAngle;
    });

    aiVector3D last = result[0];

    for (unsigned i = 1; i < result.size();) {
        aiVector3D curr = result[i];

        if ((last - curr).SquareLength() < 0.01f) {
            result.erase(result.begin() + i);
        } else {
            last = curr;
            ++i;
        }
    }

    if (result.size() > 1 && (result[0] - result[result.size() - 1]).SquareLength() < 0.01f) {
        result.erase(result.begin() + result.size() - 1);
    }
}