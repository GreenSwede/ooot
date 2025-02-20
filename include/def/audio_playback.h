#pragma once
struct SequenceLayer;
struct Note;
struct NotePool;
struct AudioListItem;
struct Drum;
struct Instrument;
struct SoundFontSound;
struct NoteSubAttributes;

Note* Audio_AllocNote(SequenceLayer* layer);
Note* Audio_AllocNoteFromActive(NotePool* pool, SequenceLayer* layer);
Note* Audio_AllocNoteFromDecaying(NotePool* pool, SequenceLayer* layer);
Note* Audio_AllocNoteFromDisabled(NotePool* pool, SequenceLayer* layer);
void Audio_AudioListPushFront(AudioListItem* list, AudioListItem* item);
void Audio_AudioListRemove(AudioListItem* item);
s32 Audio_BuildSyntheticWave(Note* note, SequenceLayer* layer, s32 waveId);
Note* Audio_FindNodeWithPrioLessThan(AudioListItem* list, s32 limit);
Drum* Audio_GetDrum(s32 fontId, s32 drumId);
Instrument* Audio_GetInstrumentInner(s32 fontId, s32 instId);
SoundFontSound* Audio_GetSfx(s32 fontId, s32 sfxId);
void Audio_InitNoteFreeList(void);
void Audio_InitNoteList(AudioListItem* list);
void Audio_InitNoteLists(NotePool* pool);
void Audio_InitNoteSub(Note* note, NoteSubEu* sub, NoteSubAttributes* attrs);
void Audio_InitSyntheticWave(Note* note, SequenceLayer* layer);
SoundFontSound* Audio_InstrumentGetSound(Instrument* instrument, s32 semitone);
void Audio_NoteDisable(Note* note);
void Audio_NoteInit(Note* note);
void Audio_NoteInitAll(void);
void Audio_NoteInitForLayer(Note* note, SequenceLayer* layer);
void Audio_NotePoolClear(NotePool* pool);
void Audio_NotePoolFill(NotePool* pool, s32 count);
void Audio_NoteReleaseAndTakeOwnership(Note* note, SequenceLayer* layer);
void Audio_NoteSetResamplingRate(NoteSubEu* noteSubEu, f32 resamplingRateInput);
void Audio_ProcessNotes(void);
void Audio_SeqLayerDecayRelease(SequenceLayer* layer, s32 target);
void Audio_SeqLayerNoteDecay(SequenceLayer* layer);
void Audio_SeqLayerNoteRelease(SequenceLayer* layer);
s32 Audio_SetFontInstrument(s32 instrumentType, s32 fontId, s32 index, void* value);
void func_800E82C0(Note* note, SequenceLayer* layer);
