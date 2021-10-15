
#include "Material.h"

MaterialColor::MaterialColor() : mIsDefined(false), r(0), g(0), b(0) {}

Material::Material() : 
    mCycleType(CycleType::Unknown),
    mCullMode(CullMode::Unknown),
    mRenderMode()
    {

}