
#include "MaterialParser.h"

#include "yaml-cpp/yaml.h"
#include <algorithm>
#include <string.h>
#include <sstream>
#include <string>
#include <stdexcept>
#include <map>

#include "./TextureCache.h"
#include "../FileUtils.h"

TextureCache gTextureCache;

ParseError::ParseError(const std::string& message) :
    mMessage(message) {
    
}

std::string formatError(const std::string& message, const YAML::Mark& mark) {
    std::stringstream output;
    output << "error at line " << mark.line + 1 << ", column "
           << mark.column + 1 << ": " << message;
    return output.str();
}

int parseInteger(const YAML::Node& node, ParseResult& output, int min, int max) {
    if (!node.IsDefined() || !node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("Expected a number", node.Mark())));
        return 0;
    }
    
    int result = 0;

    try {
        result = std::atoi(node.Scalar().c_str());
    } catch (std::invalid_argument const& err) {
        output.mErrors.push_back(ParseError(formatError("Expected a number", node.Mark())));
        return 0;
    }

    if (result < min || result > max) {
        std::stringstream errorMessage;
        errorMessage << "Expected a number between " << min << " and " << max;
        output.mErrors.push_back(ParseError(formatError(errorMessage.str(), node.Mark())));
        return result;
    }


    return result;
}

std::string parseString(const YAML::Node& node, ParseResult& output) {
    if (!node.IsDefined() || !node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("Expected a string", node.Mark())));
        return "";
    }

    return node.as<std::string>();
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
    if (!node.IsDefined()) {
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

void parseCycleType(const YAML::Node& node, CycleType& cycleType, ParseResult& output) {
    if (!node.IsDefined()) {
        return;
    }

    if (!node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("CycleType should be G_CYC_1CYCLE, G_CYC_2CYCLE, G_CYC_COPY, or G_CYC_FILL", node.Mark())));
        return;
    }

    std::string asString = node.as<std::string>();
    
    for (unsigned i = 0; i < (unsigned)CycleType::Count; ++i) {
        if (asString == gCycleTypeNames[i]) {
            cycleType = (CycleType)i;
            return;
        }
    }

    output.mErrors.push_back(ParseError(formatError("CycleType should be G_CYC_1CYCLE, G_CYC_2CYCLE, G_CYC_COPY, or G_CYC_FILL", node.Mark())));
    return;
}

void parseMaterialColor(const YAML::Node& node, MaterialColor& color, ParseResult& output) {
    color.mIsDefined = node.IsDefined();
    if (!color.mIsDefined) {
        return;
    }

    if (!node.IsMap()) {
        output.mErrors.push_back(ParseError(formatError("Color is expected to be map with r,g,b", node.Mark())));
    }

    color.r = parseInteger(node["r"], output, 0, 255);
    color.g = parseInteger(node["r"], output, 0, 255);
    color.b = parseInteger(node["r"], output, 0, 255);
}

void parsePrimColor(const YAML::Node& node, PrimitiveColor& color, ParseResult& output) {
    parseMaterialColor(node, color, output);

    if (!node.IsDefined() || !node.IsMap()) {
        return;
    }

    YAML::Node m = node["m"];
    if (m.IsDefined()) {
        color.m = parseInteger(m, output, 0, 255);
    }
    
    YAML::Node l = node["l"];
    if (l.IsDefined()) {
        color.l = parseInteger(l, output, 0, 255);
    }
} 

void parseMaterialResource(const YAML::Node& node, std::string& name, MaterialResource& resource, ParseResult& output) {
    resource.mName = name;
    resource.mContent = parseString(node["Content"], output);
    resource.mType = parseString(node["Type"], output);
    resource.mIsArray = node["IsArray"].as<bool>();
}

VertexType parseMaterialVertexType(const YAML::Node& node) {
    if (node.IsDefined() && node.IsScalar() && node.Scalar() == "Normal") {
        return VertexType::PosUVNormal;
    }

    return VertexType::PosUVColor;
}

G_IM_FMT parseTextureFormat(const YAML::Node& node, ParseResult& output) {
    std::string asString = parseString(node, output);

    if (asString == "RGBA") {
        return G_IM_FMT::G_IM_FMT_RGBA;
    }

    if (asString == "YUV") {
        return G_IM_FMT::G_IM_FMT_YUV;
    }

    if (asString == "CI") {
        return G_IM_FMT::G_IM_FMT_CI;
    }

    if (asString == "I") {
        return G_IM_FMT::G_IM_FMT_I;
    }

    if (asString == "IA") {
        return G_IM_FMT::G_IM_FMT_IA;
    }

    output.mErrors.push_back(ParseError(formatError("Texture format should be RGBA, YUV, CI, I, or IA", node.Mark())));

    return G_IM_FMT::G_IM_FMT_RGBA;
}

G_IM_SIZ parseTextureSize(const YAML::Node& node, ParseResult& output) {
    std::string asString = parseString(node, output);

    if (asString == "32") {
        return G_IM_SIZ::G_IM_SIZ_32b;
    }

    if (asString == "16") {
        return G_IM_SIZ::G_IM_SIZ_16b;
    }

    if (asString == "8") {
        return G_IM_SIZ::G_IM_SIZ_8b;
    }

    if (asString == "4") {
        return G_IM_SIZ::G_IM_SIZ_4b;
    }

    output.mErrors.push_back(ParseError(formatError("Texture size should be 32, 16, 8, or 4", node.Mark())));

    return G_IM_SIZ::G_IM_SIZ_16b;
}

G_IM_SIZ gDefaultImageSize[] = {
    // G_IM_FMT_RGBA
    G_IM_SIZ::G_IM_SIZ_16b,
    // G_IM_FMT_YUV
    G_IM_SIZ::G_IM_SIZ_16b,
    // G_IM_FMT_CI
    G_IM_SIZ::G_IM_SIZ_8b,
    // G_IM_FMT_I
    G_IM_SIZ::G_IM_SIZ_8b,
    // G_IM_FMT_IA
    G_IM_SIZ::G_IM_SIZ_16b,
};

std::shared_ptr<TextureDefinition> parseTexture(const YAML::Node& node, ParseResult& output) {
    if (!node.IsDefined()) {
        return NULL;
    }

    std::string filename;

    bool hasFormat = false;
    G_IM_FMT requestedFormat;
    bool hasSize = false;
    G_IM_SIZ requestedSize;

    if (node.IsScalar()) {
        filename = parseString(node, output);
    } else {
        filename = parseString(node["Filename"], output);

        auto yamlFormat = node["Format"];
        if (yamlFormat.IsDefined()) {
            requestedFormat = parseTextureFormat(yamlFormat, output);
            hasFormat = true;
        }

        auto yamlSize = node["Size"];
        if (yamlSize.IsDefined()) {
            requestedSize = parseTextureSize(yamlSize, output);
        }
    }

    if (!FileExists(filename)) {
        output.mErrors.push_back(ParseError(formatError(std::string("Could not open file ") + filename, node.Mark())));
        return NULL;
    }

    G_IM_FMT format;
    G_IM_SIZ size;

    if (hasFormat && hasSize) {
        format = requestedFormat;
        size = requestedSize;
    } else {
        TextureDefinition::DetermineIdealFormat(filename, format, size);

        if (hasFormat) {
            if (format != requestedFormat) {
                size = gDefaultImageSize[(int)requestedFormat];
            }
            format = requestedFormat;
        }

        if (hasSize) {
            size = requestedSize;
        }
    }

    if (!isImageFormatSupported(format, size)) {
        output.mErrors.push_back(ParseError(formatError("Unsupported image format ", node.Mark())));
        return NULL;
    }

    return gTextureCache.GetTexture(filename, format, size);
}
 
void parseMaterial(const YAML::Node& node, Material& material, ParseResult& output, std::map<std::string, std::shared_ptr<MaterialResource>>& resources) {
    parseRenderMode(node["RenderMode"], material.mRenderMode, output);
    parseCycleType(node["CycleType"], material.mCycleType, output);
    parsePrimColor(node["PrimColor"], material.mPrimColor, output);
    parseMaterialColor(node["EnvColor"], material.mEnvColor, output);
    parseMaterialColor(node["FogColor"], material.mFogColor, output);
    parseMaterialColor(node["BlendColor"], material.mBlendColor, output);
    material.mVertexType = parseMaterialVertexType(node["VertexType"]);

    material.mTexture0 = parseTexture(node["Texture0"], output);
    material.mTexture1 = parseTexture(node["Texture1"], output);

    const YAML::Node& content = node["Content"];

    if (content.IsDefined()) {
        material.mRawContent = content.as<std::string>();
    }

    const YAML::Node& usedResources = node["UsedResources"];

    if (usedResources.IsDefined() && usedResources.IsSequence()) {
        for (unsigned i = 0; i < usedResources.size(); ++i) {
            std::string resourceName = parseString(usedResources[i], output);
            auto resource = resources.find(resourceName);

            if (resource == resources.end()) {
                output.mErrors.push_back(ParseError(formatError("Could not find resource with given name", usedResources[i].Mark())));
            } else {
                material.mUsedResources.push_back(resource->second);
            }
        }
    }
}

void parseMaterialFile(std::istream& input, ParseResult& output) {
    try {
        YAML::Node doc = YAML::Load(input);

        std::map<std::string, std::shared_ptr<MaterialResource>> resources;
        
        const YAML::Node& resourceNodes = doc["Resources"];

        for (auto it = resourceNodes.begin(); it != resourceNodes.end(); ++it) {
            std::shared_ptr<MaterialResource> newResource(new MaterialResource());
            std::string resourceName = it->first.as<std::string>();
            parseMaterialResource(it->second, resourceName, *newResource, output);
            resources[resourceName] = newResource;
        }

        const YAML::Node& materials = doc["Materials"];

        for (auto it = materials.begin(); it != materials.end(); ++it) {
            Material newMaterial;
            parseMaterial(it->second, newMaterial, output, resources);
            output.mMaterialFile.mMaterials[it->first.as<std::string>()] = newMaterial;
        }
    } catch (YAML::ParserException& e) {
        output.mErrors.push_back(ParseError(e.what()));
    }
}