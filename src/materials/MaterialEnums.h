#ifndef __MATERIAL_ENUMS_H__
#define __MATERIAL_ENUMS_H__

enum class GeometryMode {
    None = 0,
    G_ZBUFFER = (1 << 0),
    G_SHADE = (1 << 1),
    G_TEXTURE_ENABLE = (1 << 2),
    G_SHADING_SMOOTH = (1 << 3),
    G_CULL_FRONT = (1 << 4),
    G_CULL_BACK = (1 << 5),
    G_FOG = (1 << 6),
    G_LIGHTING = (1 << 7),
    G_TEXTURE_GEN = (1 << 8),
    G_TEXTURE_GEN_LINEAR = (1 << 9),
    G_LOD = (1 << 10),
    G_CLIPPING = (1 << 11),
};

extern const char* gGeometryModeNames[];

#define GEOMETRY_MODE_COUNT 12

enum class CycleType {
    Unknown,
    _1Cycle,
    _2Cycle,
    Copy,
    Fill,
    Count,
};

extern const char* gCycleTypeNames[];

enum class ColorCombineSource {
    Combined,
    Texel0,
    Texel1,
    PrimitiveColor,
    ShadeColor,
    EnvironmentColor,
    KeyCenter,
    KeyScale,
    CombinedAlpha,
    Texture0Alpha,
    Texture1Alpha,
    PrimitiveAlpha,
    ShadedAlpha,
    EnvironmentAlpha,
    LODFraction,
    PrimitiveLODFraction,
    Noise,
    ConvertK4,
    ConvertK5,
    _1,
    _0,
    Count,
};

extern const char* gColorCombineSourceNames[];

bool canUseColorCombineSource(int offset, ColorCombineSource source);

enum class AlphaCombineSource {
    CombinedAlpha,
    Texture0Alpha,
    Texture1Alpha,
    PrimitiveAlpha,
    ShadedAlpha,
    EnvironmentAlpha,
    LODFraction,
    PrimitiveLODFraction,
    _1,
    _0,
    Count,
};

extern const char* gAlphaCombineSourceNames[];

bool canUseAlphaCombineSource(int offset, AlphaCombineSource source);

#endif