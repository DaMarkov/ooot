#define INTERNAL_SRC_OVERLAYS_GAMESTATES_OVL_TITLE_Z_TITLE_C
#include "actor_common.h"
#include "framerate.h"
#include "z_title.h"
#include "z_opening.h"
#include <malloc.h>
#include "port/asset_loader/collada_loader.h"
/*
 * File: z_title.c
 * Overlay: ovl_title
 * Description: Displays the Nintendo Logo
 */

const char gBuildTeam[]	      = "Open Ocarina Team";
const char gBuildDate[]	      = __DATE__;
const char gBuildTime[]	      = __TIME__;
const char gBuildMakeOption[] = "";

#include "global.h"
#include "segment_symbols.h"
#include "alloca.h"
#include "textures/nintendo_rogo_static/nintendo_rogo_static.h"
#include "def/game.h"
#include "def/gfxprint.h"
#include "def/sys_matrix.h"
#include "def/xprintf.h"
#include "def/z_actor.h"
#include "def/z_common_data.h"
#include "def/z_kankyo.h"
#include "def/z_rcp.h"
#include "def/z_sram.h"
#include "def/z_std_dma.h"
#include "def/z_title.h"
#include "def/z_view.h"

void Title_PrintBuildInfo(Gfx** gfxp) {
    Gfx* g;
    GfxPrint* printer;

    g = *gfxp;
    g = func_8009411C(g);
    printer = (GfxPrint*)alloca(sizeof(GfxPrint));
    GfxPrint_Init(printer);
    GfxPrint_Open(printer, g);
    GfxPrint_SetColor(printer, 255, 155, 255, 255);
    GfxPrint_SetPos(printer, 9, 21);
    GfxPrint_Printf(printer, "");
    GfxPrint_SetColor(printer, 255, 255, 255, 255);
    GfxPrint_SetPos(printer, 7, 23);
    GfxPrint_Printf(printer, "[Creator:%s]", gBuildTeam);
    GfxPrint_SetPos(printer, 5, 24);
    GfxPrint_Printf(printer, "[Date:%s Time:%s]", gBuildDate, gBuildTime);
    g = GfxPrint_Close(printer);
    GfxPrint_Destroy(printer);
    *gfxp = g;
}

// Note: In other rom versions pthis function also updates unk_1D4, coverAlpha, addAlpha, visibleDuration to calculate
// the fade-in/fade-out + the duration of the n64 logo animation
void Title_Calc(TitleContext* pthis) {
    Input* input = &pthis->state.input[0];

    if (CHECK_BTN_ALL(input->press.button, BTN_START)) {
        pthis->exit = true;
    }

    if (pthis->coverAlpha == 0 && pthis->visibleDuration != 0) {
        //pthis->visibleDuration--;
        //pthis->unk_1D4--;
        if (pthis->unk_1D4 == 0) {
            pthis->unk_1D4 = 0x190;
        }
    } else {
        pthis->coverAlpha += pthis->addAlpha;

        if (pthis->coverAlpha <= 0) {
            pthis->coverAlpha = 0;
            pthis->addAlpha = 3;
        } else if (pthis->coverAlpha >= 0xFF) {
            pthis->coverAlpha = 0xFF;
            pthis->exit = true;
        }
    }

    pthis->uls = pthis->ult & 0x7F;
    //pthis->ult++;
    //pthis->exit = true;
}
oot::ColladaAsset test_asset;
void Title_SetupView(TitleContext* pthis, f32 x, f32 y, f32 z) {
    View* view = &pthis->view;
    Vec3f eye;
    Vec3f lookAt;
    Vec3f up;

    eye.x = x;
    eye.y = y;
    eye.z = z;
    up.x = up.z = 0.0f;
    lookAt.x = lookAt.y = lookAt.z = 0.0f;
    up.y = 1.0f;

    func_800AA460(view, 30.0f, 10.0f, 12800.0f);
    func_800AA358(view, &eye, &lookAt, &up);
    func_800AAA50(view, 0xF);
}

void Title_Draw(TitleContext* pthis) {
    static s16 sTitleRotY = 0;
    static Lights1 sTitleLights = gdSPDefLights1(0x64, 0x64, 0x64, 0xFF, 0xFF, 0xFF, 0x45, 0x45, 0x45);

    u16 y;
    u16 idx;
    s32 pad1;
    Vec3f v3;
    Vec3f v1;
    Vec3f v2;
    s32 pad2[2];

    OPEN_DISPS(pthis->state.gfxCtx, "../z_title.c", 395);

    v3.x = 69;
    v3.y = 69;
    v3.z = 69;
    v2.x = -4949.148;
    v2.y = 4002.5417;
    v1.x = 0;
    v1.y = 0;
    v1.z = 0;
    v2.z = 1119.0837;

    
    oot::DisplayList DL;
    DL << Gfx::gxDPPipeSync();
    DL << Gfx::gxDPSetTextureLUT(G_TT_NONE);
    DL << Gfx::gxSPTexture(0x0ED8, 0x0ED8, 0, G_TX_RENDERTILE, G_ON);
    DL << Gfx::gxDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 1, nintendo_rogo_staticTex_0029C0);
    DL << Gfx::gxDPLoadSync();
    DL << Gfx::gxDPLoadBlock(G_TX_LOADTILE, 0, 0, (((32)*(32) + G_IM_SIZ_8b_INCR) >> G_IM_SIZ_8b_SHIFT)-1,  CALC_DXT(32, G_IM_SIZ_8b_BYTES));
    DL << Gfx::gxDPPipeSync();
    DL << Gfx::gxDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, ((((32) * G_IM_SIZ_8b_LINE_BYTES)+7)>>3), 0,  G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_WRAP, 5, 1, G_TX_NOMIRROR | G_TX_WRAP, 5, 2);
    DL << Gfx::gxDPSetTileSize(G_TX_RENDERTILE, 0, 0,  ((32)-1) << G_TEXTURE_IMAGE_FRAC,   ((32)-1) << G_TEXTURE_IMAGE_FRAC);
    //DL << Gfx::gxDPSetCombineLERP(TEXEL0, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, 0, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, 1);
    DL << Gfx::gxDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    DL << Gfx::gxSPClearGeometryMode(G_FOG);
    DL << Gfx::gxSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
    DL << Gfx::gxDPSetPrimColor(0, 0, 255, 170, 255, 255);
    DL << Gfx::gxDPSetEnvColor(150, 0, 50, 128);
    test_asset >> DL;

    Gfx* debug_dl = new Gfx[1000];
    int count = 0;
    debug_dl[count++] = gsDPPipeSync();
    debug_dl[count++] = gsDPSetTextureLUT(G_TT_NONE);
    debug_dl[count++] = gsSPTexture(0x0ED8, 0x0ED8, 0, G_TX_RENDERTILE, G_ON);

    //debug_dl[count++] = gsDPLoadTextureBlock(nintendo_rogo_staticTex_0029C0, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 1);
    debug_dl[count++] = gsDPSetTextureImage(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 1, nintendo_rogo_staticTex_0029C0);
    debug_dl[count++] = gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b_LOAD_BLOCK, 0, 0, G_TX_LOADTILE,  0 , G_TX_NOMIRROR | G_TX_WRAP, 5, 1, G_TX_NOMIRROR | G_TX_WRAP, 5, 2);
    debug_dl[count++] = gsDPLoadSync();
    debug_dl[count++] = gsDPLoadBlock(G_TX_LOADTILE, 0, 0, (((32)*(32) + G_IM_SIZ_8b_INCR) >> G_IM_SIZ_8b_SHIFT)-1,  CALC_DXT(32, G_IM_SIZ_8b_BYTES));
    debug_dl[count++] = gsDPPipeSync();
    debug_dl[count++] = gsDPSetTile(G_IM_FMT_I, G_IM_SIZ_8b, ((((32) * G_IM_SIZ_8b_LINE_BYTES)+7)>>3), 0,  G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_WRAP, 5, 1, G_TX_NOMIRROR | G_TX_WRAP, 5, 2);
    debug_dl[count++] = gsDPSetTileSize(G_TX_RENDERTILE, 0, 0,  ((32)-1) << G_TEXTURE_IMAGE_FRAC,   ((32)-1) << G_TEXTURE_IMAGE_FRAC);

    

    debug_dl[count++] = gsDPSetCombineLERP(TEXEL0, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, 0, PRIMITIVE, ENVIRONMENT, COMBINED, ENVIRONMENT, 0, 0, 0, 1);
    debug_dl[count++] = gsDPSetRenderMode(G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
    debug_dl[count++] = gsSPClearGeometryMode(G_FOG);
    debug_dl[count++] = gsSPSetGeometryMode(G_CULL_BACK | G_LIGHTING | G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR);
    debug_dl[count++] = gsDPSetPrimColor(0, 0, 255, 170, 255, 255);
    debug_dl[count++] = gsDPSetEnvColor(150, 0, 50, 128);

    count += test_asset.AppendToDisplayList(&debug_dl[count]);

    //debug_dl[count++] = gsSPVertex(nintendo_rogo_staticVtx_001C00, 3, 0);
    //debug_dl[count++] = gsSPVertex(test_asset.m_Meshes[0].GetVB(), test_asset.m_Meshes[0].GetVBSize() , 0);
    //debug_dl[count++] = gsSPVertex(m_pVertices, 30 , 0);

    //for (int i = 0; i < test_asset.m_Meshes[0].GetIBSize(); i+=3)
        //debug_dl[count++] = gsSP1Triangle(test_asset.m_Meshes[0].GetIB()[i], test_asset.m_Meshes[0].GetIB()[i+1], test_asset.m_Meshes[0].GetIB()[i+2], 0);

    //debug_dl[count++] = gsSPEndDisplayList();

    /*debug_dl[count++] = gsSP1Triangle(0, 1, 2, 0);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[3], 14, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 3, 4, 5, 0);
    debug_dl[count++] = gsSP2Triangles(3, 5, 6, 0, 7, 8, 2, 0);
    debug_dl[count++] = gsSP2Triangles(7, 2, 9, 0, 10, 11, 12, 0);
    debug_dl[count++] = gsSP1Triangle(10, 12, 13, 0);
    debug_dl[count++] = gsDPPipeSync();
    debug_dl[count++] = gsDPSetPrimColor(0, 0, 170, 255, 255, 255);
    debug_dl[count++] = gsDPSetEnvColor(0, 50, 150, 128);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[17], 32, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0);
    debug_dl[count++] = gsSP2Triangles(4, 5, 6, 0, 4, 7, 5, 0);
    debug_dl[count++] = gsSP2Triangles(8, 9, 10, 0, 8, 11, 9, 0),
    debug_dl[count++] = gsSP2Triangles(12, 13, 14, 0, 13, 15, 14, 0);
    debug_dl[count++] = gsSP2Triangles(16, 17, 18, 0, 16, 19, 17, 0);
    debug_dl[count++] = gsSP2Triangles(20, 21, 22, 0, 21, 23, 22, 0);
    debug_dl[count++] = gsSP2Triangles(24, 25, 26, 0, 24, 27, 25, 0);
    debug_dl[count++] = gsSP2Triangles(28, 29, 30, 0, 29, 31, 30, 0);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[49], 32, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 0, 3, 1, 0);
    debug_dl[count++] = gsSP2Triangles(4, 5, 6, 0, 4, 7, 5, 0);
    debug_dl[count++] = gsSP2Triangles(8, 9, 10, 0, 9, 11, 10, 0);
    debug_dl[count++] = gsSP2Triangles(12, 11, 13, 0, 13, 14, 12, 0);
    debug_dl[count++] = gsSP2Triangles(15, 16, 17, 0, 16, 18, 17, 0);
    debug_dl[count++] = gsSP2Triangles(19, 20, 21, 0, 20, 22, 21, 0);
    debug_dl[count++] = gsSP2Triangles(23, 24, 25, 0, 25, 22, 23, 0);
    debug_dl[count++] = gsSP2Triangles(26, 27, 24, 0, 26, 28, 27, 0);
    debug_dl[count++] = gsSP2Triangles(29, 5, 30, 0, 29, 30, 31, 0);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[81], 4, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0);
    debug_dl[count++] = gsDPPipeSync();
    debug_dl[count++] = gsDPSetPrimColor(0, 0, 255, 255, 170, 255);
    debug_dl[count++] = gsDPSetEnvColor(50, 100, 0, 128);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[85], 32, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 0, 2, 3, 0);
    debug_dl[count++] = gsSP2Triangles(4, 5, 6, 0, 4, 6, 7, 0);
    debug_dl[count++] = gsSP2Triangles(8, 9, 10, 0, 8, 11, 9, 0);
    debug_dl[count++] = gsSP2Triangles(12, 7, 6, 0, 12, 13, 7, 0);
    debug_dl[count++] = gsSP2Triangles(14, 2, 1, 0, 14, 15, 2, 0);
    debug_dl[count++] = gsSP2Triangles(16, 17, 18, 0, 17, 19, 18, 0);
    debug_dl[count++] = gsSP2Triangles(20, 21, 22, 0, 20, 23, 21, 0);
    debug_dl[count++] = gsSP2Triangles(24, 25, 26, 0, 25, 27, 26, 0);
    debug_dl[count++] = gsSP2Triangles(28, 29, 30, 0, 28, 31, 29, 0);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[117], 29, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0);
    debug_dl[count++] = gsSP2Triangles(4, 5, 6, 0, 5, 7, 6, 0);
    debug_dl[count++] = gsSP2Triangles(8, 9, 10, 0, 9, 11, 10, 0);
    debug_dl[count++] = gsSP2Triangles(11, 12, 13, 0, 12, 7, 13, 0);
    debug_dl[count++] = gsSP2Triangles(14, 15, 3, 0, 15, 2, 3, 0);
    debug_dl[count++] = gsSP2Triangles(2, 16, 0, 0, 16, 17, 0, 0);
    debug_dl[count++] = gsSP2Triangles(17, 18, 19, 0, 18, 20, 19, 0);
    debug_dl[count++] = gsSP2Triangles(21, 22, 23, 0, 22, 24, 23, 0);
    debug_dl[count++] = gsSP2Triangles(25, 26, 27, 0, 26, 28, 27, 0);
    debug_dl[count++] = gsDPPipeSync();
    debug_dl[count++] = gsDPSetPrimColor(0, 0, 255, 255, 170, 255);
    debug_dl[count++] = gsDPSetEnvColor(200, 150, 0, 128);
    debug_dl[count++] = gsSPVertex(&nintendo_rogo_staticVtx_001C00[146], 32, 0);
    debug_dl[count++] = gsSP2Triangles(0, 1, 2, 0, 1, 3, 2, 0);
    debug_dl[count++] = gsSP2Triangles(4, 5, 6, 0, 5, 7, 6, 0);
    debug_dl[count++] = gsSP2Triangles(8, 9, 10, 0, 8, 11, 9, 0);
    debug_dl[count++] = gsSP2Triangles(12, 13, 14, 0, 13, 15, 14, 0);
    debug_dl[count++] = gsSP2Triangles(16, 17, 18, 0, 16, 19, 17, 0);
    debug_dl[count++] = gsSP2Triangles(20, 21, 22, 0, 21, 23, 22, 0);
    debug_dl[count++] = gsSP2Triangles(24, 25, 26, 0, 25, 27, 26, 0);
    debug_dl[count++] = gsSP2Triangles(28, 29, 30, 0, 29, 31, 30, 0);
    debug_dl[count++] = gsSPEndDisplayList();*/


    func_8002EABC(&v1, &v2, &v3, pthis->state.gfxCtx);
    gSPSetLights1(POLY_OPA_DISP++, sTitleLights);
    Title_SetupView(pthis, 0, 150.0, 300.0);
    func_80093D18(pthis->state.gfxCtx);
    Matrix_Translate(-60.0, -5.0, 0, MTXMODE_NEW);
    Matrix_Scale(1.5, 1.5, 1.5, MTXMODE_APPLY);
    Matrix_RotateZYX(sTitleRotY*2, sTitleRotY*7, 0, MTXMODE_APPLY);

    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(pthis->state.gfxCtx, "../z_title.c", 424), G_MTX_LOAD);
    //gSPDisplayList(POLY_OPA_DISP++, gNintendo64LogoDL);
    gSPDisplayList(POLY_OPA_DISP++, debug_dl);
    func_800944C4(pthis->state.gfxCtx);
    gDPPipeSync(POLY_OPA_DISP++);
    gDPSetCycleType(POLY_OPA_DISP++, G_CYC_2CYCLE);
    gDPSetRenderMode(POLY_OPA_DISP++, G_RM_XLU_SURF2, G_RM_OPA_CI | CVG_DST_WRAP);
    gDPSetCombineLERP(POLY_OPA_DISP++, TEXEL1, PRIMITIVE, ENV_ALPHA, TEXEL0, 0, 0, 0, TEXEL0, PRIMITIVE, ENVIRONMENT,
                      COMBINED, ENVIRONMENT, COMBINED, 0, PRIMITIVE, 0);
    gDPSetPrimColor(POLY_OPA_DISP++, 0, 0, 170, 255, 255, 255);
    gDPSetEnvColor(POLY_OPA_DISP++, 0, 0, 255, 128);

    gDPLoadMultiBlock(POLY_OPA_DISP++, nintendo_rogo_static_Tex_001800, 0x100, 1, G_IM_FMT_I, G_IM_SIZ_8b, 32, 32, 0,
                      G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, 5, 5, 2, 11);

    for (idx = 0, y = 94; idx < 16; idx++, y += 2) {
        gDPLoadTextureBlock(POLY_OPA_DISP++, &((u8*)nintendo_rogo_static_Tex_000000)[0x180 * idx], G_IM_FMT_I,
                            G_IM_SIZ_8b, 192, 2, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK,
                            G_TX_NOMASK, G_TX_NOLOD, G_TX_NOLOD);

        gDPSetTileSize(POLY_OPA_DISP++, 1, pthis->uls, (pthis->ult & 0x7F) - idx * 4, 0, 0);
        gSPTextureRectangle(POLY_OPA_DISP++, 388, y << 2, 1156, (y + 2) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10, 1 << 10);
    }

    Environment_FillScreen(pthis->state.gfxCtx, 0, 0, 0, (s16)pthis->coverAlpha, FILL_SCREEN_XLU);

    sTitleRotY += 100;

    CLOSE_DISPS(pthis->state.gfxCtx, "../z_title.c", 483);
}

void Title_Main(GameState* thisx) {
    TitleContext* pthis = (TitleContext*)thisx;

    OPEN_DISPS(pthis->state.gfxCtx, "../z_title.c", 494);

    gSPSegment(POLY_OPA_DISP++, 0, NULL);
    gSPSegment(POLY_OPA_DISP++, 1, pthis->staticSegment);
    Gfx_ClearDisplay(pthis->state.gfxCtx, 0, 0, 0);
    Title_Calc(pthis);
    Title_Draw(pthis);

    if (gIsCtrlr2Valid || 1) {
        Gfx* gfx = POLY_OPA_DISP;
        s32 pad;

        Title_PrintBuildInfo(&gfx);
        POLY_OPA_DISP = gfx;
    }

    if (pthis->exit) {
        gSaveContext.seqId = (u8)NA_BGM_DISABLED;
        gSaveContext.natureAmbienceId = 0xFF;
        gSaveContext.gameMode = 1;
        pthis->state.running = false;
        SET_NEXT_GAMESTATE(&pthis->state, Opening_Init, OpeningContext);
    }

    CLOSE_DISPS(pthis->state.gfxCtx, "../z_title.c", 541);
}

void Title_Destroy(GameState* thisx) {
    TitleContext* pthis = (TitleContext*)thisx;

    Sram_InitSram(&pthis->state, &pthis->sramCtx);
}

void Title_Init(GameState* thisx) {
    test_asset = oot::ColladaAsset("diamond.dae");
    auto dp = test_asset.CreateDisplayList();

    size_t size = POINTER_SUB(_nintendo_rogo_staticSegmentRomEnd, _nintendo_rogo_staticSegmentRomStart);
    TitleContext* pthis = (TitleContext*)thisx;

    //pthis->staticSegment = GameState_Alloc(&pthis->state, size, "../z_title.c", 611);
    osSyncPrintf("z_title.c\n");
    pthis->staticSegment = _nintendo_rogo_staticSegmentRomStart;
    framerate_set_profile(PROFILE_TITLE);
    Matrix_Init(&pthis->state);
    View_Init(&pthis->view, pthis->state.gfxCtx);
    pthis->state.main = Title_Main;
    pthis->state.destroy = Title_Destroy;
    pthis->exit = false;
    gSaveContext.fileNum = 0xFF;
    Sram_Alloc(&pthis->state, &pthis->sramCtx);
    pthis->ult = 0;
    pthis->unk_1D4 = 0x14;
    pthis->coverAlpha = 255;
    pthis->addAlpha = -3;
    pthis->visibleDuration = 0x3C;
}
