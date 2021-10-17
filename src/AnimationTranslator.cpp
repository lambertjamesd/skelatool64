
#include "AnimationTranslator.h"

#include <map>
#include <vector>
#include <climits>
#include <algorithm>

struct SKBoneKeyframeChain {
    SKBoneKeyframe keyframe;
    unsigned short tick;
    struct SKBoneKeyframeChain* next;
};

unsigned short keyForKeyframe(const SKBoneKeyframe& keyframe) {
    return ((unsigned short)keyframe.boneIndex << 8) | (unsigned short)keyframe.usedAttributes;
}

bool keyframeSortFn(const SKBoneKeyframeChain& a, const SKBoneKeyframeChain& b) {
    if (a.tick != b.tick) {
        return a.tick < b.tick;
    }
    
    if (a.keyframe.boneIndex != b.keyframe.boneIndex) {
        return a.keyframe.boneIndex < b.keyframe.boneIndex;
    }

    return a.keyframe.usedAttributes < b.keyframe.usedAttributes;
}

void populateKeyframes(const aiAnimation& input, BoneHierarchy& bones, float modelScale, float timeScalar, std::vector<SKBoneKeyframeChain>& output) {
    for (unsigned i = 0; i < input.mNumChannels; ++i) {
        aiNodeAnim* node = input.mChannels[i];

        Bone* targetBone = bones.BoneForName(node->mNodeName.C_Str());

        if (!targetBone) {
            continue;
        }

        for (unsigned keyIndex = 0; keyIndex < node->mNumPositionKeys; ++keyIndex) {
            aiVectorKey* vectorKey = &node->mPositionKeys[keyIndex];

            SKBoneKeyframeChain keyframe;
            keyframe.tick = (unsigned short)(vectorKey->mTime * timeScalar + 0.5f);
            keyframe.next = nullptr;
            keyframe.keyframe.usedAttributes = SKBoneAttrMaskPosition;
            keyframe.keyframe.boneIndex = (unsigned char)targetBone->GetIndex();

            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.x * modelScale));
            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.y * modelScale));
            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.z * modelScale));
            output.push_back(keyframe);
        }

        for (unsigned keyIndex = 0; keyIndex < node->mNumRotationKeys; ++keyIndex) {
            aiQuatKey* quatKey = &node->mRotationKeys[keyIndex];

            SKBoneKeyframeChain keyframe;
            keyframe.tick = (unsigned short)(quatKey->mTime * timeScalar + 0.5f);
            keyframe.next = nullptr;
            keyframe.keyframe.usedAttributes = SKBoneAttrMaskRotation;
            keyframe.keyframe.boneIndex = (unsigned char)targetBone->GetIndex();

            if (quatKey->mValue.w < 0.0f) {
                keyframe.keyframe.attributeData.push_back((short)(-quatKey->mValue.x * std::numeric_limits<short>::max()));
                keyframe.keyframe.attributeData.push_back((short)(-quatKey->mValue.y * std::numeric_limits<short>::max()));
                keyframe.keyframe.attributeData.push_back((short)(-quatKey->mValue.z * std::numeric_limits<short>::max()));
            } else {
                keyframe.keyframe.attributeData.push_back((short)(quatKey->mValue.x * std::numeric_limits<short>::max()));
                keyframe.keyframe.attributeData.push_back((short)(quatKey->mValue.y * std::numeric_limits<short>::max()));
                keyframe.keyframe.attributeData.push_back((short)(quatKey->mValue.z * std::numeric_limits<short>::max()));
            }
            output.push_back(keyframe);
        }

        for (unsigned keyIndex = 0; keyIndex < node->mNumScalingKeys; ++keyIndex) {
            aiVectorKey* vectorKey = &node->mScalingKeys[keyIndex];

            SKBoneKeyframeChain keyframe;
            keyframe.tick = (unsigned short)(vectorKey->mTime * timeScalar + 0.5f);
            keyframe.next = nullptr;
            keyframe.keyframe.usedAttributes = SKBoneAttrMaskScale;
            keyframe.keyframe.boneIndex = (unsigned char)targetBone->GetIndex();

            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.x * 0x100));
            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.y * 0x100));
            keyframe.keyframe.attributeData.push_back((short)(vectorKey->mValue.z * 0x100));
            output.push_back(keyframe);
        }
    }

    std::sort(output.begin(), output.end(), keyframeSortFn);
}

void connectKeyframeChain(std::vector<SKBoneKeyframeChain>& keyframes, std::map<unsigned short, SKBoneKeyframeChain*>& firstKeyframe) {
    for (auto it = keyframes.rbegin(); it != keyframes.rend(); ++it) {
        unsigned short key = keyForKeyframe(it->keyframe);
        auto prev = firstKeyframe.find(key);

        if (prev != firstKeyframe.end()) {
            it->next = prev->second;
        }

        firstKeyframe[key] = &(*it);
    }
}

void combineChunk(std::vector<SKBoneKeyframeChain>& chunkKeyframes, struct SKAnimationChunk& output) {
    std::sort(chunkKeyframes.begin(), chunkKeyframes.end(), keyframeSortFn);

    for (auto it = chunkKeyframes.begin(); it != chunkKeyframes.end(); ++it) {
        if (!output.keyframes.size() || output.keyframes.rbegin()->tick != it->tick) {
            SKAnimationKeyframe newKeyframe;
            newKeyframe.tick = it->tick;
            output.keyframes.push_back(newKeyframe);
        }
        auto targetKeyframe = output.keyframes.rbegin();

        if (!targetKeyframe->bones.size() || targetKeyframe->bones.rbegin()->boneIndex != it->keyframe.boneIndex) {
            SKBoneKeyframe newBone;
            newBone.usedAttributes = 0;
            newBone.boneIndex = it->keyframe.boneIndex;
            targetKeyframe->bones.push_back(newBone);
        }
        auto targetBone = targetKeyframe->bones.rbegin();
        targetBone->usedAttributes |= it->keyframe.usedAttributes;
        targetBone->attributeData.insert(targetBone->attributeData.end(), it->keyframe.attributeData.begin(), it->keyframe.attributeData.end());
    }
}

unsigned buildChunk(std::vector<SKBoneKeyframeChain>& keyframes, unsigned atIndex, unsigned short currentTick, struct SKAnimationChunk& output) {
    std::vector<SKBoneKeyframeChain> nextKeyframes;

    while (atIndex < keyframes.size() && keyframes[atIndex].tick == currentTick) {
        if (keyframes[atIndex].next) {
            nextKeyframes.push_back(*keyframes[atIndex].next);
        }
        ++atIndex;
    }

    combineChunk(nextKeyframes, output);

    return atIndex;
}

void buildInitialState(std::map<unsigned short, SKBoneKeyframeChain*>& firstKeyFrame, struct SKAnimationChunk& output) {
    std::vector<SKBoneKeyframeChain> keyframes;

    for (auto it = firstKeyFrame.begin(); it != firstKeyFrame.end(); ++it) {
        SKBoneKeyframeChain modified = *(it->second);
        modified.tick = 0;
        keyframes.push_back(modified);
    }

    combineChunk(keyframes, output);
}

bool translateAnimationToSK(const aiAnimation& input, struct SKAnimation& output, BoneHierarchy& bones, float modelScale, unsigned short targetTicksPerSecond) {
    float timeScalar = (float)targetTicksPerSecond / (float)1000.0f;

    std::vector<SKBoneKeyframeChain> keyframes;
    populateKeyframes(input, bones, modelScale, timeScalar, keyframes);

    if (keyframes.size() == 0) {
        return false;
    }

    std::map<unsigned short, SKBoneKeyframeChain*> firstKeyFrame;
    connectKeyframeChain(keyframes, firstKeyFrame);

    struct SKAnimationChunk currentChunk;
    currentChunk.nextChunkSize = 0;
    currentChunk.nextChunkTick = 0;

    unsigned currentIndex = 0;

    buildInitialState(firstKeyFrame, currentChunk);

    output.ticksPerSecond = targetTicksPerSecond;
    output.maxTicks = (unsigned short)(input.mDuration * timeScalar);

    while (currentIndex < keyframes.size()) {
        unsigned short tick = keyframes[currentIndex].tick;
        currentIndex = buildChunk(keyframes, currentIndex, tick, currentChunk);

        if (currentIndex < keyframes.size()) {
            currentChunk.nextChunkTick = keyframes[currentIndex].tick;
        } else {
            currentChunk.nextChunkTick = std::numeric_limits<unsigned short>::max();
        }

        if (currentChunk.keyframes.size()) {
            output.chunks.push_back(currentChunk);
        }

        currentChunk.nextChunkSize = 0;
        currentChunk.nextChunkTick = 0;
        currentChunk.keyframes.clear();
    }

    output.chunks.rbegin()->nextChunkTick = std::numeric_limits<unsigned short>::max();

    return true;
}