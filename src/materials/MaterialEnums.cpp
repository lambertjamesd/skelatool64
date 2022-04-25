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