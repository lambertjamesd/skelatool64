#ifndef _THEME_WRITER_H
#define _THEME_WRITER_H

#include <assimp/scene.h>
#include <string>
#include "DisplayListSettings.h"

class ThemeWriter {

public:
    bool GetDecorName(const std::string& nodeName, std::string& output);
    void AppendContentFromScene(const aiScene* scene, DisplayListSettings& settings);
    std::string GetDecorID(const std::string & name);
};

#endif