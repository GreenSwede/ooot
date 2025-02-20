#pragma once
struct Vec3f;
struct OcarinaStaff;
struct OcarinaSongInfo;
struct OcarinaNote;

extern u8 gChannelsPerBank[4][7];
extern u8* gFrogsSongPtr;
extern u8 gIsLargeSoundBank[7];
extern u8 gMorphaTransposeTable[16];
extern OcarinaSongInfo gOcarinaSongNotes[];
extern OcarinaNote* gScarecrowCustomSongPtr;
extern u8* gScarecrowSpawnSongPtr;
extern u8 gUsedChannelsPerBank[4][7];

void AudioDebug_ScrPrt(const s8* str, u16 num);
void Audio_ClearSariaBgm(void);
void Audio_ClearSariaBgm2(void);
void Audio_ClearSariaBgmAtPos(Vec3f* pos);
void Audio_Init();
void Audio_InitSound();
OcarinaStaff* Audio_OcaGetDisplayingStaff(void);
OcarinaStaff* Audio_OcaGetPlayingStaff(void);
OcarinaStaff* Audio_OcaGetRecordingStaff(void);
s32 Audio_OcaMemoryGameGenNote(void);
void Audio_OcaMemoryGameStart(u8 minigameIdx);
void Audio_OcaSetInstrument(u8);
void Audio_OcaSetRecordingState(u8);
void Audio_OcaSetSongPlayback(s8 songIdxPlusOne, s8 playbackState);
void Audio_PlayFanfare(u16);
void Audio_PlaySariaBgm(Vec3f* pos, u16 seqId, u16 distMax);
void Audio_PlaySoundIfNotInCutscene(u16 sfxId);
void Audio_PlaySoundIncreasinglyTransposed(Vec3f* pos, s16 sfxId, u8* semitones);
void Audio_PlaySoundRandom(Vec3f* pos, u16 baseSfxId, u8 randLim);
void Audio_PlaySoundRiver(Vec3f* pos, f32 freqScale);
void Audio_PlaySoundTransposed(Vec3f* pos, u16 sfxId, s8 semitone);
void Audio_PlaySoundWaterfall(Vec3f* pos, f32 freqScale);
void Audio_PreNMI();
void Audio_ResetIncreasingTranspose(void);
void Audio_SetBaseFilter(u8);
void Audio_SetBgmEnemyVolume(f32 dist);
void Audio_SetCodeReverb(s8 reverb);
void Audio_SetCutsceneFlag(s8 flag);
void Audio_SetEnvReverb(s8 reverb);
void Audio_SetExtraFilter(u8);
void Audio_SetSequenceMode(u8 seqMode);
void Audio_SetSoundProperties(u8 bankId, u8 entryIdx, u8 channelIdx);
void func_800ECC04(u16);
void func_800EE824(void);
void func_800F3054(void);
void func_800F3F3C(u8);
void func_800F4010(Vec3f* pos, u16 sfxId, f32);
void func_800F4138(Vec3f* pos, u16 sfxId, f32);
void func_800F4190(Vec3f* pos, u16 sfxId);
void func_800F4254(Vec3f* pos, u8 arg1);
void func_800F436C(Vec3f*, u16 sfxId, f32 arg2);
void func_800F4414(Vec3f*, u16 sfxId, f32 arg2);
void func_800F44EC(s8 arg0, s8 arg1);
void func_800F4524(Vec3f* pos, u16 sfxId, s8 arg2);
void func_800F47BC(void);
void func_800F47FC(void);
void func_800F483C(u8 targetVol, u8 volFadeTimer);
void func_800F4870(u8);
void func_800F4A54(u8);
void func_800F4C58(Vec3f* pos, u16 sfxId, u8);
void func_800F4E30(Vec3f* pos, f32);
void func_800F5510(u16 seqId);
void func_800F5550(u16 seqId);
void func_800F5718(void);
void func_800F574C(f32 arg0, u8 arg2);
void func_800F5918(void);
void func_800F595C(u16);
void func_800F59E8(u16);
s32 func_800F5A58(u8);
void func_800F5ACC(u16 bgmID);
void func_800F5B58(void);
void func_800F5BF0(u8 arg0);
void func_800F5C2C(void);
void func_800F5E18(u8 playerIdx, u16 seqId, u8 fadeTimer, s8 arg3, s8 arg4);
void func_800F6268(f32 dist, u16);
void func_800F64E0(u8 arg0);
void func_800F6584(u8 arg0);
void func_800F6700(s8 outputMode);
void func_800F6964(u16);
void func_800F6AB0(u16);
void func_800F6D58(u8, u8, u8);
void func_800F6FB4(u8);
void func_800F7170(void);
void func_800F71BC(s32 arg0);
