#ifndef __TEXTURE_DEFINITION_H__
#define __TEXTURE_DEFINITION_H__

#include <vector>
#include <inttypes.h>

#include "TextureFormats.h"

#include "../definitions/DataChunk.h"
#include "../definitions/FileDefinition.h"

class DataChunkStream {
public:
    DataChunkStream();
    void WriteBytes(const char* data, int byteCount);
    void WriteBits(int from, int bitCount);

    const std::vector<uint64_t>& GetData();
private:
    void FlushBuffer();

    int mCurrentBufferPos;
    uint64_t mCurrentBuffer;
    std::vector<uint64_t> mData;
};

struct PixelRGBAu8 {
    PixelRGBAu8();
    PixelRGBAu8(uint8_t rVal, uint8_t gVal, uint8_t bVal, uint8_t aVal);
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;

    bool WriteToStream(DataChunkStream& output, G_IM_SIZ size);
};

struct PixelIu8 {
    PixelIu8(uint8_t i);
    uint8_t i;

    bool WriteToStream(DataChunkStream& output, G_IM_SIZ size);
};

struct PixelIAu8 {
    PixelIAu8(uint8_t i, uint8_t a);
    uint8_t i;
    uint8_t a;

    bool WriteToStream(DataChunkStream& output, G_IM_SIZ size);
};

class TextureDefinition {
public:
    TextureDefinition(const std::string& filename, G_IM_FMT fmt, G_IM_SIZ siz);

    static void DetermineIdealFormat(const std::string& filename, G_IM_FMT& fmt, G_IM_SIZ& siz);

    std::unique_ptr<FileDefinition> GenerateDefinition(const std::string& name, const std::string& location) const;

    int Width() const;
    int Height() const;

    const std::string& Name() const;
private:
    std::string mName;
    G_IM_FMT mFmt;
    G_IM_SIZ mSiz;
    int mWidth;
    int mHeight;
    std::vector<unsigned long long> mData;
};

#endif