#include "MaterialState.h"

#include "MaterialEnums.h"

#include "RenderMode.h"

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
    a(ColorCombineSource::_0),
    b(ColorCombineSource::_0),
    c(ColorCombineSource::_0),
    d(ColorCombineSource::_0),
    aAlpha(AlphaCombineSource::_0),
    bAlpha(AlphaCombineSource::_0),
    cAlpha(AlphaCombineSource::_0),
    dAlpha(AlphaCombineSource::_0) {}


bool ColorCombineMode::operator==(const ColorCombineMode& other) const {
    return a == other.a &&
        b == other.b &&
        c == other.c &&
        d == other.d &&
        aAlpha == other.aAlpha &&
        bAlpha == other.bAlpha &&
        cAlpha == other.cAlpha &&
        dAlpha == other.dAlpha;
}

RenderModeState::RenderModeState() {
    
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
    cycleType(CycleType::Unknown) {}

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

    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle1Combine.a]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle1Combine.b]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle1Combine.c]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle1Combine.d]);

    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle1Combine.aAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle1Combine.bAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle1Combine.cAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle1Combine.dAlpha]);

    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle2Combine.a]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle2Combine.b]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle2Combine.c]);
    result->AddPrimitive(gColorCombineSourceNames[(int)to.cycle2Combine.d]);

    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle2Combine.aAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle2Combine.bAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle2Combine.cAlpha]);
    result->AddPrimitive(gAlphaCombineSourceNames[(int)to.cycle2Combine.dAlpha]);

    return result;
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
        // TODO
    }

    if (secondName.length()) {
        result->AddPrimitive(secondName + "2");
    } else {
        // TODO
    }

    return result;
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
}