
#include "Material.h"

#include "StringUtils.h"
#include "CFileDefinition.h"

MaterialColor::MaterialColor() : mIsDefined(false), r(0), g(0), b(0), a(255) {}

Material::Material() : 
    mCycleType(CycleType::Unknown),
    mCullMode(CullMode::Unknown),
    mRenderMode(),
    mVertexType(VertexType::PosUVColor)
    {

}

void Material::WriteResources(const std::vector<std::shared_ptr<MaterialResource>>& resources, std::map<std::string, std::string>& nameMapping, CFileDefinition& fileDef, std::ostream& output) {
    for (auto it = resources.begin(); it != resources.end(); ++it) {
        std::string finalName = fileDef.GetUniqueName((*it)->mName);
        nameMapping[(*it)->mName] = finalName;
        output << (*it)->mType << " " << finalName;

        if ((*it)->mIsArray) {
            output << "[]";
        }

        output << " = {" << std::endl;
        output << Indent((*it)->mContent, "    ") << std::endl;
        output << "};" << std::endl;
    }
}

void Material::WriteToDL(const std::map<std::string, std::string>& nameMapping, DisplayList& output) {
    std::string result = mRawContent;

    for (auto it = mUsedResources.begin(); it != mUsedResources.end(); ++it) {
        auto replacement = nameMapping.find((*it)->mName);
        result = FindAndReplace(result, (*it)->mName, replacement->second);
    }

    output.AddCommand(std::unique_ptr<DisplayListCommand>(new RawContentCommand(Trim(Indent(result, "    ")))));
}