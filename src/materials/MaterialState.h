#ifndef __MATERIAL_STATE_H__
#define __MATERIAL_STATE_H__

#include <inttypes.h>
#include "TextureDefinition.h"
#include "MaterialEnums.h"

struct Coloru8 {
    u_int8_t r;
    u_int8_t g;
    u_int8_t b;
    u_int8_t a;
};

struct FlagList {
    FlagList();
    uint64_t flags;
    uint64_t knownFlags;

    void SetFlag(int mask, bool value);
    void DeleteFlag(int mask);

    struct FlagList GetDeltaFrom(struct FlagList& other);
};

struct TextureCoordinateState {
public:
    TextureCoordinateState();
    bool wrap;
    bool mirror;
    int mask;
    int shift;
    int offset;
    int lowerBound;
};

struct TileState {
public:
    TileState();
    bool isOn;
    std::shared_ptr<TextureDefinition> texture;
    // 1 line is a 64 bit offset in TMEM
    int line;
    int pallete;
    struct TextureCoordinateState uCoord;
    struct TextureCoordinateState vCoord;
};


struct ColorCombineMode {
    ColorCombineMode();

    bool operator==(const ColorCombineMode& other) const;

    ColorCombineSource a;
    ColorCombineSource b;
    ColorCombineSource c;
    ColorCombineSource d;

    AlphaCombineSource aAlpha;
    AlphaCombineSource bAlpha;
    AlphaCombineSource cAlpha;
    AlphaCombineSource dAlpha;
};

struct RenderModeState {
public:
    RenderModeState();
    bool aaEnable;
    bool zCompare;
    bool zUpdate;
    bool imageRead;
    bool clearOnCvg;
    CVG_DST cvgDst;
    ZMODE zMode;
    bool cvgXAlpha;
    bool alphaCvgSel;
    bool forceBL;

    BlendSource blendSource0;
    BlendSource blendSource1;
    AlphaBlendSource alphaSource0;
    AlphaBlendSource alphaSource1;
};

struct MaterialState {
public:
    MaterialState();
    // state to keep track of
    // tiles
    // RDP tile cache
    struct TileState tiles[8];
    // geometry modes
    FlagList geometryModes;

    // G_SETOTHERMODE_H
    PipelineMode pipelineMode;
    CycleType cycleType;
    PerspectiveMode perspectiveMode;
    TextureDetail textureDetail;
    TextureLOD textureLOD;
    TextureLUT textureLUT;
    TextureFilter textureFilter;
    TextureConvert textureConvert;
    CombineKey combineKey;
    ColorDither colorDither;
    AlphaDither alphaDither;
    // G_SETOTHERMODE_L
    AlphaCompare alphaCompare;
    DepthSource depthSource;

    // combine mode
    bool hasCombineMode;
    ColorCombineMode cycle1Combine;
    ColorCombineMode cycle2Combine;

    bool hasRenderMode;
    RenderModeState cycle1RenderMode;
    RenderModeState cycle2RenderMode;

    // RDP colors
    bool usePrimitiveColor;
    Coloru8 primitiveColor;
    uint8_t primitiveM;
    uint8_t primitiveL;

    bool useEnvColor;
    Coloru8 envColor;

    bool useFillColor;
    Coloru8 fillColor;
    
    bool useFogColor;
    Coloru8 fogColor;

    bool useBlendColor;
    Coloru8 blendColor;
};

void generateMaterial(const MaterialState& from, const MaterialState& to, StructureDataChunk& output);

#endif