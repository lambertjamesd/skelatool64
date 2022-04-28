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
#include "MaterialState.h"
#include "../definitions/DataChunk.h"

#include "MaterialEnums.h"

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
    MaterialState mState;

    void Write(const MaterialState& from, StructureDataChunk& output);

    static int TextureWidth(Material* material);
    static int TextureHeight(Material* material);

    static VertexType GetVertexType(Material* material);
};

#endif