
#include <assimp/mesh.h>

#include <iostream>
#include <fstream>

#include "src/SceneWriter.h"
#include "src/CommandLineParser.h"
#include "src/materials/MaterialParser.h"
#include "src/SceneLoader.h"

#include "src/definition_generator/MeshDefinitionGenerator.h"
#include "src/definition_generator/CollisionGenerator.h"
#include "src/materials/MaterialState.h"

void materialTest(CFileDefinition& fileDef) {
    MaterialState fromState;
    MaterialState toState;

    toState.alphaCompare = AlphaCompare::Dither;
    toState.hasCombineMode = true;
    toState.cycle1Combine.color[3] = ColorCombineSource::EnvironmentColor;
    toState.cycle1Combine.alpha[3] = AlphaCombineSource::EnvironmentAlpha;
    toState.cycle2Combine.color[3] = ColorCombineSource::EnvironmentColor;
    toState.cycle2Combine.alpha[3] = AlphaCombineSource::EnvironmentAlpha;

    toState.hasRenderMode = true;

    toState.geometryModes.SetFlag((int)GeometryMode::G_LIGHTING, true);
    toState.geometryModes.SetFlag((int)GeometryMode::G_SHADE, true);
    toState.geometryModes.SetFlag((int)GeometryMode::G_TEXTURE_ENABLE, false);

    std::unique_ptr<StructureDataChunk> dataChunk(new StructureDataChunk());

    generateMaterial(fromState, toState, *dataChunk);

    dataChunk->Add(std::unique_ptr<DataChunk>(new MacroDataChunk("gsSPEndDisplayList")));

    fileDef.AddDefinition(std::unique_ptr<FileDefinition>(
        new DataFileDefinition(
            "Gfx", 
            "materialTest", 
            true, 
            "_geo", 
            std::move(dataChunk)
        )
    ));
}

bool parseMaterials(const std::string& filename, DisplayListSettings& output) {
    std::fstream file(filename, std::ios::in);

    struct ParseResult parseResult;
    parseMaterialFile(file, parseResult);
    output.mMaterials.insert(parseResult.mMaterialFile.mMaterials.begin(), parseResult.mMaterialFile.mMaterials.end());

    for (auto err : parseResult.mErrors) {
        std::cerr << err.mMessage << std::endl;
    }

    return parseResult.mErrors.size() == 0;
}

bool getVectorByName(const aiScene* scene, const std::string name, aiVector3D& result) {
    int axis;
    if (!scene->mMetaData->Get(name, axis)) {
        return false;
    }

    switch (axis) {
        case 0: result = aiVector3D(1.0f, 0.0f, 0.0f); break;
        case 1: result = aiVector3D(0.0f, 1.0f, 0.0f); break;
        case 2: result = aiVector3D(0.0f, 0.0f, 1.0f); break;
        default: return false;
    }

    int upSign;
    if (scene->mMetaData->Get(name + "Sign", upSign)) {
        if (upSign < 0) {
            result = -result;
        }
    }

    return true;
}

float angleBetween(const aiVector3D& a, const aiVector3D& b) {
    return acos(a * b / (a - b).SquareLength());
}

aiQuaternion getUpRotation(const aiVector3D& euler) {
    return aiQuaternion(euler.y * M_PI / 180.0f, euler.z * M_PI / 180.0f, euler.x * M_PI / 180.0f);
}

/**
 * F3DEX2 - 32 vertices in buffer
 * F3D - 16 vetcies in buffer
 */

int main(int argc, char *argv[]) {
    CommandLineArguments args;

    if (!parseCommandLineArguments(argc, argv, args)) {
        return 1;
    }

    DisplayListSettings settings = DisplayListSettings();

    settings.mGraphicsScale = args.mGraphicsScale;
    settings.mCollisionScale = args.mCollisionScale;
    settings.mRotateModel = getUpRotation(args.mEulerAngles);
    settings.mPrefix = args.mPrefix;
    settings.mExportAnimation = args.mExportAnimation;
    settings.mExportGeometry = args.mExportGeometry;

    bool hasError = false;

    for (auto materialFile = args.mMaterialFiles.begin(); materialFile != args.mMaterialFiles.end(); ++materialFile) {
        if (!parseMaterials(*materialFile, settings)) {
            hasError = true;
        }
    }

    if (hasError) {
        return 1;
    }

    std::cout << "Generating from mesh "  << args.mInputFile << std::endl;
    const aiScene* scene = loadScene(args.mInputFile, args.mIsLevel, settings.mVertexCacheSize);

    if (!scene) {
        return 1;
    }
    std::cout << "Saving to "  << args.mOutputFile << std::endl;
    // generateMeshFromSceneToFile(scene, args.mOutputFile, settings);

    MeshDefinitionGenerator meshGenerator(settings);

    meshGenerator.TraverseScene(scene);
    CFileDefinition fileDef(settings.mPrefix, settings.mGraphicsScale, settings.mRotateModel);
    meshGenerator.GenerateDefinitions(scene, fileDef);

    CollisionGenerator colliderGenerator(settings);
    colliderGenerator.TraverseScene(scene);
    colliderGenerator.GenerateDefinitions(scene, fileDef);

    materialTest(fileDef);

    fileDef.GenerateAll(args.mOutputFile);
    
    return 0;
}