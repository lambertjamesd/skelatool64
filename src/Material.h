#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <ostream>

#include "DisplayList.h"
#include "ExtendedMesh.h"

enum class CullMode {
    Unknown,
    Front,
    Back,
    Both,
};

enum class CycleType {
    Unknown,
    _1Cycle,
    _2Cycle,
    Copy,
    Fill,
    Count,
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

    VertexType mVetexType;

    std::string mRawContent;
    std::vector<std::shared_ptr<MaterialResource>> mUsedResources;

    static void WriteResources(const std::vector<std::shared_ptr<MaterialResource>>& resources, std::map<std::string, std::string>& nameMapping, CFileDefinition& fileDef, std::ostream& output);
    void WriteToDL(const std::map<std::string, std::string>& nameMapping, DisplayList& output);
};

#endif