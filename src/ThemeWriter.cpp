#include "ThemeWriter.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include <algorithm>
#include "SceneLoader.h"
#include "LevelWriter.h"
#include "FileUtils.h"
#include "CFileDefinition.h"
#include "DisplayList.h"
#include "DisplayListGenerator.h"
#include "Collision.h"
#include "StringUtils.h"

ThemeMesh::ThemeMesh(): mesh(nullptr), wireMesh(nullptr), index(0) {

}

ThemeWriter::ThemeWriter(const std::string& themeName, const std::string& themeHeader) : mThemeName(themeName), mThemeHeader(themeHeader), mSkybox(nullptr) {
    
}

bool GetDecorWireName(const std::string& nodeName, std::string& output) {
    if (nodeName.rfind("DecorWire ") != 0) {
        return false;
    }

    unsigned nameStart = strlen("DecorWire ");
    unsigned nameEnd = nameStart;

    while (nameEnd < nodeName.length() && nodeName[nameEnd] != '.') {
        ++nameEnd;
    }

    output = nodeName.substr(nameStart, nameEnd - nameStart);

    return true;
}

bool GetDecorGeometryName(const std::string& nodeName, std::string& output) {
    if (nodeName.rfind("DecorBoundary ") != 0) {
        return false;
    }

    unsigned nameStart = strlen("DecorBoundary ");
    unsigned nameEnd = nameStart;

    while (nameEnd < nodeName.length() && nodeName[nameEnd] != '.') {
        ++nameEnd;
    }

    output = nodeName.substr(nameStart, nameEnd - nameStart);

    return true;
}

bool ThemeWriter::GetDecorName(const std::string& nodeName, std::string& output) {
    if (nodeName.rfind("Decor ") != 0) {
        return false;
    }

    unsigned nameStart = strlen("Decor ");
    unsigned nameEnd = nameStart;

    while (nameEnd < nodeName.length() && nodeName[nameEnd] != '.') {
        ++nameEnd;
    }

    output = nodeName.substr(nameStart, nameEnd - nameStart);

    return true;
}

void ThemeWriter::AppendContentFromScene(const aiScene* scene, DisplayListSettings& settings) {
    aiMatrix4x4 worldScale;
    aiMatrix4x4::Scaling(aiVector3D(settings.mScale, settings.mScale, settings.mScale), worldScale);
    aiMatrix4x4 worldTransform = aiMatrix4x4(settings.mRotateModel.GetMatrix()) * settings.mScale;
    std::vector<RenderChunk> chunks;

    BoneHierarchy noBones;
    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        std::string decorName;
        if (GetDecorName(mesh->mName.C_Str(), decorName)) {
            auto existing = mDecorMeshes.find(decorName);

            if (existing == mDecorMeshes.end()) {
                ThemeMesh themeMesh;
                themeMesh.objectName = decorName;
                themeMesh.mesh = new ExtendedMesh(mesh, noBones);
                themeMesh.materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
                themeMesh.index = mDecorMeshes.size();
                mDecorMeshes[decorName] = themeMesh;

                chunks.push_back(RenderChunk(std::pair<Bone*, Bone*>(nullptr, nullptr), themeMesh.mesh, VertexType::PosUVColor));
            } else if (existing->second.mesh == nullptr) {
                existing->second.mesh = new ExtendedMesh(mesh, noBones);
                existing->second.materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
                chunks.push_back(RenderChunk(std::pair<Bone*, Bone*>(nullptr, nullptr), existing->second.mesh, VertexType::PosUVColor));
            }
        } else if (GetDecorGeometryName(mesh->mName.C_Str(), decorName)) {
            auto existing = mDecorMeshes.find(decorName);

            if (existing == mDecorMeshes.end()) {
                ThemeMesh themeMesh;
                themeMesh.objectName = decorName;
                themeMesh.mesh = nullptr;
                themeMesh.materialName = "";
                themeMesh.index = mDecorMeshes.size();
                extractMeshBoundary(mesh, worldTransform, themeMesh.boundary);
                mDecorMeshes[decorName] = themeMesh;
            } else if (existing->second.boundary.size() == 0) {
                extractMeshBoundary(mesh, worldTransform, existing->second.boundary);
            }
        } else if (GetDecorWireName(mesh->mName.C_Str(), decorName)) {
            auto existing = mDecorMeshes.find(decorName);

            if (existing == mDecorMeshes.end()) {
                ThemeMesh themeMesh;
                themeMesh.objectName = decorName;
                themeMesh.wireMesh = new ExtendedMesh(mesh, noBones);
                themeMesh.materialName = "";
                themeMesh.index = mDecorMeshes.size();
                mDecorMeshes[decorName] = themeMesh;
            } else if (!existing->second.wireMesh) {
                existing->second.wireMesh = new ExtendedMesh(mesh, noBones);
            }
        } else if (std::string(mesh->mName.C_Str()) == "Skybox") {
            mSkyboxMaterial = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
            mSkybox = new ExtendedMesh(mesh, noBones);
        }
    }

    AppendContentFromNode(scene, scene->mRootNode, settings);

    mMaterialCollector.CollectMaterialResources(scene, chunks, settings);
}

void ThemeWriter::AppendContentFromNode(const aiScene* scene, const aiNode* node, DisplayListSettings& settings) {
    std::string nodeName = node->mName.C_Str();

    if (nodeName.rfind("Geometry", 0) == 0) {
        for (unsigned i = 0; i < node->mNumMeshes; ++i) {
            auto mesh = scene->mMeshes[node->mMeshes[i]];
            std::string materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
            mMaterialCollector.UseMaterial(materialName, settings);
        }
    }

    for (unsigned i = 0; i < node->mNumChildren; ++i) {
        AppendContentFromNode(scene, node->mChildren[i], settings);
    }
}

std::string ThemeWriter::GetDecorID(const std::string& name) {
    std::string result = name + "_DECOR_ID";
    makeCCompatible(result);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

unsigned ThemeWriter::GetMaterialIndex(const std::string& name) {
    for (unsigned i = 0; i < mUsedMaterials.size(); ++i) {
        if (mUsedMaterials[i] == name) {
            return i;
        }
    }

    mUsedMaterials.push_back(name);
    return mUsedMaterials.size() - 1;
}

std::string ThemeWriter::WriteMaterials(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, CFileDefinition& fileDef, DisplayListSettings& settings) {
    std::string decorMaterials = fileDef.GetUniqueName("DecorMaterials");

    std::set<std::shared_ptr<MaterialResource>> resourcesAsSet;
    std::map<std::string, std::string> nameMapping;

    for (auto mesh : meshList) {
        mMaterialCollector.UseMaterial(mesh->materialName, settings);
    }

    std::vector<std::shared_ptr<MaterialResource>> resourcesAsVector;

    for (auto resource : resourcesAsSet) {
        resourcesAsVector.push_back(resource);
        nameMapping[resource->mName] = fileDef.GetUniqueName(resource->mName);
    }

    mMaterialCollector.GenerateMaterials(fileDef, settings, cfile);

    std::string currentMaterial = "";

    for (auto mesh : meshList) {
        auto material = mMaterialCollector.mMaterialNameMapping.find(mesh->materialName);

        if (material != mMaterialCollector.mMaterialNameMapping.end()) {
            mDecorMaterialNames.push_back(material->second);
        } else {
            mDecorMaterialNames.push_back("0");
        }
    }

    cfile << "Gfx* " << decorMaterials << "[] = {" << std::endl;

    for (auto materailName : mDecorMaterialNames) {
        cfile << "    " << materailName << "," << std::endl;
    }

    cfile << "};" << std::endl;

    cfile << std::endl;

    return decorMaterials;
}

std::string writeGeometry(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, std::vector<std::string>& displayListNames, CFileDefinition& fileDef, DisplayListSettings& settings) {
    std::string decorDisplayLists = fileDef.GetUniqueName("DecorDisplayLists");

    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    std::ostringstream displayLists;

    for (auto mesh : meshList) {
        if (!mesh->mesh) {
            displayListNames.push_back("0");
            continue;
        }

        auto material = settings.mMaterials.find(mesh->materialName);
        VertexType vtxType = material == settings.mMaterials.end() ? VertexType::PosUVColor : material->second.mVertexType;
        int vertexBuffer = fileDef.GetVertexBuffer(mesh->mesh, vtxType);
        RenderChunk chunk(std::pair<Bone*, Bone*>(nullptr, nullptr), mesh->mesh, vtxType);

        std::string dlName = fileDef.GetUniqueName(mesh->objectName + "DisplayList");
        displayListNames.push_back(dlName);
        DisplayList dl(dlName);
        if (settings.mIncludeCulling) {
            generateCulling(dl, fileDef.GetCullingBuffer(mesh->objectName + "Culling", mesh->mesh->bbMin, mesh->mesh->bbMax), vtxType == VertexType::PosUVNormal);
        }
        generateGeometry(chunk, rcpState, vertexBuffer, dl, settings.mHasTri2);
        dl.Generate(fileDef, displayLists);

        displayLists << std::endl;
    }

    fileDef.GenerateVertexBuffers(cfile, settings.mScale, settings.mRotateModel);

    cfile << std::endl;

    cfile << displayLists.str();

    cfile << "Gfx* " << decorDisplayLists << "[] = {" << std::endl;

    for (auto geometryName : displayListNames) {
        cfile << "    " << geometryName << "," << std::endl;
    }

    cfile << "};" << std::endl;

    cfile << std::endl;

    return decorDisplayLists;
}

std::string writeCollision(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, CFileDefinition& fileDef, DisplayListSettings& settings) {
    std::string decorShapes = fileDef.GetUniqueName("DecorShapes");

    std::vector<std::string> displayListNames;
    
    for (auto mesh : meshList) {
        if (mesh->boundary.size() == 1) {
            aiVector3D radius = mesh->boundary[0];
            radius.y = 0.0f;
            
            std::string colliderName = fileDef.GetUniqueName(mesh->objectName + "Shape");
            cfile << "struct CollisionCircle " << colliderName << " = {" << std::endl;
            cfile << "    .shapeCommon = {CollisionShapeTypeCircle}," << std::endl;
            cfile << "    .radius = " << radius.Length() << "," << std::endl;
            cfile << "};" << std::endl;

            displayListNames.push_back("(struct CollisionShape*)&" + colliderName);
        } else if (mesh->boundary.size()) {
            std::string colliderName = fileDef.GetUniqueName(mesh->objectName + "Shape");
            std::string colliderEdges = fileDef.GetUniqueName(mesh->objectName + "ShapeEdges");

            cfile << "struct CollisionPolygonEdge " << colliderEdges << "[] = {" << std::endl;

            aiVector3D min = mesh->boundary[0];
            aiVector3D max = mesh->boundary[0];

            unsigned actualEdgeCount = 0;

            for (unsigned i = 0; i < mesh->boundary.size(); ++i) {
                aiVector3D curr = mesh->boundary[i];
                aiVector3D edge = mesh->boundary[(i + 1) % mesh->boundary.size()] - curr;

                if (edge.SquareLength() < 0.001f) {
                    continue;
                }

                curr.y = 0.0f;
                edge.y = 0.0f;

                aiVector3D normal = edge;
                normal.Normalize();

                cfile << "    {";
                cfile << "{" << curr.x << ", " << curr.z << "}, ";
                cfile << "{" << normal.z << ", " << -normal.x << "}, ";
                cfile << edge.Length();
                cfile << "}," << std::endl;

                min.x = std::min(min.x, curr.x);
                min.z = std::min(min.z, curr.z);
                max.x = std::max(max.x, curr.x);
                max.z = std::max(max.z, curr.z);

                ++actualEdgeCount;
            }

            cfile << "};" << std::endl;

            cfile << "struct CollisionPolygon " << colliderName << " = {" << std::endl;
            cfile << "    .shapeCommon = {CollisionShapeTypePolygon}," << std::endl;
            cfile << "    .edges = " << colliderEdges << "," << std::endl;
            cfile << "    .edgeCount = " << actualEdgeCount << "," << std::endl;
            cfile << "};" << std::endl;

            displayListNames.push_back("(struct CollisionShape*)&" + colliderName);
        } else {
            displayListNames.push_back("0");
        }
    }    

    cfile << "struct CollisionShape* " << decorShapes << "[] = {" << std::endl;

    for (auto collisionName : displayListNames) {
        cfile << "    " << collisionName << "," << std::endl;
    }

    cfile << "};" << std::endl;

    cfile << std::endl;

    return decorShapes;
}

void ThemeWriter::WriteTheme(const std::string& output, DisplayListSettings& settings) {
    std::ofstream cfile;
    CFileDefinition fileDef(mThemeName);
    cfile.open(replaceExtension(output, ".c"), std::ios_base::out | std::ios_base::trunc);

    cfile << "#include \"" << getBaseName(replaceExtension(output, ".h")) << "\"" << std::endl;
    cfile << std::endl;

    std::vector<ThemeMesh*> meshList;

    for (auto it : mDecorMeshes) {
        meshList.push_back(new ThemeMesh(it.second));
    }

    std::sort(meshList.begin(), meshList.end(), [](ThemeMesh* a, ThemeMesh* b) -> bool {
        return a->index < b->index;
    });

    if (mSkybox) {
        ThemeMesh* newMesh = new ThemeMesh();
        newMesh->materialName = mSkyboxMaterial;
        newMesh->mesh = mSkybox;
        newMesh->index = meshList.size();
        newMesh->objectName = "Skybox";
        meshList.push_back(newMesh);
    }

    std::string decorMaterials = WriteMaterials(cfile, meshList, fileDef, settings);
    std::string decorDisplayLists = writeGeometry(cfile, meshList, mDecorGeoNames, fileDef, settings);
    std::string decorShapes = writeCollision(cfile, meshList, fileDef, settings);

    std::string skyboxName = "0";
    std::string skyboxMaterialName = "0";

    if (mSkybox) {
        skyboxName = mDecorGeoNames[mDecorGeoNames.size() - 1];
        skyboxMaterialName = mDecorMaterialNames[mDecorMaterialNames.size() - 1];
    }

    cfile << "struct ThemeDefinition " << mThemeName << "Theme = {" << std::endl;
    cfile << "    .skybox = " << skyboxName << "," << std::endl;
    cfile << "    .skyboxMaterial = " << skyboxMaterialName << "," << std::endl;
    cfile << "    .decorMaterials = " << decorMaterials << "," << std::endl;
    cfile << "    .decorDisplayLists = " << decorDisplayLists << "," << std::endl;
    cfile << "    .decorShapes = " << decorShapes << "," << std::endl;
    cfile << "    .decorCount = " << meshList.size() << "," << std::endl;
    cfile << "};" << std::endl;

    cfile.close();
}

void ThemeWriter::WriteThemeHeader(const std::string& output, DisplayListSettings& settings) {
    std::ofstream headerFile;
    headerFile.open(replaceExtension(output, ".h"), std::ios_base::out | std::ios_base::trunc);

    std::string headerName = mThemeName + "_THEME_H";
    std::transform(headerName.begin(), headerName.end(), headerName.begin(), ::toupper);

    headerFile << "#ifndef " << headerName << std::endl;
    headerFile << "#define " << headerName << std::endl;

    headerFile << std::endl;

    headerFile << "#include \"levels/themedefinition.h\"" << std::endl;

    headerFile << std::endl;

    for (auto materialName = mMaterialCollector.mMaterialNameMapping.begin(); materialName != mMaterialCollector.mMaterialNameMapping.end(); ++materialName) {
        headerFile << "extern Gfx " << materialName->second << "[];" << std::endl;
    }

    headerFile << std::endl;

    for (auto decorNames = mDecorGeoNames.begin(); decorNames != mDecorGeoNames.end(); ++decorNames) {
        if (*decorNames != "0") {
            headerFile << "extern Gfx " << *decorNames << "[];" << std::endl;
        }
    }

    for (auto decor = mDecorMeshes.begin(); decor != mDecorMeshes.end(); ++decor) {
        headerFile << "#define " << GetDecorID(decor->first) << " " << decor->second.index << std::endl;
    }

    headerFile << std::endl;

    headerFile << "extern struct ThemeDefinition " << mThemeName << "Theme;" << std::endl;

    headerFile << std::endl;

    headerFile << "#endif" << std::endl;

    headerFile.close();
}

const std::string& ThemeWriter::GetThemeHeader() const {
    return mThemeHeader;
}

const std::string& ThemeWriter::GetThemeName() const {
    return mThemeName;
}

std::string ThemeWriter::GetDecorMaterial(const std::string& decorName) {
    auto mesh = mDecorMeshes.find(decorName);

    if (mesh == mDecorMeshes.end()) {
        return "";
    }

    return mDecorMaterialNames[mesh->second.index];
}

std::string ThemeWriter::GetDecorGeo(const std::string& decorName) {
    auto mesh = mDecorMeshes.find(decorName);

    if (mesh == mDecorMeshes.end()) {
        return "";
    }

    return mDecorGeoNames[mesh->second.index];
}

const ThemeMesh* ThemeWriter::GetThemeMesh(const std::string& id) {
    auto result = mDecorMeshes.find(id);

    if (result == mDecorMeshes.end()) {
        return nullptr;
    }

    return &result->second;
}

void generateThemeDefiniton(ThemeDefinition& themeDef, DisplayListSettings& settings) {
    ThemeWriter themeWriter(themeDef.mCName, replaceExtension(themeDef.mOutput, ".h"));
    std::vector<LevelTheme> levels;

    // force the materials to be written in the theme
    themeWriter.mMaterialCollector.mSceneCount = 2;

    for (auto it = themeDef.mLevels.begin(); it != themeDef.mLevels.end(); ++it) {
        LevelTheme level;
        std::cout << "Loading scene " << it->mFilename << std::endl;
        level.scene = loadScene(it->mFilename, true, settings.mVertexCacheSize);

        if (!level.scene) {
            return;
        }

        level.definition = *it;

        themeWriter.AppendContentFromScene(level.scene, settings);

        levels.push_back(level);
    }

    std::cout << "Saving theme to " << themeDef.mOutput << std::endl;
    themeWriter.WriteTheme(themeDef.mOutput, settings);
    themeWriter.WriteThemeHeader(themeDef.mOutput, settings);

    for (auto it = levels.begin(); it != levels.end(); ++it) {
        if (it->definition.mFlags.size() == 0) {
            std::cout << "Skipping level " << it->definition.mOutput << std::endl;
            continue;
        }

        DisplayListSettings levelSettings = settings;
        levelSettings.mPrefix = it->definition.mCName;
        std::cout << "Saving level to " << it->definition.mOutput << std::endl;
        generateLevelFromSceneToFile(it->scene, it->definition.mOutput, &themeWriter, levelSettings, it->definition.mAIDifficulty);
    }
}