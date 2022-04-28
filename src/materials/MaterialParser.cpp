
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

int parseOptionalInteger(const YAML::Node& node, ParseResult& output, int min, int max, int defaultValue) {
    if (!node.IsDefined()) {
        return defaultValue;
    }

    return parseInteger(node, output, min, max);
}

std::string parseString(const YAML::Node& node, ParseResult& output) {
    if (!node.IsDefined() || !node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("Expected a string", node.Mark())));
        return "";
    }

    return node.as<std::string>();
}

template <typename T>
T parseEnumType(const YAML::Node& node, ParseResult& output, const char** names, T defaultValue, int count) {
    if (!node.IsDefined()) {
        return defaultValue;
    }

    if (!node.IsScalar()) {
        output.mErrors.push_back(ParseError(formatError("Invalid type for enum", node.Mark())));
        return defaultValue;
    }

    std::string asString = node.as<std::string>();

    for (int i = 0; i < count; ++i) {
        if (asString == names[i]) {
            return (T)i;
        }
    }

    output.mErrors.push_back(ParseError(formatError("Invalid type for enum", node.Mark())));
    return defaultValue;
}

bool parseMaterialColor(const YAML::Node& node, Coloru8& color, ParseResult& output) {
    if (!node.IsDefined()) {
        return false;
    }

    if (!node.IsMap()) {
        output.mErrors.push_back(ParseError(formatError("Color is expected to be map with r,g,b", node.Mark())));
        return false;
    }

    color.r = parseInteger(node["r"], output, 0, 255);
    color.g = parseInteger(node["g"], output, 0, 255);
    color.b = parseInteger(node["b"], output, 0, 255);
    color.a = parseOptionalInteger(node["a"], output, 0, 255, 255);

    return true;
}

void parsePrimColor(const YAML::Node& node, MaterialState& state, ParseResult& output) {
    bool result = parseMaterialColor(node, state.primitiveColor, output);

    if (!result) {
        return;
    }

    state.usePrimitiveColor = true;

    YAML::Node m = node["m"];
    if (m.IsDefined()) {
        state.primitiveM = parseInteger(m, output, 0, 255);
    }
    
    YAML::Node l = node["l"];
    if (l.IsDefined()) {
        state.primitiveL = parseInteger(l, output, 0, 255);
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

    if (asString == "G_IM_FMT_RGBA") {
        return G_IM_FMT::G_IM_FMT_RGBA;
    }

    if (asString == "G_IM_FMT_RGBA") {
        return G_IM_FMT::G_IM_FMT_YUV;
    }

    if (asString == "G_IM_FMT_CI") {
        return G_IM_FMT::G_IM_FMT_CI;
    }

    if (asString == "G_IM_FMT_I") {
        return G_IM_FMT::G_IM_FMT_I;
    }

    if (asString == "G_IM_FMT_IA") {
        return G_IM_FMT::G_IM_FMT_IA;
    }

    output.mErrors.push_back(ParseError(formatError("Texture format should be G_IM_FMT_RGBA, G_IM_FMT_YUV, G_IM_FMT_CI, G_IM_FMT_I, or G_IM_FMT_IA", node.Mark())));

    return G_IM_FMT::G_IM_FMT_RGBA;
}

G_IM_SIZ parseTextureSize(const YAML::Node& node, ParseResult& output) {
    std::string asString = parseString(node, output);

    if (asString == "G_IM_SIZ_32b") {
        return G_IM_SIZ::G_IM_SIZ_32b;
    }

    if (asString == "G_IM_SIZ_16b") {
        return G_IM_SIZ::G_IM_SIZ_16b;
    }

    if (asString == "G_IM_SIZ_8b") {
        return G_IM_SIZ::G_IM_SIZ_8b;
    }

    if (asString == "G_IM_SIZ_4b") {
        return G_IM_SIZ::G_IM_SIZ_4b;
    }

    output.mErrors.push_back(ParseError(formatError("Texture size should be G_IM_SIZ_32b, G_IM_SIZ_16b, G_IM_SIZ_8b, or G_IM_SIZ_4b", node.Mark())));

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
    // parseRenderMode(node["RenderMode"], material.mRenderMode, output);

    material.mState.cycleType = parseEnumType(node["gDPSetCycleType"], output, gCycleTypeNames, CycleType::Unknown, (int)CycleType::Count);
    
    parsePrimColor(node["gDPSetPrimColor"], material.mState, output);
    material.mState.useEnvColor = parseMaterialColor(node["gDPSetEnvColor"], material.mState.envColor, output);
    material.mState.useFogColor = parseMaterialColor(node["gDPSetFogColor"], material.mState.fogColor, output);
    material.mState.useBlendColor = parseMaterialColor(node["gDPSetBlendColor"], material.mState.blendColor, output);

    // material.mTexture0 = parseTexture(node["Texture0"], output);
    // material.mTexture1 = parseTexture(node["Texture1"], output);
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