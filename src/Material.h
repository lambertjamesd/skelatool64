#ifndef _MATERIAL_H
#define _MATERIAL_H

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
    
};

class Material {
public:
    Material();
    CycleMode mCycleMode;
    CullMode mCullMode;
};

#endif