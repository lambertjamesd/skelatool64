#include "SceneWriter.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <algorithm>
#include <vector>
#include <string>

#include "./DisplayList.h"
#include "./DisplayListGenerator.h"
#include "./BoneHierarchy.h"
#include "./ExtendedMesh.h"
#include "./RenderChunk.h"
#include "AnimationTranslator.h"
#include "MeshWriter.h"

DisplayListSettings::DisplayListSettings():
    mPrefix(""),
    mVertexCacheSize(MAX_VERTEX_CACHE_SIZE),
    mHasTri2(true),
    mScale(256.0f),
    mMaxMatrixDepth(10),
    mCanPopMultipleMatrices(true),
    mTicksPerSecond(30),
    mExportAnimation(true),
    mExportGeometry(true) {
}

std::vector<SKAnimationHeader> generateAnimationData(const aiScene* scene, BoneHierarchy& bones, CFileDefinition& fileDef, float modelScale, unsigned short targetTicksPerSecond, aiQuaternion rotate, std::ostream& output, std::ostream& animationDef) {
    std::vector<SKAnimationHeader> animations;

    for (unsigned i = 0; i < scene->mNumAnimations; ++i) {
        SKAnimation animation;
        if (translateAnimationToSK(*scene->mAnimations[i], animation, bones, modelScale, targetTicksPerSecond, rotate)) {
            std::string animationName = fileDef.GetUniqueName(scene->mAnimations[i]->mName.C_Str());
            unsigned short firstChunkSize = formatAnimationChunks(animationName, animation.chunks, output);

            SKAnimationHeader header;
            header.firstChunkSize = firstChunkSize;
            header.ticksPerSecond = targetTicksPerSecond;
            header.maxTicks = animation.maxTicks;
            header.animationName = animationName;

            animations.push_back(header);

            animationDef << "extern unsigned short " << animationName << "[];" << std::endl;
        }
    }

    return animations;
}

void generateMeshFromScene(const aiScene* scene, std::ostream& output, std::ostream& headerFile, std::ostream& animationFile, std::ostream& animationDef, DisplayListSettings& settings) {
    CFileDefinition fileDefinition(settings.mPrefix);
    BoneHierarchy bones;
    bool shouldExportAnimations;

    if (settings.mExportAnimation) {
        bones.SearchForBonesInScene(scene);
        shouldExportAnimations = bones.HasData();
    } else {
        shouldExportAnimations = false;
    }

    std::vector<std::unique_ptr<ExtendedMesh>> extendedMeshes;

    for (unsigned int i = 0; i < scene->mNumMeshes; ++i) {
        extendedMeshes.push_back(std::unique_ptr<ExtendedMesh>(new ExtendedMesh(scene->mMeshes[i], bones)));
    }

    std::vector<RenderChunk> renderChunks;

    extractChunks(extendedMeshes, renderChunks);
    orderChunks(renderChunks);

    std::string renderDLName;

    if (settings.mExportGeometry) {
        if (shouldExportAnimations) {
            output << "#include \"sk64/skelatool_defs.h\"" << std::endl;
            output << std::endl;
        }

        renderDLName = generateMesh(scene, fileDefinition, renderChunks, settings, output);
    }

    headerFile << "#ifndef _" << settings.mPrefix << "_H" << std::endl;
    headerFile << "#define _" << settings.mPrefix << "_H" << std::endl;
    headerFile << std::endl;

    headerFile << "#include <ultra64.h>" << std::endl;
    if (shouldExportAnimations) {
        headerFile << "#include \"math/transform.h\"" << std::endl;
        headerFile << "#include \"sk64/skelatool_clip.h\"" << std::endl;
    }

    headerFile << std::endl;
    if (settings.mExportGeometry) {
        headerFile << "extern Gfx " << renderDLName << "[];" << std::endl;
    }

    if (shouldExportAnimations) {        
        std::string bonesName = fileDefinition.GetUniqueName("default_bones");
        std::string boneParentName = fileDefinition.GetUniqueName("bone_parent");
        bones.GenerateRestPosiitonData(fileDefinition, bonesName, animationFile, headerFile, settings.mScale, settings.mRotateModel);
        std::string boneCountName = bonesName + "_COUNT";
        std::transform(boneCountName.begin(), boneCountName.end(), boneCountName.begin(), ::toupper);
        headerFile << "#define " << boneCountName << " " << bones.GetBoneCount() << std::endl;
        headerFile << "extern struct Transform " << bonesName << "[];" << std::endl;
        headerFile << "extern unsigned short " << boneParentName << "[];" << std::endl;

        std::string animationsName = fileDefinition.GetUniqueName("animations");
        auto animations = generateAnimationData(scene, bones, fileDefinition, settings.mScale, settings.mTicksPerSecond, settings.mRotateModel, animationFile, animationDef);
        animationDef << "struct SKAnimationHeader " << animationsName << "[] = {" << std::endl;
        int index = 0;
        for (auto it = animations.begin(); it != animations.end(); ++it) {
            animationDef << "    {" << it->firstChunkSize << ", " << it->ticksPerSecond << ", " << it->maxTicks << ", 0, (struct SKAnimationChunk*)" << it->animationName << ", 0}," << std::endl;

            std::string animationIndex = fileDefinition.GetUniqueName(it->animationName + "_INDEX");
            std::transform(animationIndex.begin(), animationIndex.end(), animationIndex.begin(), ::toupper);
            headerFile << "#define " << animationIndex << " " << index << std::endl;

            ++index;
        }
        animationDef << "};" << std::endl;

        animationDef << "unsigned short " << boneParentName << "[] = {" << std::endl;

        for (unsigned int boneIndex = 0; boneIndex < bones.GetBoneCount(); ++boneIndex) {
            Bone* bone = bones.BoneByIndex(boneIndex);
            if (bone->GetParent()) {
                animationDef << "    " << bone->GetParent()->GetIndex() << ", " << std::endl;
            } else {
                animationDef << "    0xFFFF," << std::endl;
            }
        }

        animationDef << "};" << std::endl;

        headerFile << "extern struct SKAnimationHeader " << animationsName << "[];" << std::endl;
    }

    headerFile << std::endl;
    headerFile << "#endif";
}

void generateMeshFromSceneToFile(const aiScene* scene, std::string filename, DisplayListSettings& settings) {
    std::ostringstream output;
    std::ostringstream header;
    std::ostringstream animation;
    std::ostringstream animationDef;
    generateMeshFromScene(scene, output, header, animation, animationDef, settings);

    if (settings.mExportGeometry) {
        std::ofstream outputFile;
        outputFile.open(filename + "_geo.inc.h", std::ios_base::out | std::ios_base::trunc);
        outputFile << output.str();
        outputFile.close();
    }

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

        std::ofstream animDefOutput;
        animDefOutput.open(filename + "_animdef.inc.h", std::ios_base::out | std::ios_base::trunc);
        animDefOutput << animationDef.str();
        animDefOutput.close();
    }
}