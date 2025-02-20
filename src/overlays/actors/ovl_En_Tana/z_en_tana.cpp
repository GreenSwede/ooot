#define INTERNAL_SRC_OVERLAYS_ACTORS_OVL_EN_TANA_Z_EN_TANA_C
#include "actor_common.h"
/*
 * File: z_en_tana.c
 * Overlay: ovl_En_Tana
 * Description: Shop Shelves
 */

#include "z_en_tana.h"
#include "objects/object_shop_dungen/object_shop_dungen.h"
#include "def/sys_matrix.h"
#include "def/z_actor.h"
#include "def/z_rcp.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnTana_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTana_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTana_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTana_DrawWoodenShelves(Actor* thisx, GlobalContext* globalCtx);
void EnTana_DrawStoneShelves(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Tana_InitVars = {
    ACTOR_EN_TANA,
    ACTORCAT_PROP,
    FLAGS,
    OBJECT_SHOP_DUNGEN,
    sizeof(EnTana),
    (ActorFunc)EnTana_Init,
    (ActorFunc)EnTana_Destroy,
    (ActorFunc)EnTana_Update,
    NULL,
};

//! @bug A third entry is missing here. When printing the string indexed by `params` for type 2, the
//! next data entry will be dereferenced and print garbage, stopping any future printing.
//! In a non-matching context, pthis can cause a crash if the next item isn't a valid pointer.
static const char* sShelfTypes[] = {
    "木の棚", // "Wooden Shelves"
    "石の棚", // "Stone Shelves"
};

static const ActorFunc sDrawFuncs[] = {
    EnTana_DrawWoodenShelves,
    EnTana_DrawStoneShelves,
    EnTana_DrawStoneShelves,
};

static Gfx* sShelfDLists[] = {
    gShopDungenWoodenShelvesDL,
    gShopDungenStoneShelvesDL,
    gShopDungenStoneShelvesDL,
};

static void* sStoneTextures[] = {
    NULL,
    gShopDungenStone1Tex,
    gShopDungenStone2Tex,
};

void EnTana_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnTana* pthis = (EnTana*)thisx;

    osSyncPrintf("☆☆☆ %s ☆☆☆\n", sShelfTypes[thisx->params]);
    Actor_SetScale(thisx, 1.0f);
    thisx->flags &= ~ACTOR_FLAG_0;
    thisx->draw = sDrawFuncs[thisx->params];
}

void EnTana_Destroy(Actor* thisx, GlobalContext* globalCtx) {
}

void EnTana_Update(Actor* thisx, GlobalContext* globalCtx) {
}

void EnTana_DrawWoodenShelves(Actor* thisx, GlobalContext* globalCtx) {
    EnTana* pthis = (EnTana*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_tana.c", 148);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_tana.c", 152),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sShelfDLists[thisx->params]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_tana.c", 157);
}

void EnTana_DrawStoneShelves(Actor* thisx, GlobalContext* globalCtx) {
    EnTana* pthis = (EnTana*)thisx;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_tana.c", 163);

    func_80093D18(globalCtx->state.gfxCtx);
    gSPSegment(POLY_OPA_DISP++, 0x08, SEGMENTED_TO_VIRTUAL(sStoneTextures[thisx->params]));
    gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(globalCtx->state.gfxCtx, "../z_en_tana.c", 169),
              G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPDisplayList(POLY_OPA_DISP++, sShelfDLists[thisx->params]);

    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_tana.c", 174);
}
