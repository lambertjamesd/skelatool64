#ifndef _MATERIAL_H
#define _MATERIAL_H

#include <string>
#include <optional>

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
};

struct PrimitiveColor : MaterialColor {
    unsigned char m;
    unsigned char l;
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
};

#endif