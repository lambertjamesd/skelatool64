#include "SceneWriter.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>

#include "./DisplayList.h"
#include "./DisplayListGenerator.h"
#include "./BoneHierarchy.h"
#include "./ExtendedMesh.h"
#include "./RenderChunk.h"

DisplayListSettings::DisplayListSettings():
    mPrefix(""),
    mVertexCacheSize(MAX_VERTEX_CACHE_SIZE),
    mHasTri2(true),
    mScale(256.0f),
    mMaxMatrixDepth(10),
    mCanPopMultipleMatrices(true) {

}

void generateMeshFromScene(const aiScene* scene, std::ostream& output, std::ostream& headerFile, std::ostream& animationFile, DisplayListSettings& settings) {
    RCPState rcpState(settings.mVertexCacheSize, settings.mMaxMatrixDepth, settings.mCanPopMultipleMatrices);
    CFileDefinition fileDefinition(settings.mPrefix);
    DisplayList displayList(fileDefinition.GetUniqueName("model_gfx"));
    BoneHierarchy bones;

    bones.SearchForBonesInScene(scene);

    std::vector<std::unique_ptr<ExtendedMesh>> extendedMeshes;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        extendedMeshes.push_back(std::unique_ptr<ExtendedMesh>(new ExtendedMesh(scene->mMeshes[i], bones)));
    }

    std::vector<RenderChunk> renderChunks;

    extractChunks(extendedMeshes, renderChunks);
    orderChunks(renderChunks);

    for (auto chunk = renderChunks.begin(); chunk != renderChunks.end(); ++chunk) {
        // todo apply material
        int vertexBuffer = fileDefinition.GetVertexBuffer(chunk->mMesh, VertexType::PosUVColor);
        generateGeometry(*chunk, rcpState, vertexBuffer, displayList, settings.mHasTri2);
    }

    rcpState.TraverseToBone(nullptr, displayList);


    if (bones.HasData()) {
        output << "#include \"sk64/skelatool_defs.h\"" << std::endl;
        output << std::endl;
    }

    fileDefinition.GenerateVertexBuffers(output, settings.mScale);

    displayList.Generate(fileDefinition, output);

    headerFile << "#ifndef _" << settings.mPrefix << "_H" << std::endl;
    headerFile << "#define _" << settings.mPrefix << "_H" << std::endl;
    headerFile << std::endl;

    headerFile << "#include <ultra64.h>" << std::endl;
    if (bones.HasData()) {
        headerFile << "#include \"math/transform.h\"" << std::endl;
    }

    headerFile << std::endl;
    headerFile << "extern Gfx " << displayList.GetName() << "[];" << std::endl;

    if (bones.HasData()) {        
        std::string bonesName = fileDefinition.GetUniqueName("default_bones");
        bones.GenerateRestPosiitonData(bonesName, animationFile, settings.mScale);
        headerFile << "extern struct Transform " << bonesName << "[];" << std::endl;
        std::string boneCountName = bonesName + "_COUNT";
        std::transform(boneCountName.begin(), boneCountName.end(), boneCountName.begin(), ::toupper);
        headerFile << "#define " << boneCountName << " " << bones.GetBoneCount() << std::endl;
    }

    headerFile << std::endl;
    headerFile << "#endif";
}

void generateMeshFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings) {
    std::ostringstream output;
    std::ostringstream header;
    std::ostringstream animation;
    generateMeshFromScene(scene, output, header, animation, settings);

    std::ofstream outputFile;
    outputFile.open(filename + "_geo.inc.h", std::ios_base::out | std::ios_base::trunc);
    outputFile << output.str();
    outputFile.close();

    std::ofstream outputHeader;
    outputHeader.open(filename + ".h", std::ios_base::out | std::ios_base::trunc);
    outputHeader << header.str();
    outputHeader.close();

    std::string animationContent = animation.str();

    if (animationContent.length()) {
        std::ofstream animOutput;
        animOutput.open(filename + "_anim.inc.h", std::ios_base::out | std::ios_base::trunc);
        animOutput << animationContent;
        animOutput.close();
    }
}