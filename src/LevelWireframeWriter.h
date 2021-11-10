#ifndef LEVEL_WIREFRAME_WRITER_H
#define LEVEL_WIREFRAME_WRITER_H

#include <assimp/scene.h>
#include <string>
#include "DisplayListSettings.h"
#include "LevelWriter.h"

class ThemeWriter;

void generateLevelWireframe(class LevelDefinition& levelDef, std::string filename, ThemeWriter* theme, DisplayListSettings& settings);

#endif