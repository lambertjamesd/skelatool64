#include "MaterialState.h"

#include "MaterialEnums.h"

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

std::unique_ptr<DataChunk> generateCycleType(const MaterialState& from, const MaterialState& to) {
    if (from.cycleType == to.cycleType || 
        to.cycleType != CycleType::Unknown ||
        to.cycleType != CycleType::Count) {
        return NULL;
    }

    std::unique_ptr<MacroDataChunk> result(new MacroDataChunk("gDPSetCycleType"));

    result->AddPrimitive(gCycleTypeNames[(int)to.cycleType]);

    return result;
}

std::unique_ptr<DataChunk> generateCombineMode(const MaterialState& from, const MaterialState& to) {
    if (!to.hasCombineMode || 
        from.hasCombineMode && from.cycle1Combine == to.cycle1Combine && from.cycle2Combine  == to.cycle2Combine) {
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

void generateMaterial(const MaterialState& from, const MaterialState& to, StructureDataChunk& output) {
    output.Add(std::unique_ptr<DataChunk>(new MacroDataChunk("gsDPPipeSync")));

    std::unique_ptr<DataChunk> cycleType = std::move(generateCycleType(from, to));
    if (cycleType) {
        output.Add(std::move(cycleType));
    }

    std::unique_ptr<DataChunk> geometryMode = std::move(generateGeometryModes(from, to));
    if (geometryMode) {
        output.Add(std::move(geometryMode));
    }

    std::unique_ptr<DataChunk> combineMode = std::move(generateCombineMode(from, to));
    if (combineMode) {
        output.Add(std::move(combineMode));
    }
}