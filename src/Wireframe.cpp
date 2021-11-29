#include "Wireframe.h"
#include "LevelWriter.h"
#include "MeshWriter.h"

#define WIREFRAME_SIZE  20.0f

aiColor4D gWireframeFactionColors[] = {
    aiColor4D(24 / 255.0f, 152 / 255.0f, 255 / 255.0f, 1.0),
    aiColor4D(255 / 255.0f, 120 / 255.0f, 180 / 255.0f, 1.0),
    aiColor4D(255 / 255.0f, 200 / 255.0f, 20 / 255.0f, 1.0),
    aiColor4D(0 / 255.0f, 255 / 255.0f, 20 / 255.0f, 1.0),
    aiColor4D(100 / 255.0f, 100 / 255.0f, 100 / 255.0f, 1.0),
};

aiColor4D gWireframeColor(64 / 255.0f, 1.0f, 32 / 255.0f, 1.0);

void generateLevelWireframe(LevelDefinition& levelDef, const aiScene* scene, ThemeWriter* theme, DisplayListSettings& settings, std::ostream& fileContent) {
    fileContent << "#include <ultra64.h>" << std::endl;
    fileContent << std::endl;

    BoneHierarchy blankBones;
    CFileDefinition fileDefinition(settings.mPrefix + "_wireframe");
    DisplayList sceneDisplayList(fileDefinition.GetUniqueName("model_gfx"));

    std::vector<RenderChunk> renderChunks;

    aiMatrix4x4 rotation(settings.mRotateModel.GetMatrix());

    aiMatrix4x4 normalizeMatrix;
    aiMatrix4x4 translate;
    aiMatrix4x4 scale;
    normalizeMatrix.Translation(-(levelDef.minBoundary + levelDef.maxBoundary) * (0.5f / settings.mScale), translate);

    aiVector3D boundarySize = levelDef.maxBoundary - levelDef.minBoundary;

    float scaleAmount = settings.mScale * settings.mScale * WIREFRAME_SIZE / std::max(boundarySize.x, boundarySize.z);
    normalizeMatrix.Scaling(aiVector3D(scaleAmount, scaleAmount, scaleAmount), scale);

    normalizeMatrix = scale * translate;

    for (auto geoMesh : levelDef.geometryMeshes) {
        ExtendedMesh* meshCopy = new ExtendedMesh(copyMesh(geoMesh), blankBones);
        meshCopy->Transform(normalizeMatrix * rotation);
        meshCopy->ReplaceColor(gWireframeColor);
        renderChunks.push_back(RenderChunk(
            std::pair<Bone*, Bone*>(nullptr, nullptr), 
            meshCopy,
            VertexType::PosUVColor
        ));
    }

    for (auto decor : levelDef.decor) {
        const ThemeMesh* themeMesh = theme->GetThemeMesh(decor.decorID);

        if (!themeMesh || (!themeMesh->mesh && !themeMesh->wireMesh)) {
            continue;
        }

        aiMatrix4x4 subMatrix;
        subMatrix.Translation(decor.position / settings.mScale, translate);
        subMatrix = normalizeMatrix * translate * aiMatrix4x4(decor.rotation.GetMatrix());

        ExtendedMesh* mesh = themeMesh->mesh;

        if (themeMesh->wireMesh) {
            mesh = themeMesh->wireMesh;
        }

        mesh = new ExtendedMesh(copyMesh(mesh->mMesh), blankBones);
        mesh->Transform(subMatrix);
        mesh->ReplaceColor(gWireframeColor);
        renderChunks.push_back(RenderChunk(
            std::pair<Bone*, Bone*>(nullptr, nullptr), 
            mesh,
            VertexType::PosUVColor
        ));
    }

    if (levelDef.baseMesh) {
        for (auto base : levelDef.bases) {
            aiMatrix4x4 subMatrix;
            subMatrix.Translation(base.position / settings.mScale, translate);
            subMatrix.Scaling(aiVector3D(2.0f, 5.0f, 2.0f), scale);
            subMatrix = normalizeMatrix * translate * scale * aiMatrix4x4(settings.mRotateModel.GetMatrix());

            ExtendedMesh* mesh = new ExtendedMesh(copyMesh(levelDef.baseMesh), blankBones);
            mesh->Transform(subMatrix);
            mesh->ReplaceColor(gWireframeFactionColors[base.team]);
            renderChunks.push_back(RenderChunk(
                std::pair<Bone*, Bone*>(nullptr, nullptr), 
                mesh,
                VertexType::PosUVColor
            ));
        }
    }

    generateMeshIntoDLWithMaterials(scene, fileDefinition, nullptr, renderChunks, settings, sceneDisplayList);
    fileDefinition.GenerateVertexBuffers(fileContent, 1.0f, aiQuaternion());

    sceneDisplayList.Generate(fileDefinition, fileContent);
}