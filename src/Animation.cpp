
#include "Animation.h"
#include <climits>
#include <algorithm>
#include <iomanip>

#define ALIGN_TO_16(input)    (((input) + 15) & ~15)
#define NO_CHUNK              0

SKBoneKeyframe::SKBoneKeyframe():
    boneIndex(0),
    usedAttributes(0) {

}

SKAnimationKeyframe::SKAnimationKeyframe(): tick(0) {}

SKAnimationChunk::SKAnimationChunk(): nextChunkSize(0), nextChunkTick(0) {}

unsigned short calculateChunkSize(SKAnimationChunk& chunk) {
    unsigned short result = 6;

    for (auto keyframe = chunk.keyframes.begin(); keyframe != chunk.keyframes.end(); ++keyframe) {
        result += 4;

        for (auto bone = keyframe->bones.begin(); bone != keyframe->bones.end(); ++bone) {
            result += 2 + 2 * bone->attributeData.size();
        }
    }

    return result;
}

void generateAnimationChunk(SKAnimationChunk& chunk, std::vector<unsigned short>& output) {
    output.push_back(chunk.nextChunkSize);
    output.push_back(chunk.nextChunkTick);
    output.push_back(chunk.keyframes.size());

    for (auto keyframe = chunk.keyframes.begin(); keyframe != chunk.keyframes.end(); ++keyframe) {
        output.push_back(keyframe->tick);
        output.push_back(keyframe->bones.size());

        for (auto bone = keyframe->bones.begin(); bone != keyframe->bones.end(); ++bone) {
            output.push_back(((unsigned short)bone->boneIndex << 8) | bone->usedAttributes);

            output.insert(output.end(), bone->attributeData.begin(), bone->attributeData.end());
        }
    }
}

unsigned short generateAnimationData(std::vector<SKAnimationChunk>& chunks, std::vector<unsigned short>& output) {
    unsigned short prevChunkSize = NO_CHUNK;

    for (auto it = chunks.rbegin(); it != chunks.rend(); ++it) {
        it->nextChunkSize = prevChunkSize;
        prevChunkSize = ALIGN_TO_16(calculateChunkSize(*it));
    }

    for (auto it = chunks.begin(); it != chunks.end(); ++it) {
        generateAnimationChunk(*it, output);

        // add padding
        while (output.size() * 2 < ALIGN_TO_16(output.size() * 2)) {
            output.push_back(0);
        }
    }

    return prevChunkSize;
}

#define MAX_COLS    16

unsigned short formatAnimationChunks(const std::string& name, std::vector<SKAnimationChunk>& chunks, std::ostream& output) {
    std::vector<unsigned short> data;

    unsigned short firstChunkSize = generateAnimationData(chunks, data);

    output << "u16 " << name << "[] = {";

    for (unsigned i = 0; i < data.size(); ++i) {
        if (i % MAX_COLS == 0) {
            output << std::endl << "    ";
        }

        output << "0x" << std::setw(4) << std::setfill('0') << std::hex << data[i] << ", ";
    }

    output << std::endl << "};" << std::endl;

    return firstChunkSize;
}