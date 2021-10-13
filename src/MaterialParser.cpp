
#include "MaterialParser.h"

#include "yaml-cpp/yaml.h"
#include <algorithm>
#include <string.h>
#include <sstream>

ParseError::ParseError(const std::string& message) :
    mMessage(message) {
    
}

std::string formatError(const std::string& message, const YAML::Mark& mark) {
    std::stringstream output;
    output << "error at line " << mark.line + 1 << ", column "
           << mark.column + 1 << ": " << message;
    return output.str();
}

enum class RenderModeIndex {
    Index1 = 0x1,
    Index2 = 0x2,
};

struct RenderModePossibility {
    const char* mName;
    unsigned mIndices;

    RenderModePossibility(const char* name): mName(name) {}
    RenderModePossibility(const char* name, unsigned indices): mName(name), mIndices(indices) {}

    bool operator == (const RenderModePossibility& other) const {
        return strcmp(mName, other.mName) == 0;
    }
};

struct RenderModePossibility gRenderModePossibilities[] = {
    RenderModePossibility("G_RM_AA_ZB_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_XLU_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_OPA_DECAL", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_XLU_DECAL", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_OPA_INTER", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_XLU_INTER", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_XLU_LINE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_DEC_LINE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_TEX_EDGE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_TEX_INTER", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_SUB_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_PCL_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_OPA_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_TEX_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_ZB_SUB_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_RA_ZB_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_RA_ZB_OPA_DECAL", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_RA_ZB_OPA_INTER", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_AA_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_XLU_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_XLU_LINE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_DEC_LINE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_TEX_EDGE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_SUB_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_PCL_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_OPA_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_TEX_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_AA_SUB_TERR", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_RA_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_ZB_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_XLU_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_OPA_DECAL", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_XLU_DECAL", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_CLD_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_OVL_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ZB_PCL_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_OPA_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_XLU_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_CLD_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_TEX_EDGE", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_PCL_SURF", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_ADD", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_NOOP", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_VISCVG", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),
    RenderModePossibility("G_RM_OPA_CI", (unsigned)RenderModeIndex::Index1 | (unsigned)RenderModeIndex::Index2),

    RenderModePossibility("G_RM_FOG_SHADE_A", (unsigned)RenderModeIndex::Index1),
    RenderModePossibility("G_RM_FOG_PRIM_A", (unsigned)RenderModeIndex::Index1),
    RenderModePossibility("G_RM_PASS", (unsigned)RenderModeIndex::Index1),
};

void parseRenderMode(const YAML::Node& node, RenderMode& renderMode, ParseResult& output) {
    if (node.IsNull()) {
        return;
    }

    if (!node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("Render mode should be a string", node.Mark())));
        return;
    }

    RenderModePossibility* arrayEnd = &gRenderModePossibilities[sizeof(gRenderModePossibilities)/sizeof(*gRenderModePossibilities)];

    std::string asString = node.as<std::string>();
    
    auto namedRenderMode = std::find(
        &gRenderModePossibilities[0], 
        arrayEnd, 
        RenderModePossibility(asString.c_str())
    );

    if (namedRenderMode == arrayEnd) {
        output.mErrors.push_back(ParseError(formatError("Invalid render mode", node.Mark())));
        return;
    }

    renderMode.mRenderMode1 = arrayEnd->mName;
    renderMode.mRenderMode2 = arrayEnd->mName;
}

void parseMaterial(const YAML::Node& node, Material& material, ParseResult& output) {
    parseRenderMode(node["RenderMode"], material.mRenderMode, output);
}

void parseMaterialFile(std::istream& input, ParseResult& output) {
    try {
        YAML::Node doc = YAML::Load(input);

        const YAML::Node& materials = doc["Materials"];

        for (auto it = materials.begin(); it != materials.end(); ++it) {
            Material newMaterial;
            parseMaterial(it->second, newMaterial, output);
            output.mMaterialFile.mMaterials[it->first.as<std::string>()] = newMaterial;
        }
    } catch (YAML::ParserException& e) {
        output.mErrors.push_back(ParseError(e.what()));
    }
}