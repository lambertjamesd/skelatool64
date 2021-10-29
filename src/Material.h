#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>
#include <vector>
#include <memory>

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

    std::string mRawContent;
    std::vector<std::shared_ptr<MaterialResource>> mUsedResources;
};

#endif