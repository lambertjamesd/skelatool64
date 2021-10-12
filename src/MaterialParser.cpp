
#include "MaterialParser.h"

#include "yaml-cpp/yaml.h"

ParseError::ParseError(const std::string& message) :
    mMessage(message) {

}

void parseMaterial(const YAML::Node& node, Material& material, ParseResult& output) {

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