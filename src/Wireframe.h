#ifndef _WIREFRAME_H
#define _WIREFRAME_H

#include <assimp/scene.h>
#include <ostream>
#include "ThemeWriter.h"
#include "DisplayListSettings.h"

class LevelDefinition;

void generateLevelWireframe(LevelDefinition& levelDef, const aiScene* scene, ThemeWriter* theme, DisplayListSettings& settings, std::ostream& fileContent);

#endif