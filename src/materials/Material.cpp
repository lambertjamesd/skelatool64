
#include "Material.h"

#include "../StringUtils.h"
#include "../CFileDefinition.h"

MaterialColor::MaterialColor() : mIsDefined(false), r(0), g(0), b(0), a(255) {}

Material::Material() : 
    mCycleType(CycleType::Unknown),
    mCullMode(CullMode::Unknown),
    mRenderMode(),
    mVertexType(VertexType::PosUVColor)
    {

}

void Material::WriteResources(const std::vector<std::shared_ptr<MaterialResource>>& resources, std::map<std::string, std::string>& nameMapping, CFileDefinition& fileDefinition, const std::string& fileSuffix) {
    for (auto it = resources.begin(); it != resources.end(); ++it) {
        std::string finalName = fileDefinition.GetUniqueName((*it)->mName);
        nameMapping[(*it)->mName] = finalName;

        fileDefinition.AddDefinition(std::unique_ptr<FileDefinition>(new RawFileDefinition(
            (*it)->mType,
            finalName,
            (*it)->mIsArray,
            fileSuffix,
            (*it)->mContent
        )));
    }
}

void Material::WriteToDL(const std::map<std::string, std::string>& nameMapping, DisplayList& output) {
    std::string result = mRawContent;

    for (auto it = mUsedResources.begin(); it != mUsedResources.end(); ++it) {
        auto replacement = nameMapping.find((*it)->mName);
        result = FindAndReplace(result, (*it)->mName, replacement->second, true);
    }

    output.AddCommand(std::unique_ptr<DisplayListCommand>(new RawContentCommand(Trim(Indent(result, "    ")))));
}

int Material::TextureWidth(Material* material) {
    return material && material->mTexture0 ? material->mTexture0->Width() : 0;
}

int Material::TextureHeight(Material* material) {
    return material && material->mTexture0 ? material->mTexture0->Height() : 0;
}

VertexType Material::GetVertexType(Material* material) {
    return material ? material->mVertexType : VertexType::PosUVNormal;
}