#define INTERNAL_SRC_OVERLAYS_ACTORS_OVL_EN_TORYO_Z_EN_TORYO_C
#include "actor_common.h"
/*
 * File: z_en_tory.c
 * Overlay: ovl_En_Toryo
 * Description: Boss Carpenter
 */

#include "z_en_toryo.h"
#include "objects/object_toryo/object_toryo.h"
#include "def/sys_matrix.h"
#include "def/z_actor.h"
#include "def/z_collision_check.h"
#include "def/z_common_data.h"
#include "def/z_face_reaction.h"
#include "def/z_lib.h"
#include "def/z_message_PAL.h"
#include "def/z_rcp.h"
#include "def/z_skelanime.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_3)

void EnToryo_Init(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Update(Actor* thisx, GlobalContext* globalCtx);
void EnToryo_Draw(Actor* thisx, GlobalContext* globalCtx);

void func_80B20914(EnToryo* pthis, GlobalContext* globalCtx);
s32 EnToryo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx);
void EnToryo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx);

ActorInit En_Toryo_InitVars = {
    ACTOR_EN_TORYO,
    ACTORCAT_NPC,
    FLAGS,
    OBJECT_TORYO,
    sizeof(EnToryo),
    (ActorFunc)EnToryo_Init,
    (ActorFunc)EnToryo_Destroy,
    (ActorFunc)EnToryo_Update,
    (ActorFunc)EnToryo_Draw,
};

static ColliderCylinderInit sCylinderInit = {
    {
        COLTYPE_NONE,
        AT_NONE,
        AC_NONE,
        OC1_ON | OC1_TYPE_ALL,
        OC2_TYPE_2,
        COLSHAPE_CYLINDER,
    },
    {
        ELEMTYPE_UNK0,
        { 0x00000000, 0x00, 0x00 },
        { 0x00000000, 0x00, 0x00 },
        TOUCH_NONE,
        BUMP_NONE,
        OCELEM_ON,
    },
    { 18, 63, 0, { 0, 0, 0 } },
};

static CollisionCheckInfoInit2 sColChkInfoInit = { 0, 0, 0, 0, MASS_IMMOVABLE };

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x0),
    /* Deku stick    */ DMG_ENTRY(0, 0x0),
    /* Slingshot     */ DMG_ENTRY(0, 0x0),
    /* Explosive     */ DMG_ENTRY(0, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x0),
    /* Normal arrow  */ DMG_ENTRY(0, 0x0),
    /* Hammer swing  */ DMG_ENTRY(0, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x0),
    /* Kokiri sword  */ DMG_ENTRY(0, 0x0),
    /* Master sword  */ DMG_ENTRY(0, 0x0),
    /* Giant's Knife */ DMG_ENTRY(0, 0x0),
    /* Fire arrow    */ DMG_ENTRY(0, 0x0),
    /* Ice arrow     */ DMG_ENTRY(0, 0x0),
    /* Light arrow   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(0, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(0, 0x0),
    /* Fire magic    */ DMG_ENTRY(0, 0x0),
    /* Ice magic     */ DMG_ENTRY(0, 0x0),
    /* Light magic   */ DMG_ENTRY(0, 0x0),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(0, 0x0),
    /* Giant spin    */ DMG_ENTRY(0, 0x0),
    /* Master spin   */ DMG_ENTRY(0, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(0, 0x0),
    /* Giant jump    */ DMG_ENTRY(0, 0x0),
    /* Master jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(0, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

typedef struct {
    AnimationHeader* anim;
    f32 unk_4;
    u8 mode;
    f32 transitionRate;
} EnToryoAnimation;

static EnToryoAnimation sEnToryoAnimation = { &object_toryo_Anim_000E50, 1.0f, 0, 0 };

static Vec3f sMultVec = { 800.0f, 1000.0f, 0.0f };

void EnToryo_Init(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* pthis = (EnToryo*)thisx;
    s32 pad;

    switch (globalCtx->sceneNum) {
        case SCENE_SPOT09:
            if (LINK_AGE_IN_YEARS == YEARS_ADULT) {
                pthis->stateFlags |= 1;
            }
            break;
        case SCENE_SPOT01:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_DAY) {
                pthis->stateFlags |= 2;
            }
            break;
        case SCENE_KAKARIKO:
            if ((LINK_AGE_IN_YEARS == YEARS_CHILD) && IS_NIGHT) {
                pthis->stateFlags |= 4;
            }
            break;
    }

    if ((pthis->stateFlags & 7) == 0) {
        Actor_Kill(&pthis->actor);
    }

    ActorShape_Init(&pthis->actor.shape, 0.0f, ActorShadow_DrawCircle, 42.0f);
    SkelAnime_InitFlex(globalCtx, &pthis->skelAnime, &object_toryo_Skel_007150, NULL, pthis->jointTable, pthis->morphTable,
                       17);
    Collider_InitCylinder(globalCtx, &pthis->collider);
    Collider_SetCylinder(globalCtx, &pthis->collider, &pthis->actor, &sCylinderInit);
    CollisionCheck_SetInfo2(&pthis->actor.colChkInfo, &sDamageTable, &sColChkInfoInit);
    Actor_UpdateBgCheckInfo(globalCtx, &pthis->actor, 0.0f, 0.0f, 0.0f, 4);
    Animation_Change(&pthis->skelAnime, sEnToryoAnimation.anim, 1.0f, 0.0f,
                     Animation_GetLastFrame(sEnToryoAnimation.anim), sEnToryoAnimation.mode,
                     sEnToryoAnimation.transitionRate);
    pthis->stateFlags |= 8;
    pthis->actor.targetMode = 6;
    pthis->actionFunc = func_80B20914;
}

void EnToryo_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* pthis = (EnToryo*)thisx;

    Collider_DestroyCylinder(globalCtx, &pthis->collider);
}

s32 func_80B203D8(EnToryo* pthis, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = 1;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_EVENT:
            ret = 1;
            break;
        case TEXT_STATE_CHOICE:
            if (Message_ShouldAdvance(globalCtx)) {
                if (globalCtx->msgCtx.choiceIndex == 0) {
                    Message_CloseTextbox(globalCtx);
                    pthis->actor.parent = NULL;
                    player->exchangeItemId = EXCH_ITEM_NONE;
                    globalCtx->msgCtx.msgMode = MSGMODE_PAUSED;
                    pthis->actor.textId = 0x601B;
                    ret = 3;
                } else {
                    pthis->actor.textId = 0x606F;
                    ret = 2;
                }
            }
            break;
        case TEXT_STATE_DONE:
            switch (pthis->actor.textId) {
                case 0x5028:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 4;
                        ret = 0;
                    }
                    break;
                case 0x601B:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        ret = 4;
                    }
                    break;
                case 0x606F:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 2;
                        ret = 0;
                    }
                    break;
                case 0x606A:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        gSaveContext.infTable[23] |= 1;
                        ret = 0;
                    }
                    break;
                case 0x606B:
                case 0x606C:
                case 0x606D:
                case 0x606E:
                default:
                    ret = 1;
                    if (Message_ShouldAdvance(globalCtx)) {
                        ret = 0;
                    }
                    break;
            }
            break;
    }
    return ret;
}

s32 func_80B205CC(EnToryo* pthis, GlobalContext* globalCtx) {
    s32 pad;
    Player* player = GET_PLAYER(globalCtx);
    s32 ret = 5;

    switch (Message_GetState(&globalCtx->msgCtx)) {
        case TEXT_STATE_NONE:
        case TEXT_STATE_DONE_HAS_NEXT:
        case TEXT_STATE_CLOSING:
        case TEXT_STATE_DONE_FADING:
        case TEXT_STATE_CHOICE:
        case TEXT_STATE_EVENT:
            ret = 5;
            break;
        case TEXT_STATE_DONE:
            if (Message_ShouldAdvance(globalCtx)) {
                ret = 0;
            }
            break;
    }
    return ret;
}

u32 func_80B20634(EnToryo* pthis, GlobalContext* globalCtx) {
    u32 ret;

    if (pthis->unk_1E0 != 0) {
        if (pthis->unk_1E0 == 10) {
            func_80078884(NA_SE_SY_TRE_BOX_APPEAR);
            if (gSaveContext.infTable[23] & 2) {
                ret = 0x606E;
            } else {
                ret = 0x606D;
            }
        } else {
            ret = 0x200F;
        }
    }
    //! @bug return value may be unitialized
    return ret;
}

s32 func_80B206A0(EnToryo* pthis, GlobalContext* globalCtx) {
    s32 textId = Text_GetFaceReaction(globalCtx, 0);
    s32 ret = textId;

    if (textId == 0) {
        if ((pthis->stateFlags & 1)) {
            if ((gSaveContext.eventChkInf[9] & 0xF) == 0xF) {
                ret = 0x606C;
            } else if ((gSaveContext.infTable[23] & 1)) {
                ret = 0x606B;
            } else {
                ret = 0x606A;
            }
        } else if ((pthis->stateFlags & 2)) {
            if ((gSaveContext.infTable[23] & 4)) {
                ret = 0x5029;
            } else {
                ret = 0x5028;
            }
        } else {
            ret = textId;
            if ((pthis->stateFlags & 4)) {
                ret = 0x506C;
            }
        }
    }
    return ret;
}

void func_80B20768(EnToryo* pthis, GlobalContext* globalCtx) {
    Player* player = GET_PLAYER(globalCtx);
    s16 sp32;
    s16 sp30;

    if (pthis->unk_1E4 == 3) {
        Actor_ProcessTalkRequest(&pthis->actor, globalCtx);
        Message_ContinueTextbox(globalCtx, pthis->actor.textId);
        pthis->unk_1E4 = 1;
    }

    if (pthis->unk_1E4 == 1) {
        pthis->unk_1E4 = func_80B203D8(pthis, globalCtx);
    }

    if (pthis->unk_1E4 == 5) {
        pthis->unk_1E4 = func_80B205CC(pthis, globalCtx);
        return;
    }

    if (pthis->unk_1E4 == 2) {
        Message_ContinueTextbox(globalCtx, pthis->actor.textId);
        pthis->unk_1E4 = 1;
    }

    if (pthis->unk_1E4 == 4) {
        if (Actor_HasParent(&pthis->actor, globalCtx)) {
            pthis->actor.parent = NULL;
            pthis->unk_1E4 = 5;
        } else {
            func_8002F434(&pthis->actor, globalCtx, GI_SWORD_BROKEN, 100.0f, 10.0f);
        }
        return;
    }

    if (pthis->unk_1E4 == 0) {
        if (Actor_ProcessTalkRequest(&pthis->actor, globalCtx)) {
            pthis->unk_1E0 = func_8002F368(globalCtx);
            if (pthis->unk_1E0 != 0) {
                player->actor.textId = func_80B20634(pthis, globalCtx);
                pthis->actor.textId = player->actor.textId;
            }
            pthis->unk_1E4 = 1;
            return;
        }

        Actor_GetScreenPos(globalCtx, &pthis->actor, &sp32, &sp30);
        if ((sp32 >= 0) && (sp32 < 0x141) && (sp30 >= 0) && (sp30 < 0xF1)) {
            pthis->actor.textId = func_80B206A0(pthis, globalCtx);
            func_8002F298(&pthis->actor, globalCtx, 100.0f, 10);
        }
    }
}

void func_80B20914(EnToryo* pthis, GlobalContext* globalCtx) {
    SkelAnime_Update(&pthis->skelAnime);
    func_80B20768(pthis, globalCtx);
    if (pthis->unk_1E4 != 0) {
        pthis->stateFlags |= 0x10;
    } else {
        pthis->stateFlags &= ~0x10;
    }
}

void EnToryo_Update(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* pthis = (EnToryo*)thisx;
    ColliderCylinder* collider = &pthis->collider;
    Player* player = GET_PLAYER(globalCtx);
    f32 rot;

    Collider_UpdateCylinder(thisx, collider);
    CollisionCheck_SetOC(globalCtx, &globalCtx->colChkCtx, (Collider*)collider);

    pthis->actionFunc(pthis, globalCtx);

    if ((pthis->stateFlags & 8)) {
        pthis->unk_1EC.unk_18.x = player->actor.focus.pos.x;
        pthis->unk_1EC.unk_18.y = player->actor.focus.pos.y;
        pthis->unk_1EC.unk_18.z = player->actor.focus.pos.z;

        if ((pthis->stateFlags & 0x10)) {
            func_80034A14(thisx, &pthis->unk_1EC, 0, 4);
            return;
        }

        rot = thisx->yawTowardsPlayer - thisx->shape.rot.y;
        if ((rot < 14563.0f) && (rot > -14563.0f)) {
            func_80034A14(thisx, &pthis->unk_1EC, 0, 2);
        } else {
            func_80034A14(thisx, &pthis->unk_1EC, 0, 1);
        }
    }
}

void EnToryo_Draw(Actor* thisx, GlobalContext* globalCtx) {
    EnToryo* pthis = (EnToryo*)thisx;

    func_80093D18(globalCtx->state.gfxCtx);
    SkelAnime_DrawFlexOpa(globalCtx, pthis->skelAnime.skeleton, pthis->skelAnime.jointTable, pthis->skelAnime.dListCount,
                          EnToryo_OverrideLimbDraw, EnToryo_PostLimbDraw, pthis);
}

s32 EnToryo_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot,
                             void* thisx) {
    EnToryo* pthis = (EnToryo*)thisx;

    if ((pthis->stateFlags & 8)) {
        switch (limbIndex) {
            case 8:
                rot->x += pthis->unk_1EC.unk_0E.y;
                rot->y -= pthis->unk_1EC.unk_0E.x;
                break;
            case 15:
                rot->x += pthis->unk_1EC.unk_08.y;
                rot->z += pthis->unk_1EC.unk_08.x;
                break;
        }
    }
    return 0;
}

void EnToryo_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx) {
    EnToryo* pthis = (EnToryo*)thisx;

    switch (limbIndex) {
        case 15:
            Matrix_MultVec3f(&sMultVec, &pthis->actor.focus.pos);
            break;
    }
}
