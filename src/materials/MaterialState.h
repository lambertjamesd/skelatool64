#ifndef __MATERIAL_STATE_H__
#define __MATERIAL_STATE_H__

#include <inttypes.h>
#include "TextureDefinition.h"
#include "MaterialEnums.h"

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

struct MaterialState {
public:
    MaterialState();
    // state to keep track of
    // tiles
    // RDP tile cache
    struct TileState tiles[8];
    // geometry modes
    FlagList geometryModes;
    // cycle type
    CycleType cycleType;
    // render mode
    // combine mode
    // RDP colors
};

std::unique_ptr<DataChunk> generateGeometryModes(const MaterialState& from, const MaterialState& to);

void generateMaterial(const MaterialState& from, const MaterialState& to, StructureDataChunk& output);

#endif