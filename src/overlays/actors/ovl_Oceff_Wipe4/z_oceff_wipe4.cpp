#define INTERNAL_SRC_OVERLAYS_ACTORS_OVL_OCEFF_WIPE4_Z_OCEFF_WIPE4_C
#include "actor_common.h"
/*
 * File: z_oceff_wipe4.c
 * Overlay: ovl_Oceff_Wipe4
 * Description: Scarecrow's Song and an unused Ocarina Effect
 */

#include "z_oceff_wipe4.h"
#include "vt.h"
#include "def/sys_matrix.h"
#include "def/z_actor.h"
#include "def/z_camera.h"
#include "def/z_lib.h"
#include "def/z_parameter.h"
#include "def/z_rcp.h"

#define FLAGS (ACTOR_FLAG_4 | ACTOR_FLAG_25)

void OceffWipe4_Init(Actor* thisx, GlobalContext* globalCtx);
void OceffWipe4_Destroy(Actor* thisx, GlobalContext* globalCtx);
void OceffWipe4_Update(Actor* thisx, GlobalContext* globalCtx);
void OceffWipe4_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit Oceff_Wipe4_InitVars = {
    ACTOR_OCEFF_WIPE4,
    ACTORCAT_ITEMACTION,
    FLAGS,
    OBJECT_GAMEPLAY_KEEP,
    sizeof(OceffWipe4),
    (ActorFunc)OceffWipe4_Init,
    (ActorFunc)OceffWipe4_Destroy,
    (ActorFunc)OceffWipe4_Update,
    (ActorFunc)OceffWipe4_Draw,
};

void OceffWipe4_Init(Actor* thisx, GlobalContext* globalCtx) {
    OceffWipe4* pthis = (OceffWipe4*)thisx;

    Actor_SetScale(&pthis->actor, 0.1f);
    pthis->timer = 0;
    pthis->actor.world.pos = GET_ACTIVE_CAM(globalCtx)->eye;
    osSyncPrintf(VT_FGCOL(CYAN) " WIPE4 arg_data = %d\n" VT_RST, pthis->actor.params);
}

void OceffWipe4_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    OceffWipe4* pthis = (OceffWipe4*)thisx;

    func_800876C8(globalCtx);
}

void OceffWipe4_Update(Actor* thisx, GlobalContext* globalCtx) {
    OceffWipe4* pthis = (OceffWipe4*)thisx;

    pthis->actor.world.pos = GET_ACTIVE_CAM(globalCtx)->eye;
    if (pthis->timer < 50) {
        pthis->timer++;
    } else {
        Actor_Kill(&pthis->actor);
    }
}

#include "overlays/ovl_Oceff_Wipe4/ovl_Oceff_Wipe4.cpp"

void OceffWipe4_Draw(Actor* thisx, GlobalContext* globalCtx) {
    u32 scroll = globalCtx->state.frames & 0xFFF;
    OceffWipe4* pthis = (OceffWipe4*)thisx;
    f32 z;
    u8 alpha;
    s32 pad[2];
    Vec3f eye;
    Vtx* vtxPtr;
    Vec3f vec;

    eye = GET_ACTIVE_CAM(globalCtx)->eye;
    Camera_GetSkyboxOffset(&vec, GET_ACTIVE_CAM(globalCtx));
    if (pthis->timer < 16) {
        z = Math_SinS(pthis->timer * 1024) * 1330.0f;
    } else {
        z = 1330.0f;
    }

    vtxPtr = sFrustumVtx;
    if (pthis->timer >= 30) {
        alpha = 12 * (50 - pthis->timer);
    } else {
        alpha = 255;
    }

    vtxPtr[1].v.cn[3] = vtxPtr[3].v.cn[3] = vtxPtr[5].v.cn[3] = vtxPtr[7].v.cn[3] = vtxPtr[9].v.cn[3] =
        vtxPtr[11].v.cn[3] = vtxPtr[13].v.cn[3] = vtxPtr[15].v.cn[3] = vtxPtr[17].v.cn[3] = vtxPtr[19].v.cn[3] =
            vtxPtr[21].v.cn[3] = alpha;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_oceff_wipe4.c", 314);

    func_80093D84(globalCtx->state.gfxCtx);

    Matrix_Translate(eye.x + vec.x, eye.y + vec.y, eye.z + vec.z, MTXMODE_NEW);
    Matrix_Scale(0.1f, 0.1f, 0.1f, MTXMODE_APPLY);
    func_800D1FD4(&globalCtx->billboardMtxF);
    Matrix_Translate(0.0f, 0.0f, -z, MTXMODE_APPLY);

    gSPMatrix(POLY_XLU_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_oceff_wipe4.c", 324),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);

    if (pthis->actor.params == OCEFF_WIPE4_UNUSED) {
        gSPDisplayList(POLY_XLU_DISP++, sUnusedMaterialDL);
    } else {
        gSPDisplayList(POLY_XLU_DISP++, sMaterialDL);
    }

    gSPDisplayList(POLY_XLU_DISP++, sMaterial2DL);
    gSPDisplayList(POLY_XLU_DISP++, Gfx_TwoTexScroll(globalCtx->state.gfxCtx, 0, scroll * 2, scroll * (-2), 32, 64, 1,
                                                     scroll * (-1), scroll, 32, 32));
    gSPDisplayList(POLY_XLU_DISP++, &sMaterial2DL[11]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_oceff_wipe4.c", 344);
}
