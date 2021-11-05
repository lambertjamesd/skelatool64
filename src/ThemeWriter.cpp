#include "ThemeWriter.h"

#include <string.h>
#include <fstream>
#include <sstream>
#include "SceneLoader.h"
#include "LevelWriter.h"
#include "FileUtils.h"
#include "CFileDefinition.h"
#include "DisplayList.h"
#include "DisplayListGenerator.h"

ThemeWriter::ThemeWriter(const std::string& themeName) : mThemeName(themeName) {
    
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
    BoneHierarchy noBones;
    for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
        aiMesh* mesh = scene->mMeshes[meshIndex];
        std::string decorName;
        if (GetDecorName(mesh->mName.C_Str(), decorName)) {
            if (mDecorMeshes.find(decorName) == mDecorMeshes.end()) {
                ThemeMesh themeMesh;
                themeMesh.mesh = new ExtendedMesh(mesh, noBones);
                themeMesh.materialName = scene->mMaterials[mesh->mMaterialIndex]->GetName().C_Str();
                themeMesh.index = mDecorMeshes.size();
                mDecorMeshes[decorName] = themeMesh;
            }
        }
    }
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

std::string writeMaterials(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, CFileDefinition& fileDef, DisplayListSettings& settings) {
    std::string decorMaterials = fileDef.GetUniqueName("DecorMaterials");

    std::set<std::shared_ptr<MaterialResource>> resourcesAsSet;
    std::map<std::string, std::string> nameMapping;

    for (auto mesh : meshList) {
        auto material = settings.mMaterials.find(mesh->materialName);

        if (material != settings.mMaterials.end()) {

            for (auto resource : material->second.mUsedResources) {
                resourcesAsSet.insert(resource);
            }
        }
    }

    std::vector<std::shared_ptr<MaterialResource>> resourcesAsVector;

    for (auto resource : resourcesAsSet) {
        resourcesAsVector.push_back(resource);
        nameMapping[resource->mName] = fileDef.GetUniqueName(resource->mName);
    }

    Material::WriteResources(resourcesAsVector, nameMapping, fileDef, cfile);

    std::vector<std::string> displayListNames;

    for (auto mesh : meshList) {
        auto material = settings.mMaterials.find(mesh->materialName);

        if (material != settings.mMaterials.end()) {
            std::string dlName = fileDef.GetUniqueName("DecorMaterials");
            displayListNames.push_back(dlName);
            DisplayList dl(dlName);
            material->second.WriteToDL(nameMapping, dl);
            dl.Generate(fileDef, cfile);
        } else {
            displayListNames.push_back("0");
        }
    }

    cfile << "Gfx* " << decorMaterials << "[] = {" << std::endl;

    for (auto materailName : displayListNames) {
        cfile << "    " << materailName << "," << std::endl;
    }

    cfile << "};" << std::endl;

    cfile << std::endl;

    return decorMaterials;
}

std::string writeGeometry(std::ostream& cfile, std::vector<ThemeMesh*>& meshList, CFileDefinition& fileDef, DisplayListSettings& settings) {
    std::string decorDisplayLists = fileDef.GetUniqueName("DecorDisplayLists");

    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    std::ostringstream displayLists;

    std::vector<std::string> displayListNames;

    for (auto mesh : meshList) {
        auto material = settings.mMaterials.find(mesh->materialName);
        VertexType vtxType = material == settings.mMaterials.end() ? VertexType::PosUVNormal : material->second.mVetexType;
        int vertexBuffer = fileDef.GetVertexBuffer(mesh->mesh, vtxType);
        RenderChunk chunk(std::pair<Bone*, Bone*>(nullptr, nullptr), mesh->mesh, vtxType);

        std::string dlName = fileDef.GetUniqueName("DecorDisplayLists");
        displayListNames.push_back(dlName);
        DisplayList dl(dlName);

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
        if (mesh->boundary.size()) {
            std::string colliderName = fileDef.GetUniqueName("DecorShapes");
            std::string colliderEdges = fileDef.GetUniqueName("DecorShapeEdges");

            cfile << "struct CollisionPolygonEdge " << colliderEdges << "[] = {" << std::endl;

            // struct Vector2 corner;
            // struct Vector2 normal;
            // float edgeLen;

            cfile << "};" << std::endl;

            cfile << "struct CollisionPolygon " << colliderName << " = {" << std::endl;
            cfile << "    .shapeCommon = {CollisionShapeTypePolygon}," << std::endl;
            cfile << "    .boundingBox = {{0, 0}, {0, 0}}," << std::endl;
            cfile << "    .edges = " << colliderEdges << "," << std::endl;
            cfile << "    .edgeCount = " << mesh->boundary.size() << "," << std::endl;
            cfile << "};" << std::endl;

            displayListNames.push_back("(struct CollisionShape*)&colliderName");
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
        meshList.push_back(&it.second);
    }

    std::sort(meshList.begin(), meshList.end(), [](const ThemeMesh* a, const ThemeMesh* b) -> bool {
        return a->index < b->index;
    });

    std::string decorMaterials = writeMaterials(cfile, meshList, fileDef, settings);
    std::string decorDisplayLists = writeGeometry(cfile, meshList, fileDef, settings);
    std::string decorShapes = writeCollision(cfile, meshList, fileDef, settings);

    cfile << "struct ThemeDefinition " << mThemeName << "Theme = {" << std::endl;
    cfile << "    .decorMaterials = " << decorMaterials << "," << std::endl;
    cfile << "    .decorDisplayLists = " << decorDisplayLists << "," << std::endl;
    cfile << "    .decorShapes = " << decorShapes << "," << std::endl;
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

    headerFile << "extern struct ThemeDefinition " << mThemeName << "Theme;" << std::endl;

    headerFile << std::endl;

    headerFile << "#endif" << std::endl;

    headerFile.close();
}

void generateThemeDefiniton(ThemeDefinition& themeDef, DisplayListSettings& settings) {
    ThemeWriter themeWriter(themeDef.mCName);
    std::vector<LevelTheme> levels;

    for (auto it = themeDef.mLevels.begin(); it != themeDef.mLevels.end(); ++it) {
        LevelTheme level;
        level.scene = loadScene(it->mFilename, true, settings.mVertexCacheSize);

        if (!level.scene) {
            return;
        }

        level.definition = *it;

        themeWriter.AppendContentFromScene(level.scene, settings);

        levels.push_back(level);
    }

    themeWriter.WriteThemeHeader(themeDef.mOutput, settings);
    themeWriter.WriteTheme(themeDef.mOutput, settings);

    for (auto it = levels.begin(); it != levels.end(); ++it) {
        DisplayListSettings levelSettings = settings;
        levelSettings.mPrefix = it->definition.mCName;
        generateLevelFromSceneToFile(it->scene, it->definition.mOutput, levelSettings);
    }
}