# Skeletool64

Converts various 3d formats to a N64 display list. Will support skeletal animation

# pre requisites to build

libpng libtiff libassimp-dev

# how to install with apt

```
echo "deb [trusted=yes] https://lambertjamesd.github.io/apt/ ./" | tee /etc/apt/sources.list.d/lambertjamesd.list
sudo apt install skeletool64
```

## how to use

'skeletool64 path/to/input/file.obj -o path/to/output.h`

The file formats located here [https://assimp.sourceforge.net/main_features_formats.html](https://assimp.sourceforge.net/main_features_formats.html) are supported, though not all are tested

will take the input model file and convert it to a header file with accompanying c file  `path/to/ouptut_geo.c`

Here some command line arguments

| Argument | Argument Type | Description |
|----------|--------|-------------|
| -o       | filename | where to write the output |
| --name   | string | The name to prefix all c definitions with |
| --fixed-point-scale | number | Scales all geometry by the given number when converting from floating point to fixed point geometry defaults to 256 |
| --model-scale | number | used to scale all geometry defaults to 1 |
| --m | filename | used to add a materials yaml or json file more details about how materials works below. You can add mulitple material files. You can also specify a 3d model as a material source and all materials from the file will be included. |
| --material-output | | if present only materials are generated instead of geometry |
| -r | number,number,number | a set of euler angles used to rotate the model |
| --default-material | string | the name of the material that is expected to be active when rendering this model. This is useful to reduce needed state changes between default state and the state needed to render the model defaults to `default` | 
| --force-material | string | if present all geometry uses the given material |

## materials 

skeletool64 will attempt to automatically create materials. You can override any material by name by adding them to a materials yaml file. You can find the schema for a material file here [schema/material-schema.json](schema/material-schema.json). When a material is specified by name in a materials file it will replace any materials in 3d files with the same name. An example of a materials file

```
materials:
  default:
    gDPSetRenderMode: G_RM_ZB_OPA_SURF
    gDPSetTextureFilter: G_TF_BILERP
    gDPSetTexturePersp: G_TP_PERSP
    gDPSetCycleType: G_CYC_1CYCLE
    gDPSetAlphaCompare: G_AC_NONE
    gSPGeometryMode:
      set: [G_CULL_BACK, G_ZBUFFER]
      clear: [G_CULL_FRONT, G_TEXTURE_GEN, G_TEXTURE_GEN_LINEAR]

  button_base_black:
    gDPSetPrimColor:
      r: 32
      g: 32
      b: 32
    gSPGeometryMode:
      set: [G_LIGHTING, G_SHADE]
    gDPSetCombineMode: 
      color: ["PRIMITIVE", "0", "SHADE", "0"]

  awe_total:
    gDPSetTile: 
      filename: ../../portal_pak_modified/materials/models/props_animsigns/awe_total.png
      siz: G_IM_SIZ_4b
      fmt: G_IM_FMT_I

    gDPSetRenderMode: G_RM_ZB_OPA_DECAL
        
    gSPGeometryMode:
      clear: [G_LIGHTING]
      set: [G_SHADE]
    gDPSetCombineMode:
      color: [SHADE, PRIMITIVE, TEXEL0, PRIMITIVE]
    gDPSetPrimColor:
      r: 242
      g: 245
      b: 247
    gDPSetCycleType: G_CYC_1CYCLE

  cube_fizzled:
    gDPSetTile: 
      filename:
        ../images/cube.png
      siz: G_IM_SIZ_16b
      fmt: G_IM_FMT_RGBA
      s:
        mirror: true
      t:
        mirror: true

    gDPSetRenderMode: 
      - blend: ["G_BL_CLR_IN", "G_BL_A_IN", "G_BL_CLR_FOG", "G_BL_1MA"]
      - G_RM_ZB_XLU_SURF

    gDPSetCycleType: G_CYC_2CYCLE
    gSPGeometryMode:
      set: [G_LIGHTING, G_SHADE]
    gDPSetCombineMode: 
      - color: ["SHADE", "0", "TEXEL0", "0"]
        alpha: ["0", "0", "0", "PRIMITIVE"]
      - color: ["NOISE", "COMBINED", "PRIMITIVE", "COMBINED"]
        alpha: ["0", "0", "0", "PRIMITIVE"]

    gDPSetFogColor:
      r: 0
      g: 0
      b: 0

    gDPSetPrimColor:
      r: 100
      g: 100
      b: 200
      a: 128
```

there are 4 reserved material names `texture_lit`, `texture_unlit`, `solid_lit`, and `solid_unlit`. If you specify a material with one of these names then materials generated from 3d model files will use one of these 4 materials as a base.