#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <ostream>

#include "TextureDefinition.h"
#include "../DisplayList.h"
#include "../ExtendedMesh.h"

#include "MaterialEnums.h"

enum class CullMode {
    Unknown,
    Front,
    Back,
    Both,
};

struct BlendModeSource {
    
};

struct RenderMode {
    std::string mRenderMode1;
    std::string mRenderMode2;
};

struct MaterialColor {
    MaterialColor();
    bool mIsDefined;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
};

struct PrimitiveColor : MaterialColor {
    unsigned char m;
    unsigned char l;
};

struct MaterialResource {
public:
    std::string mName;
    std::string mContent;
    std::string mType;
    bool mIsArray;
};

class CFileDefinition;

class Material {
public:
    Material();
    CycleType mCycleType;
    CullMode mCullMode;
    RenderMode mRenderMode;
    PrimitiveColor mPrimColor;
    MaterialColor mEnvColor;
    MaterialColor mFogColor;
    MaterialColor mBlendColor;

    std::shared_ptr<TextureDefinition> mTexture0;
    std::shared_ptr<TextureDefinition> mTexture1;

    VertexType mVertexType;

    std::string mRawContent;
    std::vector<std::shared_ptr<MaterialResource>> mUsedResources;

    static void WriteResources(const std::vector<std::shared_ptr<MaterialResource>>& resources, std::map<std::string, std::string>& nameMapping, CFileDefinition& fileDefinition, const std::string& fileSuffix);
    void WriteToDL(const std::map<std::string, std::string>& nameMapping, DisplayList& output);

    static int TextureWidth(Material* material);
    static int TextureHeight(Material* material);

    static VertexType GetVertexType(Material* material);
};

#endif