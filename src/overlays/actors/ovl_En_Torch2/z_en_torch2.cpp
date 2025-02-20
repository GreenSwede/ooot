#define INTERNAL_SRC_OVERLAYS_ACTORS_OVL_EN_TORCH2_Z_EN_TORCH2_C
#include "actor_common.h"
/*
 * File: z_en_torch2.c
 * Overlay: ovl_En_Torch2
 * Description: Dark Link
 */

#include "z_en_torch2.h"
#include "objects/object_torch2/object_torch2.h"
#include "def/code_800EC960.h"
#include "def/code_800FD970.h"
#include "def/padutils.h"
#include "def/z_actor.h"
#include "def/z_collision_check.h"
#include "def/z_common_data.h"
#include "def/z_effect.h"
#include "def/z_en_item00.h"
#include "def/z_lib.h"
#include "def/z_play.h"
#include "def/z_player_lib.h"
#include "def/z_rcp.h"
#include "def/z_skelanime.h"

#define FLAGS (ACTOR_FLAG_0 | ACTOR_FLAG_2 | ACTOR_FLAG_4 | ACTOR_FLAG_5)

typedef enum {
    /* 0 */ ENTORCH2_WAIT,
    /* 1 */ ENTORCH2_ATTACK,
    /* 2 */ ENTORCH2_DEATH,
    /* 3 */ ENTORCH2_DAMAGE
} EnTorch2ActionStates;

typedef enum {
    /*  0 */ FORWARD_SLASH_1H,
    /*  1 */ FORWARD_SLASH_2H,
    /*  2 */ FORWARD_COMBO_1H,
    /*  3 */ FORWARD_COMBO_2H,
    /*  4 */ RIGHT_SLASH_1H,
    /*  5 */ RIGHT_SLASH_2H,
    /*  6 */ RIGHT_COMBO_1H,
    /*  7 */ RIGHT_COMBO_2H,
    /*  8 */ LEFT_SLASH_1H,
    /*  9 */ LEFT_SLASH_2H,
    /* 10 */ LEFT_COMBO_1H,
    /* 11 */ LEFT_COMBO_2H,
    /* 12 */ STAB_1H,
    /* 13 */ STAB_2H,
    /* 14 */ STAB_COMBO_1H,
    /* 15 */ STAB_COMBO_2H,
    /* 16 */ FLIPSLASH_START,
    /* 17 */ JUMPSLASH_START,
    /* 18 */ FLIPSLASH_FINISH,
    /* 19 */ JUMPSLASH_FINISH,
    /* 20 */ BACKSLASH_RIGHT,
    /* 21 */ BACKSLASH_LEFT,
    /* 22 */ HAMMER_FORWARD,
    /* 23 */ HAMMER_SIDE,
    /* 24 */ SPIN_ATTACK_1H,
    /* 25 */ SPIN_ATTACK_2H,
    /* 26 */ BIG_SPIN_1H,
    /* 27 */ BIG_SPIN_2H
} PlayerSwordAnimation;

void EnTorch2_Init(Actor* thisx, GlobalContext* globalCtx);
void EnTorch2_Destroy(Actor* thisx, GlobalContext* globalCtx);
void EnTorch2_Update(Actor* thisx, GlobalContext* globalCtx);
void EnTorch2_Draw(Actor* thisx, GlobalContext* globalCtx);

ActorInit En_Torch2_InitVars = {
    ACTOR_EN_TORCH2,
    ACTORCAT_BOSS,
    FLAGS,
    OBJECT_TORCH2,
    sizeof(Player),
    (ActorFunc)EnTorch2_Init,
    (ActorFunc)EnTorch2_Destroy,
    (ActorFunc)EnTorch2_Update,
    (ActorFunc)EnTorch2_Draw,
};

static f32 sStickTilt = 0.0f;
static s16 sStickAngle = 0;
static f32 sSwordJumpHeight = 0.0f;
static s32 sHoldShieldTimer = 0;
static u8 sZTargetFlag = false;
static u8 sDeathFlag = false;

static Input sInput;
static u8 sSwordJumpState;
static Vec3f sSpawnPoint;
static u8 sJumpslashTimer;
static u8 sJumpslashFlag;
static u8 sActionState;
static u8 sSwordJumpTimer;
static u8 sCounterState;
static u8 sDodgeRollState;
static u8 sStaggerCount;
static u8 sStaggerTimer;
static s8 sLastSwordAnim;
static u8 sAlpha;

static DamageTable sDamageTable = {
    /* Deku nut      */ DMG_ENTRY(0, 0x1),
    /* Deku stick    */ DMG_ENTRY(2, 0x0),
    /* Slingshot     */ DMG_ENTRY(1, 0x0),
    /* Explosive     */ DMG_ENTRY(2, 0x0),
    /* Boomerang     */ DMG_ENTRY(0, 0x1),
    /* Normal arrow  */ DMG_ENTRY(2, 0x0),
    /* Hammer swing  */ DMG_ENTRY(2, 0x0),
    /* Hookshot      */ DMG_ENTRY(0, 0x1),
    /* Kokiri sword  */ DMG_ENTRY(1, 0x0),
    /* Master sword  */ DMG_ENTRY(2, 0x0),
    /* Giant's Knife */ DMG_ENTRY(4, 0x0),
    /* Fire arrow    */ DMG_ENTRY(2, 0x0),
    /* Ice arrow     */ DMG_ENTRY(2, 0x0),
    /* Light arrow   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 1   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 2   */ DMG_ENTRY(2, 0x0),
    /* Unk arrow 3   */ DMG_ENTRY(2, 0x0),
    /* Fire magic    */ DMG_ENTRY(2, 0xE),
    /* Ice magic     */ DMG_ENTRY(0, 0x6),
    /* Light magic   */ DMG_ENTRY(3, 0xD),
    /* Shield        */ DMG_ENTRY(0, 0x0),
    /* Mirror Ray    */ DMG_ENTRY(0, 0x0),
    /* Kokiri spin   */ DMG_ENTRY(1, 0x0),
    /* Giant spin    */ DMG_ENTRY(4, 0x0),
    /* Master spin   */ DMG_ENTRY(2, 0x0),
    /* Kokiri jump   */ DMG_ENTRY(2, 0x0),
    /* Giant jump    */ DMG_ENTRY(8, 0x0),
    /* Master jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 1     */ DMG_ENTRY(0, 0x0),
    /* Unblockable   */ DMG_ENTRY(0, 0x0),
    /* Hammer jump   */ DMG_ENTRY(4, 0x0),
    /* Unknown 2     */ DMG_ENTRY(0, 0x0),
};

void EnTorch2_Init(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    Player* pthis = (Player*)thisx;

    sInput.cur.button = sInput.press.button = sInput.rel.button = 0;
    sInput.cur.stick_x = sInput.cur.stick_y = 0;
    pthis->currentShield = PLAYER_SHIELD_HYLIAN;
    pthis->heldItemActionParam = pthis->heldItemId = PLAYER_AP_SWORD_MASTER;
    Player_SetModelGroup(pthis, 2);
    globalCtx->playerInit(pthis, globalCtx, &gDarkLinkSkel);
    pthis->actor.naviEnemyId = 0x26;
    pthis->cylinder.base.acFlags = AC_ON | AC_TYPE_PLAYER;
    pthis->swordQuads[0].base.atFlags = pthis->swordQuads[1].base.atFlags = AT_ON | AT_TYPE_ENEMY;
    pthis->swordQuads[0].base.acFlags = pthis->swordQuads[1].base.acFlags = AC_ON | AC_HARD | AC_TYPE_PLAYER;
    pthis->swordQuads[0].base.colType = pthis->swordQuads[1].base.colType = COLTYPE_METAL;
    pthis->swordQuads[0].info.toucher.damage = pthis->swordQuads[1].info.toucher.damage = 8;
    pthis->swordQuads[0].info.bumperFlags = pthis->swordQuads[1].info.bumperFlags = BUMP_ON;
    pthis->shieldQuad.base.atFlags = AT_ON | AT_TYPE_ENEMY;
    pthis->shieldQuad.base.acFlags = AC_ON | AC_HARD | AC_TYPE_PLAYER;
    pthis->actor.colChkInfo.damageTable = &sDamageTable;
    pthis->actor.colChkInfo.health = gSaveContext.healthCapacity >> 3;
    pthis->actor.colChkInfo.cylRadius = 60;
    pthis->actor.colChkInfo.cylHeight = 100;
    globalCtx->func_11D54(pthis, globalCtx);

    sActionState = ENTORCH2_WAIT;
    sDodgeRollState = 0;
    sSwordJumpHeight = 0.0f;
    sSwordJumpState = 0;
    sJumpslashTimer = 0;
    sJumpslashFlag = false;
    sCounterState = sStaggerTimer = sStaggerCount = 0;
    sLastSwordAnim = 0;
    sAlpha = 95;
    sSpawnPoint = pthis->actor.home.pos;
}

void EnTorch2_Destroy(Actor* thisx, GlobalContext* globalCtx) {
    s32 pad;
    Player* pthis = (Player*)thisx;

    Effect_Delete(globalCtx, pthis->swordEffectIndex);
    func_800F5B58();
    Collider_DestroyCylinder(globalCtx, &pthis->cylinder);
    Collider_DestroyQuad(globalCtx, &pthis->swordQuads[0]);
    Collider_DestroyQuad(globalCtx, &pthis->swordQuads[1]);
    Collider_DestroyQuad(globalCtx, &pthis->shieldQuad);
}

Actor* EnTorch2_GetAttackItem(GlobalContext* globalCtx, Player* pthis) {
    Actor* rangedItem = Actor_GetProjectileActor(globalCtx, &pthis->actor, 4000.0f);

    if (rangedItem != NULL) {
        return rangedItem;
    } else {
        return func_80033684(globalCtx, &pthis->actor);
    }
}

s32 EnTorch2_SwingSword(GlobalContext* globalCtx, Input* input, Player* pthis) {
    f32 noAttackChance = 0.0f;
    s32 attackDelay = 7;
    Player* player = GET_PLAYER(globalCtx);

    if ((pthis->linearVelocity < 0.0f) || (player->linearVelocity < 0.0f)) {
        return 0;
    }
    if (gSaveContext.health < 0x50) {
        attackDelay = 15;
        noAttackChance += 0.3f;
    }
    if (sAlpha != 255) {
        noAttackChance += 2.0f;
    }
    if ((((globalCtx->gameplayFrames & attackDelay) == 0) || (sSwordJumpState != 0)) &&
        (noAttackChance <= Rand_ZeroOne())) {
        if (sSwordJumpState == 0) {
            switch ((s32)(Rand_ZeroOne() * 7.0f)) {
                case 1:
                case 5:
                    sStickAngle += 0x4000;
                    sStickTilt = 127.0f;
                    break;
                case 2:
                case 6:
                    sStickAngle -= 0x4000;
                    sStickTilt = 127.0f;
                    break;
            }
        }
        input->cur.button = BTN_B;
        return 1;
    }
    return 0;
}

void EnTorch2_Backflip(Player* pthis, Input* input, Actor* thisx) {
    thisx->world.rot.y = thisx->shape.rot.y = thisx->yawTowardsPlayer;
    sStickAngle = thisx->yawTowardsPlayer + 0x8000;
    sStickTilt = 127.0f;
    sZTargetFlag = true;
    input->cur.button = BTN_A;
    pthis->invincibilityTimer = 10;
    sCounterState = 0;
}

void EnTorch2_Update(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    Player* player2 = GET_PLAYER(globalCtx2);
    Player* player = player2;
    Player* pthis = (Player*)thisx;
    Input* input = &sInput;
    Camera* camera;
    s16 sp66;
    u8 staggerThreshold;
    s8 stickY;
    s32 pad60;
    Actor* attackItem;
    s16 sp5A;
    s16 pad58;
    u32 pad54;
    f32 sp50;
    s16 sp4E;

    sp5A = player->actor.shape.rot.y - pthis->actor.shape.rot.y;
    input->cur.button = 0;
    camera = Gameplay_GetCamera(globalCtx, 0);
    attackItem = EnTorch2_GetAttackItem(globalCtx, pthis);
    switch (sActionState) {
        case ENTORCH2_WAIT:
            pthis->actor.shape.rot.y = pthis->actor.world.rot.y = pthis->actor.yawTowardsPlayer;
            pthis->skelAnime.curFrame = 0.0f;
            pthis->skelAnime.playSpeed = 0.0f;
            pthis->actor.world.pos.x = (Math_SinS(pthis->actor.world.rot.y) * 25.0f) + sSpawnPoint.x;
            pthis->actor.world.pos.z = (Math_CosS(pthis->actor.world.rot.y) * 25.0f) + sSpawnPoint.z;
            if ((pthis->actor.xzDistToPlayer <= 120.0f) || Actor_IsTargeted(globalCtx, &pthis->actor) ||
                (attackItem != NULL)) {
                if (attackItem != NULL) {
                    sDodgeRollState = 1;
                    sStickAngle = pthis->actor.yawTowardsPlayer;
                    sStickTilt = 127.0f;
                    input->cur.button = BTN_A;
                    sZTargetFlag = false;
                    sp66 = camera->camDir.y - sStickAngle;
                    sInput.cur.stick_x = sStickTilt * Math_SinS(sp66);
                    stickY = sStickTilt * Math_CosS(sp66);
                    if (stickY) {}
                    sInput.cur.stick_y = stickY;
                }
                func_800F5ACC(NA_BGM_MINI_BOSS);
                sActionState = ENTORCH2_ATTACK;
            }
            break;

        case ENTORCH2_ATTACK:
            sStickTilt = 0.0f;

            // Handles Dark Link's sword clanking on Link's sword

            if ((pthis->swordQuads[0].base.acFlags & AC_BOUNCED) || (pthis->swordQuads[1].base.acFlags & AC_BOUNCED)) {
                pthis->swordQuads[0].base.acFlags &= ~AC_BOUNCED;
                pthis->swordQuads[1].base.acFlags &= ~AC_BOUNCED;
                pthis->swordQuads[0].base.atFlags |= AT_BOUNCED;
                pthis->swordQuads[1].base.atFlags |= AT_BOUNCED;
                pthis->cylinder.base.acFlags &= ~AC_HIT;

                if (sLastSwordAnim != pthis->swordAnimation) {
                    sStaggerCount++;
                    sLastSwordAnim = pthis->swordAnimation;
                }
                /*! @bug
                 *  This code is needed to reset sCounterState, and should run regardless
                 *  of how much health Link has. Without it, sCounterState stays at 2 until
                 *  something else resets it, preventing Dark Link from using his shield and
                 *  creating a hole in his defenses. This also makes Dark Link harder at low
                 *  health, while the other health checks are intended to make him easier.
                 */
                if ((gSaveContext.health < 0x50) && (sCounterState != 0)) {
                    sCounterState = 0;
                    sStaggerTimer = 50;
                }
            }
            if ((sCounterState != 0) && (pthis->swordState != 0)) {
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &pthis->swordQuads[0].base);
                CollisionCheck_SetAC(globalCtx, &globalCtx->colChkCtx, &pthis->swordQuads[1].base);
            }

            // Ignores hits when jumping on Link's sword
            if ((pthis->invincibilityTimer < 0) && (sActionState != ENTORCH2_DAMAGE) &&
                (pthis->cylinder.base.acFlags & AC_HIT)) {
                pthis->cylinder.base.acFlags &= ~AC_HIT;
            }

            // Handles Dark Link rolling to dodge item attacks

            if (sDodgeRollState != 0) {
                sStickTilt = 127.0f;
            } else if (attackItem != NULL) {
                sDodgeRollState = 1;
                sStickAngle = pthis->actor.yawTowardsPlayer;
                sStickTilt = 127.0f;
                input->cur.button = BTN_A;
            } else if (sJumpslashTimer == 0) {

                // Handles Dark Link's initial reaction to jumpslashes

                if (((player->swordState != 0) || (player->actor.velocity.y > -3.0f)) &&
                    (player->swordAnimation == JUMPSLASH_START)) {
                    pthis->actor.world.rot.y = pthis->actor.shape.rot.y = pthis->actor.yawTowardsPlayer;

                    if (globalCtx->gameplayFrames % 2) {
                        sStickAngle = pthis->actor.yawTowardsPlayer + 0x4000;
                    } else {
                        sStickAngle = pthis->actor.yawTowardsPlayer - 0x4000;
                    }
                    sStickTilt = 127.0f;
                    sJumpslashTimer = 15;
                    sJumpslashFlag = false;
                    input->cur.button |= BTN_A;

                    // Handles jumping on Link's sword

                } else if (sSwordJumpState != 0) {
                    sStickTilt = 0.0f;
                    player->stateFlags3 |= 4;
                    Math_SmoothStepToF(&pthis->actor.world.pos.x,
                                       (Math_SinS(player->actor.shape.rot.y - 0x3E8) * 45.0f) +
                                           player->actor.world.pos.x,
                                       1.0f, 5.0f, 0.0f);
                    Math_SmoothStepToF(&pthis->actor.world.pos.z,
                                       (Math_CosS(player->actor.shape.rot.y - 0x3E8) * 45.0f) +
                                           player->actor.world.pos.z,
                                       1.0f, 5.0f, 0.0f);
                    sSwordJumpTimer--;
                    if (((u32)sSwordJumpTimer == 0) || ((player->invincibilityTimer > 0) && (pthis->swordState == 0))) {
                        pthis->actor.world.rot.y = pthis->actor.shape.rot.y = pthis->actor.yawTowardsPlayer;
                        input->cur.button = BTN_A;
                        player->stateFlags3 &= ~4;
                        sStickTilt = 127.0f;
                        player->skelAnime.curFrame = 3.0f;
                        sStickAngle = pthis->actor.yawTowardsPlayer + 0x8000;
                        sSwordJumpTimer = sSwordJumpState = 0;
                        pthis->actor.flags |= ACTOR_FLAG_0;
                    } else if (sSwordJumpState == 1) {
                        if (sSwordJumpTimer < 16) {
                            EnTorch2_SwingSword(globalCtx, input, pthis);
                            sSwordJumpState++;
                        } else if (sSwordJumpTimer == 19) {
                            func_800F4190(&pthis->actor.projectedPos, NA_SE_VO_LI_AUTO_JUMP);
                        }
                    }
                } else {
                    // This does nothing, as sHoldShieldTimer is never set.
                    if (sHoldShieldTimer != 0) {
                        sHoldShieldTimer--;
                        input->cur.button = BTN_R;
                    }

                    // Handles Dark Link's reaction to sword attack other than jumpslashes

                    if (func_800354B4(globalCtx, &pthis->actor, 120.0f, 0x7FFF, 0x7FFF, pthis->actor.world.rot.y)) {
                        if ((player->swordAnimation == STAB_1H) && (pthis->actor.xzDistToPlayer < 90.0f)) {

                            // Handles the reaction to a one-handed stab. If the conditions are satisfied,
                            // Dark Link jumps on Link's sword. Otherwise he backflips away.

                            if ((pthis->swordState == 0) && (sCounterState == 0) && (player->invincibilityTimer == 0) &&
                                (player->swordAnimation == STAB_1H) && (pthis->actor.xzDistToPlayer <= 85.0f) &&
                                Actor_IsTargeted(globalCtx, &pthis->actor)) {

                                sStickTilt = 0.0f;
                                sSwordJumpState = 1;
                                player->stateFlags3 |= 4;
                                pthis->actor.flags &= ~ACTOR_FLAG_0;
                                sSwordJumpTimer = 27;
                                player->swordState = 0;
                                player->linearVelocity = 0.0f;
                                pthis->invincibilityTimer = -7;
                                pthis->linearVelocity = 0.0f;
                                player->skelAnime.curFrame = 2.0f;
                                LinkAnimation_Update(globalCtx, &player->skelAnime);
                                sHoldShieldTimer = 0;
                                input->cur.button = BTN_A;
                            } else {
                                EnTorch2_Backflip(pthis, input, &pthis->actor);
                            }
                        } else {

                            // Handles reactions to all other sword attacks

                            sStickAngle = thisx->yawTowardsPlayer;
                            input->cur.button = BTN_B;

                            if (player->swordAnimation <= FORWARD_COMBO_2H) {
                                sStickTilt = 0.0f;
                            } else if (player->swordAnimation <= RIGHT_COMBO_2H) {
                                sStickTilt = 127.0f;
                                sStickAngle += 0x4000;
                            } else if (player->swordAnimation <= LEFT_COMBO_2H) {
                                sStickTilt = 127.0f;
                                sStickAngle -= 0x4000;
                            } else if (player->swordAnimation <= HAMMER_SIDE) {
                                input->cur.button = BTN_R;
                            } else if (player->swordAnimation <= BIG_SPIN_2H) {
                                EnTorch2_Backflip(pthis, input, &pthis->actor);
                            } else {
                                EnTorch2_Backflip(pthis, input, &pthis->actor);
                            }
                            if (!CHECK_BTN_ANY(input->cur.button, BTN_A | BTN_R) && (pthis->swordState == 0) &&
                                (player->swordState != 0)) {
                                sCounterState = 1;
                            }
                        }
                    } else {

                        // Handles movement and attacks when not reacting to Link's actions

                        sStickAngle = thisx->yawTowardsPlayer;
                        sp50 = 0.0f;
                        if ((90.0f >= pthis->actor.xzDistToPlayer) && (pthis->actor.xzDistToPlayer > 70.0f) &&
                            (ABS(sp5A) >= 0x7800) && (pthis->actor.isTargeted || !(player->stateFlags1 & 0x00400000))) {
                            EnTorch2_SwingSword(globalCtx, input, pthis);
                        } else if (((pthis->actor.xzDistToPlayer <= 70.0f) ||
                                    ((pthis->actor.xzDistToPlayer <= 80.0f + sp50) && (player->swordState != 0))) &&
                                   (pthis->swordState == 0)) {
                            if (!EnTorch2_SwingSword(globalCtx, input, pthis) && (pthis->swordState == 0) &&
                                (sCounterState == 0)) {
                                EnTorch2_Backflip(pthis, input, &pthis->actor);
                            }
                        } else if (pthis->actor.xzDistToPlayer <= 50 + sp50) {
                            sStickTilt = 127.0f;
                            sStickAngle = pthis->actor.yawTowardsPlayer;
                            if (!pthis->actor.isTargeted) {
                                Math_SmoothStepToS(&sStickAngle, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328, 0);
                            }
                        } else if (pthis->actor.xzDistToPlayer > 100.0f + sp50) {
                            if ((player->swordState == 0) || (player->swordAnimation < SPIN_ATTACK_1H) ||
                                (player->swordAnimation > BIG_SPIN_2H) || (pthis->actor.xzDistToPlayer >= 280.0f)) {
                                sStickTilt = 127.0f;
                                sStickAngle = pthis->actor.yawTowardsPlayer;
                                if (!pthis->actor.isTargeted) {
                                    Math_SmoothStepToS(&sStickAngle, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328, 0);
                                }
                            } else {
                                EnTorch2_Backflip(pthis, input, &pthis->actor);
                            }
                        } else if (((ABS(sp5A) < 0x7800) && (ABS(sp5A) >= 0x3000)) ||
                                   !EnTorch2_SwingSword(globalCtx, input, pthis)) {
                            sStickAngle = pthis->actor.yawTowardsPlayer;
                            sStickTilt = 127.0f;
                            if (!pthis->actor.isTargeted) {
                                Math_SmoothStepToS(&sStickAngle, player->actor.shape.rot.y + 0x7FFF, 1, 0x2328, 0);
                            }
                        }
                    }
                }

                // Handles Dark Link's counterattack to jumpslashes

            } else if (sJumpslashFlag && (sAlpha == 255) && (pthis->actor.velocity.y > 0)) {
                input->cur.button |= BTN_B;
            } else if (!sJumpslashFlag && (pthis->actor.bgCheckFlags & 1)) {
                pthis->actor.world.rot.y = pthis->actor.shape.rot.y = pthis->actor.yawTowardsPlayer;
                sStickAngle = pthis->actor.yawTowardsPlayer;
                if (sAlpha != 255) {
                    sStickAngle += 0x8000;
                    sStickTilt = 127.0f;
                    sZTargetFlag = true;
                }
                input->cur.button |= BTN_A;
                sJumpslashFlag = true;
                pthis->invincibilityTimer = 10;
            }

            // Rotates Dark Link's stick angle from Link-relative to camera-relative.

            sp66 = camera->camDir.y - sStickAngle;
            sInput.cur.stick_x = sStickTilt * Math_SinS(sp66);
            stickY = sStickTilt * Math_CosS(sp66);
            if (sAlpha) {}
            sInput.cur.stick_y = stickY;

            if ((sAlpha != 255) && ((globalCtx->gameplayFrames % 8) == 0)) {
                sAlpha++;
            }
            break;

        case ENTORCH2_DAMAGE:
            pthis->swordState = 0;
            input->cur.stick_x = input->cur.stick_y = 0;
            if ((pthis->invincibilityTimer > 0) && (pthis->actor.world.pos.y < (pthis->actor.floorHeight - 160.0f))) {
                pthis->stateFlags3 &= ~1;
                pthis->actor.flags |= ACTOR_FLAG_0;
                pthis->invincibilityTimer = 0;
                pthis->actor.velocity.y = 0.0f;
                pthis->actor.world.pos.y = sSpawnPoint.y + 40.0f;
                pthis->actor.world.pos.x = (Math_SinS(player->actor.shape.rot.y) * -120.0f) + player->actor.world.pos.x;
                pthis->actor.world.pos.z = (Math_CosS(player->actor.shape.rot.y) * -120.0f) + player->actor.world.pos.z;
                if (Actor_WorldDistXYZToPoint(&pthis->actor, &sSpawnPoint) > 800.0f) {
                    sp50 = Rand_ZeroOne() * 20.0f;
                    sp4E = Rand_CenteredFloat(4000.0f);
                    pthis->actor.shape.rot.y = pthis->actor.world.rot.y =
                        Math_Vec3f_Yaw(&sSpawnPoint, &player->actor.world.pos);
                    pthis->actor.world.pos.x =
                        (Math_SinS(pthis->actor.world.rot.y + sp4E) * (25.0f + sp50)) + sSpawnPoint.x;
                    pthis->actor.world.pos.z =
                        (Math_CosS(pthis->actor.world.rot.y + sp4E) * (25.0f + sp50)) + sSpawnPoint.z;
                    pthis->actor.world.pos.y = sSpawnPoint.y;
                } else {
                    pthis->actor.world.pos.y = pthis->actor.floorHeight;
                }
                Math_Vec3f_Copy(&pthis->actor.home.pos, &pthis->actor.world.pos);
                globalCtx->func_11D54(pthis, globalCtx);
                sActionState = ENTORCH2_ATTACK;
                sStickTilt = 0.0f;
                if (sAlpha != 255) {
                    sStaggerCount = 0;
                    sStaggerTimer = 0;
                }
            }
            break;

        case ENTORCH2_DEATH:
            if (sAlpha - 13 <= 0) {
                sAlpha = 0;
                Actor_Kill(&pthis->actor);
                return;
            }
            sAlpha -= 13;
            pthis->actor.shape.shadowAlpha -= 13;
            break;
    }

    // Causes Dark Link to shield in place when Link is using magic attacks other than the spin attack

    if ((gSaveContext.unk_13F0 == 3) && (player->swordState == 0 || (player->swordAnimation < SPIN_ATTACK_1H) ||
                                         (player->swordAnimation > BIG_SPIN_2H))) {
        sStickTilt = 0.0f;
        input->cur.stick_x = 0;
        input->cur.stick_y = 0;
        input->cur.button = BTN_R;
    }

    if ((sActionState == ENTORCH2_ATTACK) && (pthis->actor.xzDistToPlayer <= 610.0f) && sZTargetFlag) {
        input->cur.button |= BTN_Z;
    }

    // Updates Dark Link's "controller". The conditional seems to cause him to
    // stop targeting and hold shield if he's been holding it long enough.

    pad54 = input->prev.button ^ input->cur.button;
    input->press.button = input->cur.button & pad54;
    if (CHECK_BTN_ANY(input->cur.button, BTN_R)) {
        input->cur.button = ((sCounterState == 0) && (pthis->swordState == 0)) ? BTN_R : input->cur.button ^ BTN_R;
    }
    input->rel.button = input->prev.button & pad54;
    input->prev.button = input->cur.button & (u16) ~(BTN_A | BTN_B);
    PadUtils_UpdateRelXY(input);

    input->press.stick_x += (s8)(input->cur.stick_x - input->prev.stick_x);
    input->press.stick_y += (s8)(input->cur.stick_y - input->prev.stick_y);

    // Handles Dark Link being damaged

    if ((pthis->actor.colChkInfo.health == 0) && sDeathFlag) {
        pthis->csMode = 0x18;
        pthis->unk_448 = &player->actor;
        pthis->unk_46A = 1;
        sDeathFlag = false;
    }
    if ((pthis->invincibilityTimer == 0) && (pthis->actor.colChkInfo.health != 0) &&
        (pthis->cylinder.base.acFlags & AC_HIT) && !(pthis->stateFlags1 & 0x04000000) &&
        !(pthis->swordQuads[0].base.atFlags & AT_HIT) && !(pthis->swordQuads[1].base.atFlags & AT_HIT)) {

        if (!Actor_ApplyDamage(&pthis->actor)) {
            func_800F5B58();
            pthis->actor.flags &= ~(ACTOR_FLAG_0 | ACTOR_FLAG_2);
            pthis->unk_8A1 = 2;
            pthis->unk_8A4 = 6.0f;
            pthis->unk_8A8 = 6.0f;
            pthis->unk_8A0 = pthis->actor.colChkInfo.damage;
            pthis->unk_8A2 = pthis->actor.yawTowardsPlayer + 0x8000;
            sDeathFlag++;
            sActionState = ENTORCH2_DEATH;
            Enemy_StartFinishingBlow(globalCtx, &pthis->actor);
            Item_DropCollectibleRandom(globalCtx, &pthis->actor, &thisx->world.pos, 0xC0);
            pthis->stateFlags3 &= ~4;
        } else {
            func_800F5ACC(NA_BGM_MINI_BOSS);
            if (pthis->actor.colChkInfo.damageEffect == 1) {
                if (sAlpha == 255) {
                    Actor_SetColorFilter(&pthis->actor, 0, 0xFF, 0, 0x50);
                } else {
                    Actor_SetColorFilter(&pthis->actor, 0, 0xFF, 0x2000, 0x50);
                }
            } else {
                pthis->actor.flags &= ~ACTOR_FLAG_0;
                pthis->unk_8A0 = pthis->actor.colChkInfo.damage;
                pthis->unk_8A1 = 1;
                pthis->unk_8A8 = 6.0f;
                pthis->unk_8A4 = 8.0f;
                pthis->unk_8A2 = pthis->actor.yawTowardsPlayer + 0x8000;
                Actor_SetDropFlag(&pthis->actor, &pthis->cylinder.info, 1);
                pthis->stateFlags3 &= ~4;
                pthis->stateFlags3 |= 1;
                sActionState = ENTORCH2_DAMAGE;
                if (sAlpha == 255) {
                    Actor_SetColorFilter(&pthis->actor, 0x4000, 0xFF, 0, 0xC);
                } else {
                    Actor_SetColorFilter(&pthis->actor, 0x4000, 0xFF, 0x2000, 0xC);
                }
            }
        }
        pthis->actor.colChkInfo.damage = 0;
        pthis->unk_8A0 = 0;
    }

    // Handles being frozen by a deku nut

    if ((pthis->actor.colorFilterTimer == 0) || (pthis->actor.colorFilterParams & 0x4000)) {
        pthis->stateFlags3 &= ~4;
    } else {
        pthis->stateFlags3 |= 4;
        pthis->stateFlags1 &= ~0x04000000;
        pthis->invincibilityTimer = 0;
        input->press.stick_x = input->press.stick_y = 0;
        /*! @bug
         *  Setting cur.button to 0 clears the Z-trigger, causing Dark Link to break his
         *  lock on Link. If he presses A while not locked on, he'll put his sword away.
         *  This clears his held item param permanently and makes him unable to attack.
         */
        input->cur.button = 0;
        input->press.button = 0;
        pthis->linearVelocity = 0.0f;
    }

    globalCtx->playerUpdate(pthis, globalCtx, input);

    /*
     * Handles sword clanks and removes their recoil for both Links. Dark Link staggers
     * if he's had to counter with enough different sword animations in a row.
     */
    if (pthis->linearVelocity == -18.0f) {
        staggerThreshold = (u32)Rand_CenteredFloat(2.0f) + 6;
        if (gSaveContext.health < 0x50) {
            staggerThreshold = (u32)Rand_CenteredFloat(2.0f) + 3;
        }
        if (pthis->actor.xzDistToPlayer > 80.0f) {
            pthis->linearVelocity = 1.2f;
        } else if (pthis->actor.xzDistToPlayer < 70.0f) {
            pthis->linearVelocity = -1.5f;
        } else {
            pthis->linearVelocity = 1.0f;
        }
        if (staggerThreshold < sStaggerCount) {
            pthis->skelAnime.playSpeed *= 0.6f;
            func_800F4190(&pthis->actor.projectedPos, NA_SE_PL_DAMAGE);
            sStaggerTimer = 0;
            sStaggerCount = 0;
        }
    }
    if (player->linearVelocity == -18.0f) {
        if (pthis->actor.xzDistToPlayer > 80.0f) {
            player->linearVelocity = 1.2f;
        } else if (pthis->actor.xzDistToPlayer < 70.0f) {
            player->linearVelocity = -1.5f;
        } else {
            player->linearVelocity = 1.0f;
        }
    }
    /*
     * This ensures Dark Link's counter animation mirrors Link's exactly.
     */
    if ((sCounterState != 0) && (sCounterState == 1)) {
        if (pthis->swordState == 0) {
            sCounterState = 0;
        } else {
            sCounterState = 2;
            pthis->swordState = 1;
            pthis->skelAnime.curFrame = player->skelAnime.curFrame - player->skelAnime.playSpeed;
            pthis->skelAnime.playSpeed = player->skelAnime.playSpeed;
            LinkAnimation_Update(globalCtx, &pthis->skelAnime);
            Collider_ResetQuadAT(globalCtx, &pthis->swordQuads[0].base);
            Collider_ResetQuadAT(globalCtx, &pthis->swordQuads[1].base);
        }
    }
    if (sStaggerTimer != 0) {
        sStaggerTimer--;
        if (sStaggerTimer == 0) {
            sCounterState = 0;
            sStaggerCount = 0;
        }
    }
    if (sDodgeRollState != 0) {
        if (sDodgeRollState == 1) {
            pthis->invincibilityTimer = 20;
        }
        sDodgeRollState = (pthis->invincibilityTimer > 0) ? 2 : 0;
    }
    if (pthis->invincibilityTimer != 0) {
        pthis->cylinder.base.colType = COLTYPE_NONE;
        pthis->cylinder.info.elemType = ELEMTYPE_UNK5;
    } else {
        pthis->cylinder.base.colType = COLTYPE_HIT5;
        pthis->cylinder.info.elemType = ELEMTYPE_UNK1;
    }
    /*
     * Handles the jump movement onto Link's sword. Dark Link doesn't move during the
     * sword jump. Instead, his shape y-offset is increased (see below). Once the sword
     * jump is finished, the offset is added to his position to fix the discrepancy.
     */
    if (sSwordJumpState != 0) {
        Math_SmoothStepToF(&sSwordJumpHeight, 2630.0f, 1.0f, 2000.0f, 0.0f);
        pthis->actor.velocity.y -= 0.6f;
    } else if (sSwordJumpHeight != 0) {
        pthis->actor.world.pos.y += sSwordJumpHeight * 0.01f;
        sSwordJumpHeight = 0;
    }
    if ((sActionState == ENTORCH2_WAIT) || (pthis->invincibilityTimer < 0)) {
        sZTargetFlag = false;
    } else {
        sZTargetFlag = true;
    }
    if (sJumpslashTimer != 0) {
        sJumpslashTimer--;
    }
    pthis->actor.focus.pos = pthis->actor.world.pos;
    pthis->actor.focus.pos.y += 20.0f;
    pthis->actor.shape.yOffset = sSwordJumpHeight;
}

s32 EnTorch2_OverrideLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3f* pos, Vec3s* rot, void* thisx,
                              Gfx** gfx) {
    Player* pthis = (Player*)thisx;

    return func_8008FCC8(globalCtx, limbIndex, dList, pos, rot, &pthis->actor);
}

void EnTorch2_PostLimbDraw(GlobalContext* globalCtx, s32 limbIndex, Gfx** dList, Vec3s* rot, void* thisx, Gfx** gfx) {
    Player* pthis = (Player*)thisx;

    func_80090D20(globalCtx, limbIndex, dList, rot, &pthis->actor);
}

void EnTorch2_Draw(Actor* thisx, GlobalContext* globalCtx2) {
    GlobalContext* globalCtx = globalCtx2;
    Player* pthis = (Player*)thisx;
    s32 pad;

    OPEN_DISPS(globalCtx->state.gfxCtx, "../z_en_torch2.c", 1050);
    func_80093C80(globalCtx);
    func_80093D84(globalCtx->state.gfxCtx);
    if (sAlpha == 255) {
        gDPSetEnvColor(POLY_OPA_DISP++, 255, 0, 0, sAlpha);
        gSPSegment(POLY_OPA_DISP++, 0x0C, D_80116280 + 2);
        func_8002EBCC(&pthis->actor, globalCtx, 0);
        func_8002ED80(&pthis->actor, globalCtx, 0);
        POLY_OPA_DISP = SkelAnime_DrawFlex(globalCtx, pthis->skelAnime.skeleton, pthis->skelAnime.jointTable,
                                           pthis->skelAnime.dListCount, EnTorch2_OverrideLimbDraw, EnTorch2_PostLimbDraw,
                                           pthis, POLY_OPA_DISP);
    } else {
        gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, sAlpha);
        gSPSegment(POLY_XLU_DISP++, 0x0C, D_80116280);
        func_8002EBCC(&pthis->actor, globalCtx, 0);
        func_8002ED80(&pthis->actor, globalCtx, 0);
        POLY_XLU_DISP = SkelAnime_DrawFlex(globalCtx, pthis->skelAnime.skeleton, pthis->skelAnime.jointTable,
                                           pthis->skelAnime.dListCount, EnTorch2_OverrideLimbDraw, EnTorch2_PostLimbDraw,
                                           pthis, POLY_XLU_DISP);
    }
    CLOSE_DISPS(globalCtx->state.gfxCtx, "../z_en_torch2.c", 1114);
}
