#define cimg_display 0
#define cimg_use_png
#define cimg_use_tiff
#include "../../cimg/CImg.h"

#include "TextureDefinition.h"
#include "../FileUtils.h"

#include <iomanip>
#include <algorithm>
#include <iomanip>

DataChunkStream::DataChunkStream() :
    mCurrentBufferPos(0),
    mCurrentBuffer(0) {}

void DataChunkStream::WriteBytes(const char* data, int byteCount) {
    for (int i = 0; i < byteCount; ++i) {
        WriteBits(data[i], 8);
    }
}

void DataChunkStream::WriteBits(int from, int bitCount) {
    if (!bitCount) {
        return;
    } else if (bitCount + mCurrentBufferPos > 64) {
        int firstChunkSize = 64 - mCurrentBufferPos;
        int secondChunkSize = bitCount - firstChunkSize;

        WriteBits(from >> secondChunkSize, firstChunkSize);
        FlushBuffer();
        WriteBits(from, secondChunkSize);
    } else {
        uint64_t mask = ~(~(uint64_t)0 << bitCount);
        mCurrentBuffer |= (from & mask) << (64 - mCurrentBufferPos - bitCount);
        mCurrentBufferPos += bitCount;
    }
}

const std::vector<uint64_t>& DataChunkStream::GetData() {
    FlushBuffer();
    return mData;
}

void DataChunkStream::FlushBuffer() {
    if (mCurrentBufferPos == 0) {
        return;
    }

    mData.push_back(mCurrentBuffer);

    mCurrentBufferPos = 0;
    mCurrentBuffer = 0;
}

PixelRGBAu8::PixelRGBAu8() : r(0), g(0), b(0), a(0) {}
PixelRGBAu8::PixelRGBAu8(uint8_t rVal, uint8_t gVal, uint8_t bVal, uint8_t aVal) : 
    r(rVal), g(gVal), b(bVal), a(aVal) {}


bool PixelRGBAu8::WriteToStream(DataChunkStream& output, G_IM_SIZ size) {
    switch (size) {
        case G_IM_SIZ::G_IM_SIZ_32b:
            output.WriteBytes((const char*)this, sizeof(PixelRGBAu8));
            return true;
        case G_IM_SIZ::G_IM_SIZ_16b:
            output.WriteBits(r >> 3, 5);
            output.WriteBits(g >> 3, 5);
            output.WriteBits(b >> 3, 5);
            output.WriteBits(a >> 7, 1);
            return true;
        default:
            return false;

    }
}

PixelIu8::PixelIu8(uint8_t i) : i(i) {}

bool PixelIu8::WriteToStream(DataChunkStream& output, G_IM_SIZ size) {
    switch (size) {
        case G_IM_SIZ::G_IM_SIZ_8b:
            output.WriteBits(i, 8);
            return true;
        case G_IM_SIZ::G_IM_SIZ_4b:
            output.WriteBits(i >> 4, 4);
            return true;
        default:
            return false;
    }
}

PixelIAu8::PixelIAu8(uint8_t i, uint8_t a) : i(i), a(a) {}

bool PixelIAu8::WriteToStream(DataChunkStream& output, G_IM_SIZ size) {
    switch (size) {
        case G_IM_SIZ::G_IM_SIZ_16b:
            output.WriteBits(i, 8);
            output.WriteBits(a, 8);
            return true;
        case G_IM_SIZ::G_IM_SIZ_8b:
            output.WriteBits(i >> 4, 4);
            output.WriteBits(a >> 4, 4);
            return true;
        case G_IM_SIZ::G_IM_SIZ_4b:
            output.WriteBits(i >> 5, 3);
            output.WriteBits(a >> 7, 1);
            return true;
        default:
            return false;
    }
}

struct PixelRGBAu8 readRGBAPixel(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y) {
    struct PixelRGBAu8 result;

    result.r = 0;
    result.g = 0;
    result.b = 1;
    result.a = 0xFF;
    
    switch (input.spectrum()) {
        case 4:
            result.a = input(x, y, 0, 3);
        case 3:
            result.r = input(x, y, 0, 0);
            result.g = input(x, y, 0, 1);
            result.b = input(x, y, 0, 2);
            break;
        case 2:
            result.a = input(x, y, 0, 1);
        case 1:
            result.r = input(x, y, 0, 0);
            result.g = input(x, y, 0, 0);
            result.b = input(x, y, 0, 0);
            break;
    }
    
    return result;
}

struct PixelIu8 readIPixel(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y) {
    switch (input.spectrum()) {
        case 4:
        case 3:
            return PixelIu8((
                input(x, y, 0, 0) * 85 +
                input(x, y, 0, 1) * 86 +
                input(x, y, 0, 2) * 85
            ) >> 8);
        case 2:
        case 1:
            return PixelIu8(input(x, y, 0, 0));
    }

    return PixelIu8(0);
}

struct PixelIAu8 readIAPixel(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y) {
    uint8_t alpha = 0xFF;

    switch (input.spectrum()) {
        case 4:
            alpha = input(x, y, 0, 3);
        case 3:
            return PixelIAu8((
                input(x, y, 0, 0) * 85 +
                input(x, y, 0, 1) * 86 +
                input(x, y, 0, 2) * 85
            ) >> 8, alpha);
        case 2:
            alpha = input(x, y, 0, 1);
        case 1:
            return PixelIAu8(input(x, y, 0, 0), alpha);
    }

    return PixelIAu8(0, alpha);
}

bool convertPixel(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y, DataChunkStream& output, G_IM_FMT fmt, G_IM_SIZ siz) {
    switch (fmt) {
        case G_IM_FMT::G_IM_FMT_RGBA: {
            PixelRGBAu8 pixel = readRGBAPixel(input, x, y);
            return pixel.WriteToStream(output, siz);
        }
        case G_IM_FMT::G_IM_FMT_I: {
            PixelIu8 pixel = readIPixel(input, x, y);
            return pixel.WriteToStream(output, siz);
        }
        case G_IM_FMT::G_IM_FMT_IA: {
            PixelIAu8 pixel = readIAPixel(input, x, y);
            return pixel.WriteToStream(output, siz);
        }
        default:
            return false;
    }
}

const char* gFormatShortName[] = {
    "rgba",
    "yuv",
    "ci",
    "i",
    "ia",
};

const char* gSizeName[] = {
    "4b",
    "8b",
    "16b",
    "32b",
};

TextureDefinition::TextureDefinition(const std::string& filename, G_IM_FMT fmt, G_IM_SIZ siz) :
    mName(getBaseName(replaceExtension(filename, "")) + "_" + gFormatShortName[(int)fmt] + "_" + gSizeName[(int)siz]),
    mFmt(fmt),
    mSiz(siz) {

    cimg_library_suffixed::CImg<unsigned char> imageData(filename.c_str());

    mWidth = imageData.width();
    mHeight = imageData.height();

    DataChunkStream dataStream;

    for (int y = 0; y < mHeight; ++y) {
        for (int x = 0; x < mWidth; ++x) {
            convertPixel(imageData, x, y, dataStream, fmt, siz);
        }
    }

    auto data = dataStream.GetData();
    mData.resize(data.size());

    std::copy(data.begin(), data.end(), mData.begin());
}

bool isGrayscale(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y) {
    switch (input.spectrum()) {
        case 1:
        case 2:
            return true;
        case 3:
        case 4:
            return input(x, y, 0, 0) == input(x, y, 0, 1) && input(x, y, 0, 1) == input(x, y, 0, 2);
    }

    return false;
}

int colorHash(cimg_library_suffixed::CImg<unsigned char>& input, int x, int y) {
    switch (input.spectrum()) {
        case 1:
        case 2:
            return input(x, y, 0, 0);
        case 3:
        case 4:
            return (input(x, y, 0, 0) << 24) | (input(x, y, 0, 1) << 16) | (input(x, y, 0, 2) << 8);
    }

    return 0;
}

void TextureDefinition::DetermineIdealFormat(const std::string& filename, G_IM_FMT& fmt, G_IM_SIZ& siz) {
    cimg_library_suffixed::CImg<unsigned char> imageData(filename.c_str());

    bool hasColor = false;
    bool hasFullTransparency = false;
    bool hasPartialTransparency = false;
    std::set<int> colorCount;

    for (int y = 0; y < imageData.height(); ++y) {
        for (int x = 0; x < imageData.width(); ++x) {
            colorCount.insert(colorHash(imageData, x, y));
            bool isPixelGrayscale = isGrayscale(imageData, x, y);
            hasColor = hasColor || !isPixelGrayscale;
            unsigned char alpha = imageData.spectrum() == 4 ? imageData(x, y, 0, 3) : 0xFF;

            hasPartialTransparency = hasPartialTransparency || (alpha != 0 && alpha != 0xFF);
            hasFullTransparency = hasFullTransparency || alpha == 0;
        }
    }

    if (hasColor) {
        if (hasPartialTransparency) {
            fmt = G_IM_FMT::G_IM_FMT_RGBA;
            siz = G_IM_SIZ::G_IM_SIZ_32b;
        } else {
            fmt = G_IM_FMT::G_IM_FMT_RGBA;
            siz = G_IM_SIZ::G_IM_SIZ_16b;
        }
    } else {
        if (hasPartialTransparency || hasFullTransparency) {
            fmt = G_IM_FMT::G_IM_FMT_IA;
            siz = G_IM_SIZ::G_IM_SIZ_16b;
        } else {
            fmt = G_IM_FMT::G_IM_FMT_I;
            siz = G_IM_SIZ::G_IM_SIZ_8b;
        }
    }
}

std::unique_ptr<FileDefinition> TextureDefinition::GenerateDefinition(const std::string& name, const std::string& location) const {
    std::unique_ptr<StructureDataChunk> dataChunk(new StructureDataChunk());

    int line;
    int index = 0;

    GetLine(line);

    for (int y = 0; y < mHeight; ++y) {
        std::ostringstream stream;

        for (int lineIndex = 0; lineIndex < line; ++lineIndex) {
            uint64_t data = mData[index];

            if (lineIndex != 0) {
                stream << ", ";
            }

            stream << "0x" << std::hex << std::setw(16) << std::setfill('0') << data;

            ++index;
        }

        dataChunk->AddPrimitive(stream.str());
    }

    return std::unique_ptr<FileDefinition>(new DataFileDefinition("u64", name, true, location, std::move(dataChunk)));
}

int TextureDefinition::Width() const {
    return mWidth;
}

int TextureDefinition::Height() const {
    return mHeight;
}

G_IM_FMT TextureDefinition::Format() const {
    return mFmt;
}

G_IM_SIZ TextureDefinition::Size() const {
    return mSiz;
}

bool TextureDefinition::GetLine(int& line) const {
    int bitLine = bitSizeforSiz(mSiz) * mWidth;
    line = bitLine / 64;

    if (bitLine % 64 == 0) {
        return true;
    }

    return false;
}

const std::string& TextureDefinition::Name() const {
    return mName;
}