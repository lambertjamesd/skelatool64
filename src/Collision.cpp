#include "Collision.h"

#include <set>
#include <map>

void findEdges(aiMesh* mesh, std::set<std::pair<unsigned, unsigned>>& edges) {
    for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
        aiFace* face = &mesh->mFaces[faceIndex];
        for (unsigned index = 0; index < face->mNumIndices; ++index) {
            unsigned a = face->mIndices[index];
            unsigned b = face->mIndices[(index + 1) % face->mNumIndices];

            if (a > b) {
                unsigned tmp = a;
                a = b;
                b = tmp;
            }

            edges.insert(std::make_pair(a, b));
        }
    }
}

void remapEdges(std::set<std::pair<unsigned, unsigned>>& prevEdges, std::set<std::pair<unsigned, unsigned>>& newEdges, std::map<unsigned, unsigned>& indexMapping) {
    for (auto it = prevEdges.begin(); it != prevEdges.end(); ++it) {
        auto firstIndex = indexMapping.find(it->first);
        if (firstIndex == indexMapping.end()) {
            continue;
        }
        auto secondIndex = indexMapping.find(it->second);
        if (secondIndex == indexMapping.end()) {
            continue;
        }
        unsigned a = firstIndex->second;
        unsigned b = secondIndex->second;

        if (a > b) {
            unsigned tmp = a;
            a = b;
            b = tmp;
        }

        newEdges.insert(std::make_pair(a, b));
    }
}

void reorderMeshBoundary(std::vector<aiVector3D>& unordered, std::set<std::pair<unsigned, unsigned>>& edges, std::vector<aiVector3D>& result) {
    std::set<unsigned> included;
    included.insert(0);
    unsigned curr = 0;
    result.push_back(unordered[0]);

    bool hasMore = true;

    while (hasMore) {
        hasMore = false;

        for (unsigned i = 0; i < unordered.size(); ++i) {
            // if the point is already included then skip it
            if (included.find(i) != included.end()) {
                continue;
            }

            if (edges.find(i < curr ? std::make_pair(i, curr) : std::make_pair(curr, i)) != edges.end()) {
                included.insert(i);
                result.push_back(unordered[i]);
                curr = i;
                hasMore = true;
                break;
            }
        }
    }
}

int getExistingIndex(std::vector<aiVector3D>& vertices, const aiVector3D& check) {
    for (unsigned i = 0; i < vertices.size(); ++i) {
        if ((vertices[i] - check).SquareLength() < 0.01f) {
            return i;
        }
    }

    return -1;
}

void extractMeshBoundary(aiMesh* mesh, const aiMatrix4x4& transform, std::vector<aiVector3D>& result) {
    std::vector<aiVector3D> transformed;
    std::set<std::pair<unsigned, unsigned>> edges;
    findEdges(mesh, edges);

    for (unsigned i = 0; i < mesh->mNumVertices; ++i) {
        transformed.push_back(transform * mesh->mVertices[i]);
    }

    float minY = transformed[0].y;

    for (unsigned i = 1; i < transformed.size(); ++i) {
        minY = std::min(minY, transformed[i].y);
    }

    std::map<unsigned, unsigned> indexMapping;

    std::vector<aiVector3D> filtered;

    for (unsigned i = 0; i < transformed.size(); ++i) {
        if (fabsf(transformed[i].y - minY) < 0.1f) {
            int existing = getExistingIndex(filtered, transformed[i]);

            if (existing >= 0) {
                indexMapping[i] = (unsigned)existing;
            }  else {
                indexMapping[i] = filtered.size();
                filtered.push_back(transformed[i]);
            }

        }
    }

    std::set<std::pair<unsigned, unsigned>> remappedEdges;
    remapEdges(edges, remappedEdges, indexMapping);
    reorderMeshBoundary(filtered, remappedEdges, result);

    float winding = 0;

    for (unsigned i = 0; i < result.size(); ++i) {
        aiVector3D curr = result[i];
        aiVector3D next = result[(i + 1) % result.size()];
        curr.y = 0.0f;
        next.y = 0.0f;

        aiVector3D cross = curr ^ next;
        winding += cross.y;
    }
    
    if (winding > 0.0f) {
        std::reverse(result.begin(), result.end());
    }

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