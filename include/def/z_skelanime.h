#pragma once
struct GlobalContext;
struct AnimationContext;
struct Vec3s;
struct LinkAnimationHeader;
struct Actor;
struct SkelAnime;
struct AnimationHeader;
struct LegacyAnimationHeader;
struct SkeletonHeader;
struct FlexSkeletonHeader;
struct Vec3f;

void AnimationContext_DisableQueue(GlobalContext* globalCtx);
void AnimationContext_Reset(AnimationContext* animationCtx);
void AnimationContext_SetCopyAll(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src);
void AnimationContext_SetCopyFalse(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src, const u8* copyFlag);
void AnimationContext_SetCopyTrue(GlobalContext* globalCtx, s32 vecCount, Vec3s* dst, Vec3s* src, const u8* copyFlag);
void AnimationContext_SetInterp(GlobalContext* globalCtx, s32 vecCount, Vec3s* base, Vec3s* mod, f32 weight);
void AnimationContext_SetLoadFrame(GlobalContext* globalCtx, LinkAnimationHeader* animation, s32 frame, s32 limbCount, Vec3s* frameTable);
void AnimationContext_SetMoveActor(GlobalContext* globalCtx, Actor* actor, SkelAnime* skelAnime, f32 arg3);
void AnimationContext_SetNextQueue(GlobalContext* globalCtx);
void AnimationContext_Update(GlobalContext* globalCtx, AnimationContext* animationCtx);
void Animation_Change(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames);
void Animation_ChangeImpl(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames, s8 taper);
void Animation_EndLoop(SkelAnime* skelAnime);
s16 Animation_GetLastFrame(void* animation);
s16 Animation_GetLastFrameLegacy(LegacyAnimationHeader* animation);
s16 Animation_GetLength(void* animation);
s16 Animation_GetLengthLegacy(LegacyAnimationHeader* animation);
s16 Animation_GetLimbCountLegacy(LegacyAnimationHeader* animation);
void Animation_MorphToLoop(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames);
void Animation_MorphToPlayOnce(SkelAnime* skelAnime, AnimationHeader* animation, f32 morphFrames);
s32 Animation_OnFrame(SkelAnime* skelAnime, f32 frame);
void Animation_PlayLoop(SkelAnime* skelAnime, AnimationHeader* animation);
void Animation_PlayLoopSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed);
void Animation_PlayOnce(SkelAnime* skelAnime, AnimationHeader* animation);
void Animation_PlayOnceSetSpeed(SkelAnime* skelAnime, AnimationHeader* animation, f32 playSpeed);
void Animation_Reverse(SkelAnime* skelAnime);
void Animation_SetMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, f32 morphFrames);
void LinkAnimation_AnimateFrame(GlobalContext* globalCtx, SkelAnime* skelAnime);
void LinkAnimation_BlendToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation1, f32 frame1, LinkAnimationHeader* animation2, f32 frame2, f32 weight, Vec3s* blendTable);
void LinkAnimation_BlendToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation1, f32 frame1, LinkAnimationHeader* animation2, f32 frame2, f32 weight, Vec3s* blendTable);
void LinkAnimation_Change(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 playSpeed, f32 startFrame, f32 endFrame, u8 mode, f32 morphFrames);
void LinkAnimation_CopyJointToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime);
void LinkAnimation_CopyMorphToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime);
void LinkAnimation_EndLoop(SkelAnime* skelAnime);
void LinkAnimation_InterpJointMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, f32 frame);
void LinkAnimation_LoadToJoint(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 frame);
void LinkAnimation_LoadToMorph(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 frame);
s32 LinkAnimation_OnFrame(SkelAnime* skelAnime, f32 frame);
void LinkAnimation_PlayLoop(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation);
void LinkAnimation_PlayLoopSetSpeed(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 playSpeed);
void LinkAnimation_PlayOnce(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation);
void LinkAnimation_PlayOnceSetSpeed(GlobalContext* globalCtx, SkelAnime* skelAnime, LinkAnimationHeader* animation, f32 playSpeed);
void LinkAnimation_SetUpdateFunction(SkelAnime* skelAnime);
s32 LinkAnimation_Update(GlobalContext* globalCtx, SkelAnime* skelAnime);
void SkelAnime_CopyFrameTable(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src);
void SkelAnime_CopyFrameTableFalse(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag);
void SkelAnime_CopyFrameTableTrue(SkelAnime* skelAnime, Vec3s* dst, Vec3s* src, u8* copyFlag);
Gfx* SkelAnime_Draw(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, void* arg, Gfx* gfx);
Gfx* SkelAnime_DrawFlex(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDraw overrideLimbDraw, PostLimbDraw postLimbDraw, void* arg, Gfx* gfx);
void SkelAnime_DrawFlexLod(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 dListIndex);
void SkelAnime_DrawFlexOpa(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, s32 dListCount, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg);
void SkelAnime_DrawLod(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg, s32 dListIndex);
void SkelAnime_DrawOpa(GlobalContext* globalCtx, void** skeleton, Vec3s* jointTable, OverrideLimbDrawOpa overrideLimbDraw, PostLimbDrawOpa postLimbDraw, void* arg);
void SkelAnime_Free(SkelAnime* skelAnime, GlobalContext* globalCtx);
s32 SkelAnime_GetFrameDataLegacy(LegacyAnimationHeader* animation, s32 frame, Vec3s* frameTable);
s32 SkelAnime_Init(GlobalContext* globalCtx, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount);
s32 SkelAnime_InitFlex(GlobalContext* globalCtx, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount);
void SkelAnime_InitLink(GlobalContext* globalCtx, SkelAnime* skelAnime, FlexSkeletonHeader* skeletonHeaderSeg, LinkAnimationHeader* animation, s32 initFlags, Vec3s* jointTable, Vec3s* morphTable, s32 limbCount);
s32 SkelAnime_InitSkin(GlobalContext* globalCtx, SkelAnime* skelAnime, SkeletonHeader* skeletonHeaderSeg, AnimationHeader* animation);
void SkelAnime_InterpFrameTable(s32 limbCount, Vec3s* dst, Vec3s* start, Vec3s* target, f32 weight);
s32 SkelAnime_Update(SkelAnime* skelAnime);
void SkelAnime_UpdateTranslation(SkelAnime* skelAnime, Vec3f* pos, s16 angle);
