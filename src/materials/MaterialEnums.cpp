#include "MaterialEnums.h"

const char* gGeometryModeNames[GEOMETRY_MODE_COUNT] = {
    "G_ZBUFFER",
    "G_SHADE",
    "G_TEXTURE_ENABLE",
    "G_SHADING_SMOOTH",
    "G_CULL_FRONT",
    "G_CULL_BACK",
    "G_FOG",
    "G_LIGHTING",
    "G_TEXTURE_GEN",
    "G_TEXTURE_GEN_LINEAR",
    "G_LOD",
    "G_CLIPPING",
};

const char* gCycleTypeNames[(int)CycleType::Count] = {
    "Unknown",
    "G_CYC_1CYCLE",
    "G_CYC_2CYCLE",
    "G_CYC_COPY",
    "G_CYC_FILL",
};

const char* gColorCombineSourceNames[(int)ColorCombineSource::Count] = {
    "COMBINED",
    "TEXEL0",
    "TEXEL1",
    "PRIMITIVE",
    "SHADE",
    "ENVIRONMENT",
    "CENTER",
    "SCALE",
    "COMBINED_ALPHA",
    "TEXEL0_ALPHA",
    "TEXEL1_ALPHA",
    "PRIMITIVE_ALPHA",
    "SHADED_ALPHA",
    "ENVIRONMENT_ALPHA",
    "LOD_FRACTION",
    "PRIM_LOD_FRAC",
    "NOISE",
    "K4",
    "K5",
    "1",
    "0",
};

bool gCanUseColorCombineSource[4][(int)ColorCombineSource::Count] = {
    {true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, true, false, false, true, true},
    {true, true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, true, false, false, true},
    {true, true, true, true, true, true, false, true, true, true, true, true, true, true, true, true, false, false, true, false, true},
    {true, true, true, true, true, true, false, false, false, false, false, false, false, false, false, false, false, false, false, true, true},
};

bool canUseColorCombineSource(int offset, ColorCombineSource source) {
    return gCanUseColorCombineSource[offset][(int)source];
}


extern const char* gAlphaCombineSourceNames[(int)AlphaCombineSource::Count] = {
    "COMBINED",
    "TEXEL0",
    "TEXEL1",
    "PRIMITIVE",
    "SHADE",
    "ENVIRONMENT",
    "LOD_FRACTION",
    "PRIM_LOD_FRAC",
    "1",
    "0",
};

bool gCanUseAlphaCombineSources[4][(int)AlphaCombineSource::Count] = {
    {true, true, true, true, true, true, false, false, true, true},
    {true, true, true, true, true, true, false, false, true, true},
    {false, true, true, true, true, true, true, true, false, true},
    {true, true, true, true, true, true, false, false, true, true},
};

bool canUseAlphaCombineSource(int offset, ColorCombineSource source) {
    return gCanUseAlphaCombineSources[offset][(int)source];
}