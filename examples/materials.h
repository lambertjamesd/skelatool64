#ifndef __SKOUT_MATERIALS_H__
#define __SKOUT_MATERIALS_H__

#include <ultra64.h>

#define TEST_MATERIAL_A_INDEX 0
#define DEFAULT_INDEX 1
#define OUTPUT_MATERIAL_COUNT 2
#define OUTPUT_TRANSPARENT_START 3

extern u64 output_image_i_8b[];
extern Gfx output_Test_Material_A[];
extern Gfx output_Test_Material_A_revert[];
extern Gfx output_default[];
extern Gfx output_default_revert[];
extern Gfx* output_material_list[];
extern Gfx* output_material_revert_list[];

#endif
