#include "MaterialState.h"

#include "MaterialEnums.h"

#include "RenderMode.h"

Coloru8::Coloru8() : r(0), g(0), b(0), a(255) {}

bool Coloru8::operator==(const Coloru8& other) const {
    return r == other.r && g == other.g && b == other.b && a == other.a;
}

FlagList::FlagList() : flags(0), knownFlags(0) {}

void FlagList::SetFlag(int mask, bool value) {
    knownFlags |= mask;

    if (value) {
        flags |= mask;
    } else {
        flags &= ~mask;
    }
}

void FlagList::DeleteFlag(int mask) {
    flags &= ~mask;
    knownFlags &= ~mask;
}

struct FlagList FlagList::GetDeltaFrom(struct FlagList& other) {
    struct FlagList result;

    result.knownFlags = 
        // flags known by both with different values
        (knownFlags & other.knownFlags & (flags ^ other.flags)) | 
        // flags known by this but not that
        (knownFlags & ~other.knownFlags);
    // mask by knownFlags so unknown data is set
    // to 0 to avoid confusion
    result.flags = flags & result.knownFlags;

    return result;
}

TextureCoordinateState::TextureCoordinateState():
    wrap(true),
    mirror(false),
    mask(0),
    shift(0),
    offset(0),
    lowerBound(0) {

}

TileState::TileState():
    isOn(false),
    line(0),
    pallete(0) {

}

ColorCombineMode::ColorCombineMode() : 
    color{ColorCombineSource::_0, ColorCombineSource::_0, ColorCombineSource::_0, ColorCombineSource::_0},
    alpha{AlphaCombineSource::_0, AlphaCombineSource::_0, AlphaCombineSource::_0, AlphaCombineSource::_0} {}


bool ColorCombineMode::operator==(const ColorCombineMode& other) const {
    return color[0] == other.color[0] &&
        color[1] == other.color[1] &&
        color[2] == other.color[2] &&
        color[3] == other.color[3] &&
        alpha[0] == other.alpha[0] &&
        alpha[1] == other.alpha[1] &&
        alpha[2] == other.alpha[2] &&
        alpha[3] == other.alpha[3];
}

RenderModeState::RenderModeState() : data(G_RM_OPA_SURF) {
    
}

RenderModeState::RenderModeState(int data) : data(data) {};

bool RenderModeState::operator==(const RenderModeState& other) const {
    return data == other.data;
}

#define DEFINE_RENDER_MODE_ENTRY(name)  std::make_pair(std::string(#name), RenderModeState(name))

std::pair<std::string, RenderModeState> gRenderModes[] = {
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_RA_ZB_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_XLU_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_OPA_DECAL),
    DEFINE_RENDER_MODE_ENTRY(G_RM_RA_ZB_OPA_DECAL),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_XLU_DECAL),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_OPA_INTER),
    DEFINE_RENDER_MODE_ENTRY(G_RM_RA_ZB_OPA_INTER),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_XLU_INTER),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_XLU_LINE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_DEC_LINE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_TEX_EDGE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_TEX_INTER),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_SUB_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_PCL_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_OPA_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_TEX_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_ZB_SUB_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_RA_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_XLU_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_XLU_LINE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_DEC_LINE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_TEX_EDGE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_SUB_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_PCL_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_OPA_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_TEX_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_AA_SUB_TERR),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_XLU_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_OPA_DECAL),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_XLU_DECAL),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_CLD_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_OVL_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ZB_PCL_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_OPA_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_XLU_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_TEX_EDGE),
    DEFINE_RENDER_MODE_ENTRY(G_RM_CLD_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_PCL_SURF),
    DEFINE_RENDER_MODE_ENTRY(G_RM_ADD),
    DEFINE_RENDER_MODE_ENTRY(G_RM_NOOP),
    DEFINE_RENDER_MODE_ENTRY(G_RM_VISCVG),
    DEFINE_RENDER_MODE_ENTRY(G_RM_OPA_CI),
    DEFINE_RENDER_MODE_ENTRY(G_RM_FOG_SHADE_A),
    DEFINE_RENDER_MODE_ENTRY(G_RM_FOG_PRIM_A),
    DEFINE_RENDER_MODE_ENTRY(G_RM_PASS),
};


bool findRenderModeByName(const std::string& name, RenderModeState& output) {
    for (auto pair : gRenderModes) {
        if (pair.first == name) {
            output = pair.second;
            return true;
        }
    }

    return false;
}

MaterialState::MaterialState() :
    pipelineMode(PipelineMode::Unknown),
    cycleType(CycleType::Unknown),
    perspectiveMode(PerspectiveMode::Unknown),
    textureDetail(TextureDetail::Unknown),
    textureLOD(TextureLOD::Unknown),
    textureLUT(TextureLUT::Unknown),
    textureFilter(TextureFilter::Unknown),
    textureConvert(TextureConvert::Unknown),
    combineKey(CombineKey::Unknown),
    colorDither(ColorDither::Unknown),
    alphaDither(AlphaDither::Unknown),
    alphaCompare(AlphaCompare::Unknown),
    depthSource(DepthSource::Unknown),
    hasCombineMode(false),
    hasRenderMode(false),
    usePrimitiveColor(false),
    primitiveM(255),
    primitiveL(255),
    useEnvColor(false),
    useFillColor(false),
    useFogColor(false),
    useBlendColor(false)
     {}

void appendToFlags(std::ostringstream& flags, const std::string& value) {
    if (flags.tellp() != 0) {
        flags << " | ";
    }
    flags << value;
}

std::unique_ptr<DataChunk> generateGeometryModes(const MaterialState& from, const MaterialState& to) {
    std::ostringstream clearFlags;
    std::ostringstream setFlags;

    for (int i = 0; i < GEOMETRY_MODE_COUNT; ++i) {
        int mask = 1 << i;

        bool isKnownToTarget = (to.geometryModes.knownFlags & mask) != 0;
        bool isKnownToSource = (from.geometryModes.knownFlags & mask) != 0;
        bool targetMatchesSource = (mask & (to.geometryModes.flags ^ from.geometryModes.flags)) == 0;

        if (isKnownToTarget && (!isKnownToSource || !targetMatchesSource)) {
            if (to.geometryModes.flags & mask) {
                appendToFlags(setFlags, gGeometryModeNames[i]);
            } else {
                appendToFlags(clearFlags, gGeometryModeNames[i]);
            }
        }
    }

    if (clearFlags.tellp() == 0 && setFlags.tellp() == 0) {
        return NULL;
    }

    if (clearFlags.tellp() == 0) {
        clearFlags << "0";
    }

    if (setFlags.tellp() == 0) {
        setFlags << "0";
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk("gsSPGeometryMode"));

    result->AddPrimitive(clearFlags.str());
    result->AddPrimitive(setFlags.str());

    return result;
}

void generateEnumMacro(int from, int to, const char* macroName, const char** options, StructureDataChunk& output) {
    if (from == to || to == 0) {
        return;
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk(macroName));

    result->AddPrimitive(options[to]);

    output.Add(std::move(result));
}

std::unique_ptr<DataChunk> generateCombineMode(const MaterialState& from, const MaterialState& to) {
    if (!to.hasCombineMode || 
        (from.hasCombineMode && from.cycle1Combine == to.cycle1Combine && from.cycle2Combine  == to.cycle2Combine)) {
        return NULL;
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk("gsDPSetCombineLERP"));

    for (int i = 0; i < 4; ++i) {
        result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle1Combine.color[i]]);
    }

    for (int i = 0; i < 4; ++i) {
        result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle1Combine.alpha[i]]);
    }

    for (int i = 0; i < 4; ++i) {
        result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle2Combine.color[i]]);
    }

    for (int i = 0; i < 4; ++i) {
        result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle2Combine.alpha[i]]);
    }

    return result;
}

std::string generateSingleRenderMode(int renderMode, int cycleNumber) {
    std::ostringstream result;

    std::vector<std::string> flags;
    renderModeExtractFlags(renderMode, flags);

    for (auto& flag : flags) {
        if (result.tellp()) {
            result << " | ";
        }
        result << flag;
    }

    result << "GBL_c" << cycleNumber << "(";

    result << renderModeGetBlendModeName(renderMode, 0) << ", ";
    result << renderModeGetBlendModeName(renderMode, 1) << ", ";
    result << renderModeGetBlendModeName(renderMode, 2) << ", ";
    result << renderModeGetBlendModeName(renderMode, 3) << ")";

    return result.str();
}

std::unique_ptr<DataChunk> generateRenderMode(const MaterialState& from, const MaterialState& to) {
    if (!to.hasRenderMode ||
        (from.hasRenderMode && from.cycle1RenderMode == to.cycle1RenderMode && from.cycle2RenderMode == to.cycle2RenderMode)) {
        return NULL;
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk("gsDPSetRenderMode"));

    std::string firstName;
    std::string secondName;

    for (auto pair : gRenderModes) {
        if (pair.second == to.cycle1RenderMode) {
            firstName = pair.first;
        }

        if (pair.second == to.cycle2RenderMode) {
            secondName = pair.first;
        }
    }

    if (firstName.length()) {
        result->AddPrimitive(firstName);
    } else {
        result->AddPrimitive(generateSingleRenderMode(to.cycle1RenderMode.data, 1));
    }

    if (secondName.length()) {
        result->AddPrimitive(secondName + "2");
    } else {
        result->AddPrimitive(generateSingleRenderMode(to.cycle1RenderMode.data, 2));
    }

    return result;
}

void generatePrimitiveColor(const MaterialState& from, const MaterialState& to, StructureDataChunk& output) {
    if (!to.usePrimitiveColor ||
        (from.usePrimitiveColor && from.primitiveColor == to.primitiveColor && from.primitiveL == to.primitiveL && from.primitiveM == to.primitiveM)) {
        return;   
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk("gsDPSetPrimColor"));

    result->AddPrimitive(to.primitiveM);
    result->AddPrimitive(to.primitiveL);
    result->AddPrimitive(to.primitiveColor.r);
    result->AddPrimitive(to.primitiveColor.g);
    result->AddPrimitive(to.primitiveColor.b);
    result->AddPrimitive(to.primitiveColor.a);

    output.Add(std::move(result));
}

void generateColor(const Coloru8& to, const char* macroName, StructureDataChunk& output) {
    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk(macroName));

    result->AddPrimitive(to.r);
    result->AddPrimitive(to.g);
    result->AddPrimitive(to.b);
    result->AddPrimitive(to.a);

    output.Add(std::move(result));
}

void generateMaterial(const MaterialState& from, const MaterialState& to, StructureDataChunk& output) {
    output.Add(std::unique_ptr<DataChunk>(new MacroDataChunk("gsDPPipeSync")));

    generateEnumMacro((int)from.pipelineMode, (int)to.pipelineMode, "gsDPPipelineMode", gPipelineModeNames, output);
    generateEnumMacro((int)from.cycleType, (int)to.cycleType, "gsDPSetCycleType", gCycleTypeNames, output);
    generateEnumMacro((int)from.perspectiveMode, (int)to.perspectiveMode, "gsDPSetTexturePersp", gPerspectiveModeNames, output);
    generateEnumMacro((int)from.textureDetail, (int)to.textureDetail, "gsDPSetTextureDetail", gTextureDetailNames, output);
    generateEnumMacro((int)from.textureLOD, (int)to.textureLOD, "gsDPSetTextureLOD", gTextureLODNames, output);
    generateEnumMacro((int)from.textureLUT, (int)to.textureLUT, "gsDPSetTextureLUT", gTextureLUTNames, output);
    generateEnumMacro((int)from.textureFilter, (int)to.textureFilter, "gsDPSetTextureFilter", gTextureFilterNames, output);
    generateEnumMacro((int)from.textureConvert, (int)to.textureConvert, "gsDPSetTextureConvert", gTextureConvertNames, output);
    generateEnumMacro((int)from.combineKey, (int)to.combineKey, "gsDPSetCombineKey", gCombineKeyNames, output);
    generateEnumMacro((int)from.colorDither, (int)to.colorDither, "gsDPSetColorDither", gCotherDitherNames, output);
    generateEnumMacro((int)from.alphaDither, (int)to.alphaDither, "gsDPSetAlphaDither", gAlphaDitherNames, output);
    generateEnumMacro((int)from.alphaCompare, (int)to.alphaCompare, "gsDPSetAlphaCompare", gAlphaCompareNames, output);
    generateEnumMacro((int)from.depthSource, (int)to.depthSource, "gsDPSetDepthSource", gDepthSourceNames, output);

    std::unique_ptr<DataChunk> geometryModes = std::move(generateGeometryModes(from, to));
    if (geometryModes) {
        output.Add(std::move(geometryModes));
    }

    std::unique_ptr<DataChunk> combineMode = std::move(generateCombineMode(from, to));
    if (combineMode) {
        output.Add(std::move(combineMode));
    }

    std::unique_ptr<DataChunk> renderMode = std::move(generateRenderMode(from, to));
    if (renderMode) {
        output.Add(std::move(renderMode));
    }

    generatePrimitiveColor(from, to, output);

    if (to.useEnvColor && (!from.useEnvColor || !(to.envColor == from.envColor))) {
        generateColor(to.envColor, "gsDPSetEnvColor", output);
    }

    if (to.useFogColor && (!from.useFogColor || !(to.fogColor == from.fogColor))) {
        generateColor(to.fogColor, "gsDPSetFogColor", output);
    }

    if (to.useBlendColor && (!from.useBlendColor || !(to.blendColor == from.blendColor))) {
        generateColor(to.blendColor, "gsDPSetBlendColor", output);
    }

    // TODO fill color
}