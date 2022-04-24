#include "TextureFormats.h"

const char* G_IM_FMT_NAMES[] = {
    "G_IM_FMT_RGBA",
    "G_IM_FMT_YUV",
    "G_IM_FMT_CI",
    "G_IM_FMT_I",
    "G_IM_FMT_IA",
};

const char* nameForImageFormat(G_IM_FMT format) {
    return G_IM_FMT_NAMES[(int)format];
}

const char* G_IM_SIZ_NAMES[] = {
    "G_IM_SIZ_4b",
    "G_IM_SIZ_8b",
    "G_IM_SIZ_16b",
    "G_IM_SIZ_32b",
};

const char* nameForImageSize(G_IM_SIZ size) {
    return G_IM_SIZ_NAMES[(int)size];
}