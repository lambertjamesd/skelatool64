#include "materials.h"
u64 output_image_i_8b[] = {
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x9ba49b8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8cbd, 0xe9eed39b8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8cc4f9, 0xffffffd28c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8cb9f5ff, 0xfffffff8b98c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8cc1f4ffff, 0xfffffffff0b08c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c9bd2fbffffff, 0xffffffffffe6a48c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8ca4deffffffffff, 0xf5f7ffffffffde9b, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0xb0e6fffffffffff0, 0xb9c1fbffffffffd2, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8cb9, 0xf0ffffffffffe4b0, 0x8c8cc8fcfffffffc, 0xc88c8c8c8c8c8c8c,
    0x8c8c8c8c8c8cc1f4, 0xfffffffffcd29b8c, 0x8c8c8cd2ffffffff, 0xfbc18c8c8c8c8c8c,
    0x8c8c8c8c8cc8fbff, 0xfffffffbc88c8c8c, 0x8c8c8c9bdeffffff, 0xfffacf8c8c8c8c8c,
    0x8c8c8c8cdbfdffff, 0xfffff4c18c8c8c8c, 0x8c8c8c8ca4e6ffff, 0xffffffc68c8c8c8c,
    0x8c8c8cd0ffffffff, 0xfff0b98c8c8c8c8c, 0x8c8c8c8c8cb0f2ff, 0xffffffe58c8c8c8c,
    0x8c8c8cecffffffff, 0xeeb58c8c8c8c8c8c, 0x8c8c8c8c8c9beaff, 0xffffffd78c8c8c8c,
    0x8c8c8cd0fffffffd, 0xfbeac29b8c8c8c8c, 0x8c8c8c8c8cc8fcff, 0xfffff4a98c8c8c8c,
    0x8c8c8c8cd0f5ffff, 0xfffffceac29b8c8c, 0x8c8c8c8ca9f3ffff, 0xfffcc88c8c8c8c8c,
    0x8c8c8c8c8cc1ffff, 0xfffffffffce4bb9b, 0x8c8c8c8cdbffffff, 0xffe29b8c8c8c8c8c,
    0x8c8c8c8c8c9bd7fc, 0xfffffffffffff8e1, 0xbb9b8cc1fcffffff, 0xf6b08c8c8c8c8c8c,
    0x8c8c8c8c8c8c9bc2, 0xeafcffffffffffff, 0xf8e0bdf2ffffffff, 0xcd8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x9bc2eafdffffffff, 0xfffffbffffffffe4, 0x9b8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c9bc8f2fdffff, 0xfffffffffffff6b0, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8ca9d4f3fd, 0xffffffffffffd78c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8ca9d4, 0xf4fffffffff2a98c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0xa9d7f4f6edb98c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8ca9bba98c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
    0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c, 0x8c8c8c8c8c8c8c8c,
};

Gfx output_Test_Material_A[] = {
    gsDPPipeSync(),
    gsDPSetTextureLOD(G_TL_TILE),
    gsDPSetAlphaDither(G_AD_DISABLE),
    gsDPSetCombineLERP(0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1),
    gsDPSetPrimColor(255, 255, 255, 0, 0, 255),
    gsDPSetEnvColor(128, 128, 128, 255),
    gsDPSetFogColor(200, 100, 50, 255),
    gsDPSetBlendColor(180, 120, 100, 255),
    gsSPTexture(65535, 65535, 0, 0, G_ON),
    gsDPTileSync(),
    gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 1, output_image_i_8b),
    gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 0, 0, G_TX_LOADTILE, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
    gsDPLoadSync(),
    gsDPLoadBlock(G_TX_LOADTILE, 0, 0, 511, 512),
    gsDPPipeSync(),
    gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, 4, 0, 0, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0, G_TX_WRAP | G_TX_NOMIRROR, 5, 0),
    gsDPSetTileSize(0, 0, 0, 124, 124),
    gsSPEndDisplayList(),
};

Gfx output_Test_Material_A_revert[] = {gsDPPipeSync(), gsSPEndDisplayList()};

Gfx output_default[] = {
    gsDPPipeSync(),
    gsDPSetCycleType(G_CYC_1CYCLE),
    gsDPSetRenderMode(G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2),
    gsSPEndDisplayList(),
};

Gfx output_default_revert[] = {gsDPPipeSync(), gsSPEndDisplayList()};

Gfx* output_material_list[] = {output_Test_Material_A, output_default};

Gfx* output_material_revert_list[] = {
    output_Test_Material_A_revert,
    output_default_revert,
};

