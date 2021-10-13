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

enum class CycleMode {
    Unknown,
    _1Cycle,
    _2Cycle,
    Copy,
    Fill,
};

struct BlendModeSource {
    
};

struct RenderMode {
    std::string mRenderMode1;
    std::string mRenderMode2;
};

class Material {
public:
    Material();
    CycleMode mCycleMode;
    CullMode mCullMode;
    RenderMode mRenderMode;
};

#endif