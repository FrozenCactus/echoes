#include "MetroidPrime/Enemies/CEmperorIngStage1.hpp"

#include "Collision/CMaterialFilter.hpp"
#include "Kyoto/Animation/CAnimTreeNode.hpp"
#include "Kyoto/Animation/CCECharacterInfo.hpp"
#include "Kyoto/Animation/CPASAnimParmData.hpp"
#include "Kyoto/Animation/CPOINode.hpp"
#include "Kyoto/Audio/CAudioSys.hpp"
#include "Kyoto/Audio/CSfxManager.hpp"
#include "Kyoto/CSimplePool.hpp"
#include "Kyoto/Math/CMath.hpp"
#include "Kyoto/Math/CRelAngle.hpp"
#include "Kyoto/Math/CloseEnough.hpp"
#include "Kyoto/Particles/CElementGen.hpp"
#include "Kyoto/Text/CStringTable.hpp"
#include "MetaRender/CCubeRenderer.hpp"
#include "MetroidPrime/BodyState/CBodyController.hpp"
#include "MetroidPrime/BodyState/CBodyStateCmdMgr.hpp"
#include "MetroidPrime/CActorModelParticles.hpp"
#include "MetroidPrime/CActorParameters.hpp"
#include "MetroidPrime/CAnimData.hpp"
#include "MetroidPrime/CCameraManager.hpp"
#include "MetroidPrime/CCollisionActor.hpp"
#include "MetroidPrime/CCollisionActorManager.hpp"
#include "MetroidPrime/CModelData.hpp"
#include "MetroidPrime/CParticleGenInfoGeneric.hpp"
#include "MetroidPrime/CStateManager.hpp"
#include "MetroidPrime/Collision/CJointCollisionDescription.hpp"
#include "MetroidPrime/Player/CPlayer.hpp"
#include "MetroidPrime/ScriptLoader.hpp"
#include "MetroidPrime/ScriptLoader/SLdrEmperorIngStage1.hpp"
#include "MetroidPrime/ScriptLoaderRel.hpp"
#include "MetroidPrime/ScriptObjects/CScriptEffect.hpp"
#include "MetroidPrime/TCastTo.hpp"
#include "MetroidPrime/Weapons/CBeamInfo.hpp"
#include "MetroidPrime/Weapons/CEnergyProjectile.hpp"
#include "MetroidPrime/Weapons/CImpactVisorEffect.hpp"
#include "MetroidPrime/Weapons/CPlasmaProjectile.hpp"
#include "MetroidPrime/Weapons/CWeapon.hpp"
#include "MetroidPrime/Weapons/CWeaponAssetInfo.hpp"
#include "REL/REL_Setup.h"
#include "rstl/StringExtras.hpp"
#include "rstl/algorithm.hpp"
#include "rstl/math.hpp"

// Guessed name: a pair of joints a tentacle collision box is stretched between.
struct SJointPair {
  const char* from;
  const char* to;
  float scale;
};

static EMaterialTypes skDamageMaterial = kMT_Solid; // Guessed name

static const SJointPair skTentacleJoints[8][5] = {
    {
        {"one_Spine_1_SDK", "one_Spine_2_SDK", 2.f},
        {"one_Spine_2_SDK", "one_Spine_3_SDK", 1.5f},
        {"one_Spine_3_SDK", "one_Spine_4_SDK", 0.75f},
        {"one_Spine_4_SDK", "one_Spine_6_SDK", 0.75f},
        {"one_Spine_6_SDK", "one_Spine_7_SDK", 1.75f},
    },
    {
        {"two_Spine_1_SDK", "two_Spine_2_SDK", 2.f},
        {"two_Spine_2_SDK", "two_Spine_3_SDK", 1.5f},
        {"two_Spine_3_SDK", "two_Spine_4_SDK", 0.75f},
        {"two_Spine_4_SDK", "two_Spine_6_SDK", 0.75f},
        {"two_Spine_6_SDK", "two_Spine_7_SDK", 1.75f},
    },
    {
        {"three_Spine_1_SDK", "three_Spine_2_SDK", 2.f},
        {"three_Spine_2_SDK", "three_Spine_3_SDK", 1.5f},
        {"three_Spine_3_SDK", "three_Spine_4_SDK", 0.75f},
        {"three_Spine_4_SDK", "three_Spine_6_SDK", 0.75f},
        {"three_Spine_6_SDK", "three_Spine_7_SDK", 1.75f},
    },
    {
        {"four_Spine_1_SDK", "four_Spine_2_SDK", 2.f},
        {"four_Spine_2_SDK", "four_Spine_3_SDK", 1.5f},
        {"four_Spine_3_SDK", "four_Spine_4_SDK", 0.75f},
        {"four_Spine_4_SDK", "four_Spine_6_SDK", 0.75f},
        {"four_Spine_6_SDK", "four_Spine_7_SDK", 1.75f},
    },
    {
        {"five_Spine_1_SDK", "five_Spine_2_SDK", 2.f},
        {"five_Spine_2_SDK", "five_Spine_3_SDK", 1.5f},
        {"five_Spine_3_SDK", "five_Spine_4_SDK", 0.75f},
        {"five_Spine_4_SDK", "five_Spine_6_SDK", 0.75f},
        {"five_Spine_6_SDK", "five_Spine_7_SDK", 1.75f},
    },
    {
        {"seven_Spine_1_SDK", "seven_Spine_2_SDK", 2.f},
        {"seven_Spine_2_SDK", "seven_Spine_3_SDK", 1.5f},
        {"seven_Spine_3_SDK", "seven_Spine_4_SDK", 0.75f},
        {"seven_Spine_4_SDK", "seven_Spine_6_SDK", 0.75f},
        {"seven_Spine_6_SDK", "seven_Spine_7_SDK", 1.75f},
    },
    {
        {"six_Spine_1_SDK", "six_Spine_2_SDK", 2.f},
        {"six_Spine_2_SDK", "six_Spine_3_SDK", 1.5f},
        {"six_Spine_3_SDK", "six_Spine_4_SDK", 0.75f},
        {"six_Spine_4_SDK", "six_Spine_6_SDK", 0.75f},
        {"six_Spine_6_SDK", "six_Spine_7_SDK", 1.75f},
    },
    {
        {"eight_Spine_1_SDK", "eight_Spine_2_SDK", 2.f},
        {"eight_Spine_2_SDK", "eight_Spine_3_SDK", 1.5f},
        {"eight_Spine_3_SDK", "eight_Spine_4_SDK", 0.75f},
        {"eight_Spine_4_SDK", "eight_Spine_6_SDK", 0.75f},
        {"eight_Spine_6_SDK", "eight_Spine_7_SDK", 1.75f},
    },
};

static const char* const skPortalLocators[8] = {
    "portal1_LCTR_SDK", "portal2_LCTR_SDK", "portal3_LCTR_SDK", "portal4_LCTR_SDK",
    "portal5_LCTR_SDK", "portal7_LCTR_SDK", "portal6_LCTR_SDK", "portal8_LCTR_SDK",
};

static const char* const skTipSpineLocators[7] = {
    "eight_Spine_1", "eight_Spine_2", "eight_Spine_3", "eight_Spine_4",
    "eight_Spine_5", "eight_Spine_6", "eight_Spine_7",
};

static const char* const skTentacleLocators[8][7] = {
    {"one_Spine_1_SDK", "one_Spine_2_SDK", "one_Spine_3_SDK", "one_Spine_4_SDK", "one_Spine_5_SDK",
     "one_Spine_6_SDK", "one_Spine_7_SDK"},
    {"two_Spine_1_SDK", "two_Spine_2_SDK", "two_Spine_3_SDK", "two_Spine_4_SDK", "two_Spine_5_SDK",
     "two_Spine_6_SDK", "two_Spine_7_SDK"},
    {"three_Spine_1_SDK", "three_Spine_2_SDK", "three_Spine_3_SDK", "three_Spine_4_SDK",
     "three_Spine_5_SDK", "three_Spine_6_SDK", "three_Spine_7_SDK"},
    {"four_Spine_1_SDK", "four_Spine_2_SDK", "four_Spine_3_SDK", "four_Spine_4_SDK",
     "four_Spine_5_SDK", "four_Spine_6_SDK", "four_Spine_7_SDK"},
    {"five_Spine_1_SDK", "five_Spine_2_SDK", "five_Spine_3_SDK", "five_Spine_4_SDK",
     "five_Spine_5_SDK", "five_Spine_6_SDK", "five_Spine_7_SDK"},
    {"six_Spine_1_SDK", "six_Spine_2_SDK", "six_Spine_3_SDK", "six_Spine_4_SDK", "six_Spine_5_SDK",
     "six_Spine_6_SDK", "six_Spine_7_SDK"},
    {"seven_Spine_1_SDK", "seven_Spine_2_SDK", "seven_Spine_3_SDK", "seven_Spine_4_SDK",
     "seven_Spine_5_SDK", "seven_Spine_6_SDK", "seven_Spine_7_SDK"},
    {"eight_Spine_1_SDK", "eight_Spine_2_SDK", "eight_Spine_3_SDK", "eight_Spine_4_SDK",
     "eight_Spine_5_SDK", "eight_Spine_6_SDK", "eight_Spine_7_SDK"},
};

static const u64 skTentacleRenderMasks[8] = {
    0x200, 0x400, 0x100, 0x1000, 0x8, 0x4, 0x10, 0x40,
};

static const char* const skDarkProjectileChargeEffect = "DarkProjectileCharge";
static const char* const skDarkVulnerabilityEffect = "DarkVulnerability";
static const char* const skLightVulnerabilityEffect = "LightVulnerability";

static const int skPhase0Attacks[3] = {0, 0, 3};
static const int skPhase1Attacks[4] = {1, 0, 3, 0};
static const int skPhase2Attacks[4] = {2, 3, 1, 0};

static float sfxTimer = 0.f;

static const char* const skBrainLocator = "brainSpine_scale";
static const char* const skSphereBaseLocator = "sphere_base";

static EMaterialTypes skSphereIncludeProjectile = kMT_Projectile;                   // Guessed name
static EMaterialTypes skSphereIncludePlayer = kMT_Player;                           // Guessed name
static EMaterialTypes skSphereExcludePlatform = kMT_Platform;                       // Guessed name
static EMaterialTypes skSphereExcludeCollisionActor = kMT_CollisionActor;           // Guessed name
static EMaterialTypes skSphereExcludeNoStaticCollision = kMT_NoStaticCollision;     // Guessed name
static EMaterialTypes skSphereExcludeNoPlatformCollision = kMT_NoPlatformCollision; // Guessed name
static EMaterialTypes skTentacleInclude = kMT_Solid;                                // Guessed name
static EMaterialTypes skTentacleExcludeCollisionActor = kMT_CollisionActor;         // Guessed name
static EMaterialTypes skTentacleExcludeCharacter = kMT_Character;                   // Guessed name
static EMaterialTypes skTentacleExcludeAIPassthrough = kMT_AIPassthrough;           // Guessed name
static EMaterialTypes skTentacleExcludePlayer = kMT_Player;                         // Guessed name

static const char* const skBossName = "BossEmperorIngStage1"; // Guessed name

static CPatterned::StateMachine::SStateFunction skStates[] = {
    {"TrackPlayer",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::TrackPlayer)},
    {"BeginHeartStage",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::BeginHeartStage)},
    {"HeartStage",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::HeartStage)},
    {"EndHeartStage",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::EndHeartStage)},
    {"Dead", static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::Dead)},
    {"MeleeAttack",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::MeleeAttack)},
    {"Attack", static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::Attack)},
    {"ProjectileAttack",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::ProjectileAttack)},
    {"WarpAttack",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::WarpAttack)},
    {"TauntAttack",
     static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::TauntAttack)},
    {"Taunt", static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::Taunt)},
    {"JumpRope", static_cast< CPatterned::StateMachine::StateFunc >(&CEmperorIngStage1::JumpRope)},
};

static CPatterned::StateMachine::STriggerFunction skTriggers[] = {
    {"AllTentaclesRetracted", static_cast< CPatterned::StateMachine::TriggerFunc >(
                                  &CEmperorIngStage1::AllTentaclesRetracted)},
    {"HeartStageOver",
     static_cast< CPatterned::StateMachine::TriggerFunc >(&CEmperorIngStage1::HeartStageOver)},
    {"IsDead", static_cast< CPatterned::StateMachine::TriggerFunc >(&CEmperorIngStage1::IsDead)},
    {"ShouldMeleeAttack",
     static_cast< CPatterned::StateMachine::TriggerFunc >(&CEmperorIngStage1::ShouldMeleeAttack)},
    {"ShouldAttack",
     static_cast< CPatterned::StateMachine::TriggerFunc >(&CEmperorIngStage1::ShouldAttack)},
    {"ShouldTaunt",
     static_cast< CPatterned::StateMachine::TriggerFunc >(&CEmperorIngStage1::ShouldTaunt)},
};

static CPatterned::StateMachine::SCodeFunction skCodeFuncs[] = {
    {"BeginSubStage",
     static_cast< CPatterned::StateMachine::CodeFunc >(&CEmperorIngStage1::BeginSubStage)},
    {"ResetTentacles",
     static_cast< CPatterned::StateMachine::CodeFunc >(&CEmperorIngStage1::ResetTentacles)},
    {"ChooseRandomTentacleCount", static_cast< CPatterned::StateMachine::CodeFunc >(
                                      &CEmperorIngStage1::ChooseRandomTentacleCount)},
};

rstl::optional_object< CAABox > CEmperorIngStage1::GetTouchBounds() const {
  return GetBoundingBox();
}

CEntity* LoadEmperorIngStage1(CStateManager& mgr, CInputStream& input, CEntityInfo& info) {
  SLdrEmperorIngStage1 sldrThis;
#include "MetroidPrime/ScriptLoader/SLdrEmperorIngStage1.inc"

  rstl::optional_object< CModelData > modelData(
      LdrToModelData(sldrThis.editorProperties.transform.scale, kInvalidAssetId,
                     sldrThis.patterned.animationInformation, true));
  if (!modelData) {
    return nullptr;
  }

  return rs_new CEmperorIngStage1(mgr.AllocateUniqueId(), sldrThis.editorProperties.name,
                                  LdrToEntityInfo(info, sldrThis.editorProperties),
                                  LdrToTransform4f(sldrThis.editorProperties), *modelData,
                                  LdrToActorParameters(sldrThis.actorInformation),
                                  LdrToPatternedInfo(sldrThis.patterned, nullptr), sldrThis.data);
}

#ifndef MONOLITHIC
static void SetFuncPtrs() {
  static SEmperorIngStage1_FuncPtrs funcPtrs;
  funcPtrs.mLoader = &LoadEmperorIngStage1;
  SetSEmperorIngStage1_FuncPtrs(&funcPtrs);
}

extern "C" void RELMain() { SetFuncPtrs(); }

extern "C" void RELExit() { SetSEmperorIngStage1_FuncPtrs(nullptr); }
#endif

static TUniqueId CreateWarpPortalEffect(CStateManager& mgr, CAssetId asset, const TAreaId& area) {
  const TUniqueId uid = mgr.AllocateUniqueId();
  const CLightParameters lights(false, 1.f, CLightParameters::kST_Zero, 0.f, 0.f, CColor::Black(),
                                true, CLightParameters::kLO_NoShadowCast,
                                CLightParameters::kLR_FourFrames, CVector3f::Zero(), 2, 2, false, 0,
                                true);
  const CGameSplineDesc spline(SLdrSpline(), CMotionSpline::kST_Bezier, 1.f, false);
  CScriptEffect* effect = rs_new CScriptEffect(
      uid, rstl::string_l("WarpPortalEffect"),
      CEntityInfo(area, rstl::vector< SConnection >(), true), CTransform4f::Identity(),
      CVector3f::One(), asset, true, false, true, false, 1.f, 1.f, 0.f, 0.f, false, 1.f, 2.f, 1.f,
      true, true, true, lights, false, spline, false, false, false, CScriptEffect::kRO_Normal);
  mgr.AddObject(effect);
  return uid;
}

CEmperorIngStage1::CEmperorIngStage1(TUniqueId uid, const rstl::string& name,
                                     const CEntityInfo& info, const CTransform4f& xf,
                                     const CModelData& modelData,
                                     const CActorParameters& actorParams,
                                     const CPatternedInfo& patternedInfo,
                                     const SLdrEmperorIngStage1Data& data)
: CPatterned(kPAI_EmperorIngStage1, uid, name, kFT_Zero, info, xf, modelData, patternedInfo,
             kMT_Flyer, kCT_One, kBT_Restricted, actorParams)
, mPhase(0)
, mTentacleCountMode(0)
, mTentacleManager(nullptr)
, mHeartManager(nullptr)
, mSphereManager(nullptr)
, mDestroyedEffectId(kInvalidUniqueId)
, mBrainId(kInvalidUniqueId)
, mHeartEffectId(kInvalidUniqueId)
, mSphereId(kInvalidUniqueId)
, mIdleAnim(-1)
, mBeginHeartStageAnim(-1)
, mEndHeartStageAnim(-1)
, mTurnAngle(0.f)
, mCurrentTurnSpeed(0.f)
, mMaxTurnSpeed(data.maxTurnSpeedNormal)
, mJumpRopeDuration(0.f)
, mHeartExposedTimer(0.f)
, mLastHeartHealth(GetHealthInfo()->GetInitialHP())
, mWarpTimer(0.f)
, mDamageCooldown(0.f)
, mAttackTimer(4.f)
, mTauntTimer(0.f)
, mTauntElapsedTime(0.f)
, mDamageEffectTimer(0.f)
, mTelegraphEffectTimer(0.f)
, mBeamTimeLeft(0.f)
, mHeartBeamTimer(0.f)
, mBeamLifetime(0.f)
, mRetractedFraction(0.f)
, mAttackIndex(0)
, mMeleeActive(false)
, mJumpRopeActive(false)
, mWarpAttackActive(false)
, mWarpTipsEnabled(false)
, mProjectileCharged(false)
, mProjectileChargeTimer(0.f)
, mShockwaveSfx(CSfxHandle())
, mShockwaveId(kInvalidUniqueId)
, mProjectileChargeSfx(CSfxHandle())
, mBeamId(kInvalidUniqueId)
, mBeamSfx(CSfxHandle())
, mBeamTarget(CVector3f::Zero())
, mHealthInfo(LdrToHealthInfo(data.tentacle.health))
, mNormalVulnerability(LdrToDamageVulnerability(data.tentacle.normalVulnerability))
, mWarpVulnerability(LdrToDamageVulnerability(data.tentacle.warpAttackVulnerability))
, mMeleeVulnerability(LdrToDamageVulnerability(data.tentacle.meleeAttackVulnerability))
, mProjectileVulnerability(LdrToDamageVulnerability(data.tentacle.projectileAttackVulnerability))
, mMeleeDamage(LdrToDamageInfo(data.melee.damage))
, mHeartExposedTime(data.heartExposedTime)
, mSecondPhaseHealth(data.unknown_0x905938b8)
, mThirdPhaseHealth(data.unknown_0xb826317a)
, mStayRetractedTime(data.tentacle.stayRetractedTime)
, mProjectileToken(gpSimplePool->GetObj(SObjectTag('WPSC', data.projectile.projectile)))
, mProjectileDamage(LdrToDamageInfo(data.projectile.projectileDamage))
, mProjectileVisorEffect(TLockedToken< CGenDescription >(
      gpSimplePool->GetObj(SObjectTag('PART', data.projectile.projectileVisorEffect))))
, mMaxTurnSpeedNormal(data.maxTurnSpeedNormal)
, mMaxTurnSpeedMelee(data.maxTurnSpeedMelee)
, mJumpRopeTurnSpeed(data.unknown_0xe5a7c358)
, mTurnSpeedAccel(data.turnSpeedAccel)
, mPortalEffect(data.warp.portalEffect)
, mAttackTip(data.warp.attackTip)
, mStabDamage(LdrToDamageInfo(data.warp.stabDamage))
, mTentacleDamagedSound(data.tentacle.tentacleDamagedSound)
, mWarpSound(data.warp.sound)
, mTipSound(data.warp.sound_0xa93f0198)
, mTauntFrequency(data.tauntFrequency)
, mTauntLoopDuration(data.tauntAttack.loopDuration)
, mDestroyFraction(data.tauntAttack.destroyPercentage / 100.f)
, mShockWaveInfo(data.tauntAttack.shockWaveInfo)
, mShockWaveTravelSound(data.tauntAttack.shockWaveTravelSound)
, mBeamProjectileInfo(data.beamAttack.beamInfo.weaponSystem,
                      LdrToDamageInfo(data.beamAttack.damage))
, mBeamInfo(data.beamAttack.beamInfo)
, mBeamSound(data.beamAttack.sound)
, mAttackIntervalMin(data.attackIntervalMin)
, mAttackIntervalMax(data.attackIntervalMax)
, mHeartDamageSound(data.heartDamageSound) {
  SetDrawShadow(false);
  mKnockBackController.EnableAllAnimReactions(false);
}

CEmperorIngStage1::~CEmperorIngStage1() {}

void CEmperorIngStage1::SetupStateMachine(CStateManager& mgr) {
  StateMachine* stateMachine = mStateMachine.get();
  CPatterned::SetupStateMachine(mgr);
  stateMachine->SetTriggerFunctions(skTriggers, ARRAY_SIZE(skTriggers));
  stateMachine->SetStateFunctions(skStates, ARRAY_SIZE(skStates));
  stateMachine->SetCodeFunctions(skCodeFuncs, ARRAY_SIZE(skCodeFuncs));
}

void CEmperorIngStage1::DoUserAnimEvent(CStateManager& mgr, const CInt32POINode& node,
                                        EUserEventType type, float dt) {
  switch (type) {
  case kUE_BreakLockOn:
    for (int i = 0; i < GetTentacleCount(); ++i) {
      if (CCollisionActor* actor = GetTentacleCollisionActor(mgr, i, 4)) {
        mgr.GetPlayer(0)->SetOrbitRequestForTarget(actor->GetUniqueId(),
                                                   CPlayer::kOR_ActivateOrbitSource, mgr);
      }
    }
    break;
  case kUE_Projectile:
    mProjectileChargeTimer = 0.f;
    if (mProjectileCharged) {
      const CVector3f origin =
          GetTransform() * GetScaledLocatorTransform(node.GetLocatorName()).GetTranslation();
      const CVector3f playerPos = mgr.GetPlayer(0)->GetTranslation();
      const CTransform4f xf = CTransform4f::LookAt(origin, playerPos, CVector3f::Up());
      CEnergyProjectile* projectile = rs_new CEnergyProjectile(
          true, mProjectileToken, static_cast< EWeaponType >(mProjectileDamage.GetWeaponMode1()),
          xf, kMT_Character, mProjectileDamage, mgr.AllocateUniqueId(), GetCurrentAreaId(),
          GetUniqueId(), kInvalidUniqueId, 0, false, CVector3f::One(),
          CImpactVisorEffect::ParticleEffect(mProjectileVisorEffect,
                                             CSfxManager::kInternalInvalidSfxId, true),
          false, true, false, 1.f, 4.f, 4.f);
      mgr.AddObject(projectile);
    }
    break;
  case kUE_EventStart:
    if (mWarpAttackActive) {
      const CTransform4f xf = GetTransform() * GetScaledLocatorTransform(node.GetLocatorName());
      const CVector3f pos = xf.GetTranslation();
      for (int i = 0; i < 8; ++i) {
        if (CStringExtras::CompareCaseInsensitive(rstl::string_l(skPortalLocators[i]),
                                                  node.GetLocatorName()) == 0) {
          if (i < GetTentacleCount()) {
            mTentacles[i].ActivatePortal(mgr, pos,
                                         -CUnitVector3f(xf.GetColumn(kDZ), CUnitVector3f::kN_No));
          }
          break;
        }
      }
    }
    break;
  case kUE_EventStop: {
    const CTransform4f xf = GetTransform() * GetScaledLocatorTransform(node.GetLocatorName());
    for (int i = 0; i < 8; ++i) {
      if (CStringExtras::CompareCaseInsensitive(rstl::string_l(skPortalLocators[i]),
                                                node.GetLocatorName()) == 0) {
        if (i < GetTentacleCount()) {
          mTentacles[i].DeactivatePortal(mgr);
        }
        break;
      }
    }
    break;
  }
  case kUE_GenerateEnd:
    mWarpTipsEnabled = true;
    break;
  case kUE_BeginAction: {
    mShockwaveId = mgr.AllocateUniqueId();
    const CTransform4f xf = GetBrainActor(mgr)->GetTransform();
    mgr.AddObject(
        rs_new CShockWave(mShockwaveId, rstl::string_l("Shockwave"),
                          CEntityInfo(GetCurrentAreaId(), rstl::vector< SConnection >(), true), xf,
                          GetUniqueId(), mShockWaveInfo, 4.f, 0.4f));
    mShockwaveSfx =
        CSfxManager::AddEmitter(mShockWaveTravelSound, GetTranslation(), GetCurrentAreaId().Value(),
                                true, false, CSfxManager::kMedPriority);
    break;
  }
  default:
    CPatterned::DoUserAnimEvent(mgr, node, type, dt);
    break;
  }
}

void CEmperorIngStage1::ProcessSoundEvent(int sfxId, float weight, int flags, float fallOff,
                                          float maxDist, const CSegId& locator, ushort pitchStart,
                                          ushort pitchEnd, float pitchDuration, uchar minVol,
                                          uchar maxVol, float distanceSquared,
                                          const CVector3f& position, int aid, CStateManager& mgr,
                                          bool translateId) {
  const int index = SegIdToTentacleIndex(locator);
  if (index == -1 || mTentacles[index].mHit) {
    CActor::ProcessSoundEvent(sfxId, weight, flags, fallOff, maxDist, locator, pitchStart, pitchEnd,
                              pitchDuration, minVol, maxVol, distanceSquared, position, aid, mgr,
                              translateId);
  }
}

void CEmperorIngStage1::AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) {
  CPatterned::AcceptScriptMsg(mgr, msg);
  switch (msg.GetMessage()) {
  case kSM_Create:
    BodyController()->Activate(mgr, pas::kAS_Invalid);
    break;
  case kSM_Delete:
    DestroyCollisionActors(mgr);
    break;
  case kSM_AreaLoaded:
    mDestroyedEffectId = FindConnectedObject(mgr, kSS_InternalState3, kSM_Activate);
    mBrainId = FindConnectedObject(mgr, kSS_Connect, kSM_Attach);
    mHeartEffectId = FindConnectedObject(mgr, kSS_Connect, kSM_Activate);
    FindAnimations(mgr);
    SetupCollisionActors(mgr);
    SetupTentacles(mgr);
    break;
  case kSM_Activate:
    mgr.SetBossParams(GetUniqueId(), GetHealthInfo()->GetInitialHP(),
                      gpStringTable->GetStringIndex(skBossName));
    break;
  default:
    break;
  }
}

void CEmperorIngStage1::TrackPlayer(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 1);
    SetVulnerabilityMode(mgr, kVM_Normal);
    break;
  case kStateMsg_Update:
    BodyController()->CommandMgr().DeliverCmd(
        CBCLocomotionCmd(CVector3f::Zero(), CVector3f::Zero(), 1.f));
    TurnTowardPlayer(mgr);
    break;
  case kStateMsg_Deactivate:
    break;
  }
}

void CEmperorIngStage1::TurnTowardPlayer(CStateManager& mgr) {
  CActor* brain = GetBrainActor(mgr);
  const CTransform4f baseXf =
      brain->GetTransform() *
      brain->GetModelData()->GetScaledLocatorTransform(rstl::string_l(skSphereBaseLocator));
  const CVector3f base = baseXf.GetTranslation();
  CVector3f dir = mgr.GetPlayer(0)->GetTranslation() - base;
  dir.SetZ(0.f);
  dir.Normalize();
  SetTurnTarget(mgr, dir);
}

void CEmperorIngStage1::ForceExtendTentacles() {
  for (int i = 0; i < GetTentacleCount(); ++i) {
    mTentacles[i].mState = STentacle::kS_Extended;
  }
}

void CEmperorIngStage1::BeginHeartStage(CStateManager& mgr, EStateMsg msg, float dt) {
  CActor* sphere = GetSphereActor(mgr);
  CActor* heartEffect = GetHeartEffectActor(mgr);
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 0);
    SetVulnerabilityMode(mgr, kVM_Immune);
    SetTentacleCollisionActive(mgr, 8, true);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    sphere->ModelData()->AnimationData()->SetAnimation(
        CAnimPlaybackParms(mBeginHeartStageAnim, -1, 1.f, true), false);
    heartEffect->AcceptScriptMsg(mgr, CScriptMsg(GetUniqueId(), mHeartEffectId, kSM_Activate));
    EnableTentacles(false);
    SendScriptMsgs(kSS_InternalState4, mgr, kSM_None);
    break;
  case kStateMsg_Update:
    if (!sphere->ModelData()->AnimationData()->IsAnimTimeRemaining(dt,
                                                                   rstl::string_l("Whole Body"))) {
      mAnimationState.SetState(CAnimationState::kAS_Over);
    }
    break;
  case kStateMsg_Deactivate:
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    sphere->SetActive(false);
    SetTentacleCollisionActive(mgr, 9, false);
    break;
  }
}

void CEmperorIngStage1::HeartStage(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate:
    mHeartExposedTimer = mHeartExposedTime;
    mHeartBeamTimer = 3.f;
    BodyController()->SetLocomotionType(pas::kLT_Internal9);
    break;
  case kStateMsg_Update:
    mHeartBeamTimer -= dt;
    if (mHeartBeamTimer < 0.f && CreateBeam(dt, mgr)) {
      mBeamLifetime = 3.75f;
    }
    mBeamLifetime -= dt;
    UpdateBeam(dt, mgr);
    if (mBeamId != kInvalidUniqueId && mBeamLifetime < 0.f) {
      DeleteBeam(mgr);
      mHeartBeamTimer = 0.5f;
    }
    mHeartExposedTimer -= dt;
    break;
  case kStateMsg_Deactivate:
    BodyController()->SetLocomotionType(pas::kLT_Relaxed);
    SetTentacleCollisionActive(mgr, 8, false);
    DeleteBeam(mgr);
    break;
  }
}

void CEmperorIngStage1::EndHeartStage(CStateManager& mgr, EStateMsg msg, float dt) {
  CActor* sphere = GetSphereActor(mgr);
  CActor* heartEffect = GetHeartEffectActor(mgr);
  switch (msg) {
  case kStateMsg_Activate:
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    SetTentacleCollisionActive(mgr, 9, true);
    sphere->SetActive(true);
    heartEffect->AcceptScriptMsg(mgr, CScriptMsg(GetUniqueId(), mHeartEffectId, kSM_Deactivate));
    sphere->ModelData()->AnimationData()->SetAnimation(
        CAnimPlaybackParms(mEndHeartStageAnim, -1, 1.f, true), false);
    SendScriptMsgs(kSS_InternalState5, mgr, kSM_None);
    break;
  case kStateMsg_Update:
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_Generate)) {
      switch (mTentacleCountMode) {
      case 0:
        BodyController()->CommandMgr().DeliverCmd(
            CBCGenerateCmd(pas::kGType_Zero, CVector3f::Zero(), false, false));
        break;
      case 1:
        BodyController()->CommandMgr().DeliverCmd(
            CBCGenerateCmd(pas::kGType_Two, CVector3f::Zero(), false, false));
        break;
      default:
        BodyController()->CommandMgr().DeliverCmd(
            CBCGenerateCmd(pas::kGType_One, CVector3f::Zero(), false, false));
        break;
      }
    } else {
      ForceExtendTentacles();
    }
    if (!sphere->ModelData()->AnimationData()->IsAnimTimeRemaining(dt,
                                                                   rstl::string_l("Whole Body"))) {
      mAnimationState.SetState(CAnimationState::kAS_Over);
    }
    break;
  case kStateMsg_Deactivate:
    sphere->ModelData()->AnimationData()->SetAnimation(CAnimPlaybackParms(mIdleAnim, -1, 1.f, true),
                                                       false);
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    EnableTentacles(true);
    ResetAttackTimer(mgr);
    SetVulnerabilityMode(mgr, kVM_Normal);
    break;
  }
}

void CEmperorIngStage1::Dead(CStateManager& mgr, EStateMsg msg, float dt) {
  if (msg == kStateMsg_Activate) {
    SendScriptMsgs(kSS_Dead, mgr, kSM_None);
  }
}

void CEmperorIngStage1::MeleeAttack(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 0);
    SetVulnerabilityMode(mgr, kVM_Melee);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    mMeleeActive = true;
    mMaxTurnSpeed = mMaxTurnSpeedMelee;
    break;
  case kStateMsg_Update:
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_LoopAttack)) {
      BodyController()->CommandMgr().DeliverCmd(CBCLoopAttackCmd(pas::kLAT_Four, false));
    } else {
      if (mStateMachine->GetTime() < 0.5f) {
        mTurnAngle = -(M_PIF / 180.f * mMaxTurnSpeedMelee);
      } else {
        mTurnAngle = M_PIF / 180.f * mMaxTurnSpeedMelee;
      }
      if (mStateMachine->GetTime() > 3.f && !ShouldMeleeAttack(mgr, CTriggerData(0.f))) {
        mAnimationState.SetState(CAnimationState::kAS_Over);
      }
    }
    break;
  case kStateMsg_Deactivate:
    BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_ExitState));
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    ResetAttackTimer(mgr);
    mMeleeActive = false;
    mMaxTurnSpeed = mMaxTurnSpeedNormal;
    break;
  }
}

void CEmperorIngStage1::Attack(CStateManager& mgr, EStateMsg msg, float dt) {
  const int* attacks;
  int count;
  switch (mPhase) {
  case 0:
    attacks = skPhase0Attacks;
    count = 3;
    break;
  case 1:
    attacks = skPhase1Attacks;
    count = 4;
    break;
  default:
    attacks = skPhase2Attacks;
    count = 4;
    break;
  }
  TurnToSphereForward(mgr);
  if (msg == kStateMsg_Activate) {
    EnableTentacles(false);
  }
  if (mAttackIndex >= count) {
    mAttackIndex = 0;
  }
  switch (attacks[mAttackIndex]) {
  case 0:
    ProjectileAttack(mgr, msg, dt);
    break;
  case 1:
    WarpAttack(mgr, msg, dt);
    break;
  case 2:
    TauntAttack(mgr, msg, dt);
    break;
  case 3:
    JumpRope(mgr, msg, dt);
    break;
  }
  if (msg == kStateMsg_Deactivate) {
    ResetAttackTimer(mgr);
    ++mAttackIndex;
    EnableTentacles(true);
  }
}

static void SetExternalParamRecursive(CElementGen* gen, uint index, float value) {
  if (gen) {
    gen->SetExternalParam(index, value);
    for (int i = 0; i < gen->GetNumSpawnedParticleSystems(); ++i) {
      SetExternalParamRecursive(static_cast< CElementGen* >(gen->SpawnedParticleSystem(i)), index,
                                value);
    }
  }
}

void CEmperorIngStage1::ProjectileAttack(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate: {
    SendInternalState(mgr, 0);
    SetVulnerabilityMode(mgr, kVM_Dark);
    mProjectileCharged = false;
    mRetractedFraction =
        static_cast< float >(CountRetractedTentacles()) / static_cast< float >(GetTentacleCount());
    ushort sfx;
    if (mRetractedFraction < 0.25f) {
      sfx = 0x2895;
    } else if (mRetractedFraction < 0.5f) {
      sfx = 0x2896;
    } else if (mRetractedFraction < 0.75f) {
      sfx = 0x2897;
    } else {
      sfx = 0x2898;
    }
    CAudioSys::C3DEmitterParmData parms(1000.f, 1.f, 1, 0x7f, 0x14);
    parms.mPos = GetTranslation();
    parms.mDir = CVector3f::Zero();
    parms.mSfxId = sfx;
    mProjectileChargeSfx = CSfxManager::AddEmitter(parms, GetCurrentAreaId().Value(), true, false,
                                                   CSfxManager::kMedPriority);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    break;
  }
  case kStateMsg_Update: {
    mProjectileChargeTimer = 1.f;
    const float duration = 4.f * mRetractedFraction + 2.f;
    const float t = mStateMachine->GetTime() / duration;
    if (t >= 0.9999999f) {
      mProjectileCharged = true;
    }
    if (CParticleGenInfo* info =
            AnimationData()->GetFirstParticleEffect(rstl::string_l(skDarkProjectileChargeEffect))) {
      CElementGen* gen = static_cast< CElementGen* >(
          static_cast< CParticleGenInfoGeneric* >(info)->GetParticleSystem().GetPtr());
      SetExternalParamRecursive(gen, 0, t);
    }
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_LoopAttack)) {
      BodyController()->CommandMgr().DeliverCmd(CBCLoopAttackCmd(pas::kLAT_Zero, false));
    } else {
      TurnTowardPlayer(mgr);
      if (mStateMachine->GetTime() > duration) {
        mAnimationState.SetState(CAnimationState::kAS_Over);
      }
    }
    break;
  }
  case kStateMsg_Deactivate:
    BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_ExitState));
    if (!mProjectileCharged) {
      mProjectileChargeTimer = 0.f;
    }
    if (mProjectileChargeSfx && CSfxManager::IsPlaying(mProjectileChargeSfx)) {
      CSfxManager::RemoveEmitter(mProjectileChargeSfx);
      mProjectileChargeSfx = CSfxHandle();
    }
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    break;
  }
}

void CEmperorIngStage1::WarpAttack(CStateManager& mgr, EStateMsg msg, float dt) {
  static int sRemainingTips = -1;
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 2);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    SetVulnerabilityMode(mgr, kVM_Light);
    mWarpTimer = 0.f;
    mWarpTipsEnabled = false;
    mWarpAttackActive = true;
    sRemainingTips = -1;
    break;
  case kStateMsg_Update:
    if (mStateMachine->GetTime() > 4.f) {
      SetVulnerabilityMode(mgr, kVM_Immune);
      SetAllCollisionMaterials(mgr, false);
      if (sRemainingTips == -1) {
        sRemainingTips = GetTentacleCount() - CountRetractedTentacles();
      }
    }
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_LoopAttack)) {
      BodyController()->CommandMgr().DeliverCmd(CBCLoopAttackCmd(pas::kLAT_One, true));
    } else if (mWarpTipsEnabled) {
      mWarpTimer -= dt;
      while (mWarpTimer < 0.f && sRemainingTips > 0) {
        --sRemainingTips;
        mWarpTimer = 0.75f;
        const CModelData modelData(CAnimRes(mAttackTip.ancs, mAttackTip.character_index,
                                            CVector3f::One(), mAttackTip.initial_anim, false));
        const CPlayer* player = mgr.GetPlayer(0);
        const CTransform4f playerXf = player->GetTransform();
        const CTransform4f camXf = mgr.GetCameraManager(0)->GetCurrentCameraTransform(mgr, false);
        CVector3f pos = CVector3f::Zero();
        CVector3f target = CVector3f::Zero();
        if (player->GetMorphballTransitionState() == CPlayer::kMS_Unmorphed) {
          const float upScale = mgr.Random()->Range(1.f, 5.f);
          const CVector3f up = upScale * playerXf.GetColumn(kDZ);
          const float rightScale = mgr.Random()->Range(-10.f, 10.f);
          const CVector3f right = rightScale * playerXf.GetColumn(kDX);
          const CVector3f forward = 10.f * playerXf.GetColumn(kDY);
          pos = player->GetEyePosition() + forward + right + up;
          target = player->GetEyePosition();
        } else {
          pos = player->GetAimPosition(mgr, 0.75f);
          const CRayCastResult result = mgr.RayStaticIntersection(
              pos + CVector3f::Up(), CVector3f::Down(), 50.f, CMaterialFilter());
          if (result.IsValid()) {
            pos = result.GetPoint();
          }
          target = pos + CVector3f::Up();
        }
        const CTransform4f xf = CTransform4f::LookAt(pos, target, CVector3f::Up());
        mgr.AddObject(rs_new CEmperorIngStage1Tip(
            mgr.AllocateUniqueId(), rstl::string_l("Attack Tip"),
            CEntityInfo(GetCurrentAreaId(), rstl::vector< SConnection >(), true), xf, modelData,
            CActorParameters::None(), mPortalEffect, mStabDamage, mWarpSound, mTipSound));
      }
      if (mWarpTimer < 0.f && sRemainingTips == 0 && mStateMachine->GetTime() > 5.f) {
        mAnimationState.SetState(CAnimationState::kAS_Over);
      }
    }
    break;
  case kStateMsg_Deactivate:
    BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_ExitState));
    mWarpAttackActive = false;
    SetAllCollisionMaterials(mgr, true);
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    break;
  }
}

void CEmperorIngStage1::TauntAttack(CStateManager& mgr, EStateMsg msg, float dt) {
  static int sRetractedBefore = 0;
  static int sRetractedNow = 0;
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 0);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    SetVulnerabilityMode(mgr, kVM_Normal);
    sRetractedBefore = GetTentacleCount() - CountRetractedTentacles();
    mTauntElapsedTime = 0.f;
    sRetractedNow = CountRetractedTentacles();
    break;
  case kStateMsg_Update:
    TurnTowardPlayer(mgr);
    mTauntElapsedTime += dt;
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_LoopAttack)) {
      BodyController()->CommandMgr().DeliverCmd(CBCLoopAttackCmd(pas::kLAT_Two, false));
    } else if (mTauntElapsedTime > mTauntLoopDuration) {
      BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_ExitState));
    }
    break;
  case kStateMsg_Deactivate:
    BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_AbortScripted));
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    break;
  }
}

void CEmperorIngStage1::Taunt(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 0);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    break;
  case kStateMsg_Update:
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_Taunt)) {
      BodyController()->CommandMgr().DeliverCmd(CBCTauntCmd(pas::kTT_Zero));
    }
    TurnTowardPlayer(mgr);
    break;
  case kStateMsg_Deactivate:
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    mTauntTimer = 0.f;
    break;
  }
}

void CEmperorIngStage1::JumpRope(CStateManager& mgr, EStateMsg msg, float dt) {
  switch (msg) {
  case kStateMsg_Activate:
    SendInternalState(mgr, 0);
    SetVulnerabilityMode(mgr, kVM_Melee);
    SetAllCollisionMaterials(mgr, false);
    mAnimationState.SetState(CAnimationState::kAS_Ready);
    mJumpRopeActive = true;
    mMaxTurnSpeed = mJumpRopeTurnSpeed;
    switch (mPhase) {
    case 0:
      mJumpRopeDuration = 3.5f;
      break;
    case 1:
      mJumpRopeDuration = 5.f;
      break;
    default:
      mJumpRopeDuration = 6.5f;
      break;
    }
    break;
  case kStateMsg_Update:
    if (mAnimationState.CanIssueCommand(*BodyController(), pas::kAS_LoopAttack)) {
      BodyController()->CommandMgr().DeliverCmd(CBCLoopAttackCmd(pas::kLAT_Three, false));
    } else {
      if (mStateMachine->GetTime() < 1.f) {
        mTurnAngle = -(M_PIF / 180.f * mJumpRopeTurnSpeed);
      } else {
        mTurnAngle = M_PIF / 180.f * mJumpRopeTurnSpeed;
      }
      if (mStateMachine->GetTime() > mJumpRopeDuration) {
        mAnimationState.SetState(CAnimationState::kAS_Over);
      }
    }
    break;
  case kStateMsg_Deactivate:
    BodyController()->CommandMgr().DeliverCmd(CBodyStateCmd(kBSC_ExitState));
    mAnimationState.SetState(CAnimationState::kAS_NotReady);
    mJumpRopeActive = false;
    SetAllCollisionMaterials(mgr, true);
    mMaxTurnSpeed = mMaxTurnSpeedNormal;
    ResetAttackTimer(mgr);
    break;
  }
}

void CEmperorIngStage1::Think(float dt, CStateManager& mgr) {
  if (!GetActive()) {
    return;
  }
  UpdateTentacles(dt, mgr);
  CPatterned::Think(dt, mgr);
  mAttackTimer -= dt;
  mTauntTimer += dt;
  UpdateHealth(dt, mgr);
  UpdateBrain(dt, mgr);
  UpdateCollisionActors(dt, mgr);
  UpdateEffects(dt, mgr);
  mProjectileChargeTimer -= dt;
  AnimationData()->SetEffectState(rstl::string_l(skDarkProjectileChargeEffect),
                                  mProjectileChargeTimer > 0.f, mgr);
  UpdateMeleeDamage(dt, mgr);
  UpdateJumpRopeDamage(dt, mgr);
  if (mShockwaveSfx && mShockwaveId != kInvalidUniqueId && CSfxManager::IsPlaying(mShockwaveSfx)) {
    if (const CShockWave* wave =
            static_cast< const CShockWave* >(mgr.GetObjectById(mShockwaveId))) {
      CVector3f dir = mgr.GetPlayer(0)->GetTranslation() - wave->GetTranslation();
      dir.SetZ(0.f);
      if (dir.CanBeNormalized()) {
        dir.Normalize();
        const CVector3f pos = wave->GetTranslation() + wave->GetRadius() * dir;
        CSfxManager::UpdateEmitter(mShockwaveSfx, pos, dir, 0xff);
      }
    }
  }
}

void CEmperorIngStage1::Render(const CStateManager& mgr) const {
  uint mask = 0;
  uint target = 0;
  if (mDrawParticles) {
    mgr.GetCharacterRenderMaskAndTarget(mask, target);
  }
  RenderSystemsToBeDrawnFirst(mgr, mask, target);
  if (GetPointGeneratorParticles()) {
    mgr.SetupParticleHook(*this);
  }

  const CModelData::EWhichModel which = CModelData::GetRenderingModel(mgr);
  const int alpha = GetRenderAlphaBufferAlpha(mgr);
  if (alpha != -1) {
    gpRender->SetDestinationAlpha(alpha);
  }

  {
    rstl::reserved_vector< CModelFlags, 8 > flags;
    rstl::reserved_vector< CColor, 8 > colors;
    rstl::reserved_vector< CPlane, 8 > planes;
    u64 masks[8];
    for (int i = 0; i < 8; ++i) {
      const STentacle& tentacle = mTentacles[i];
      flags.push_back(tentacle.GetModelFlags());
      colors.push_back(tentacle.GetDamageColor());
      planes.push_back(tentacle.GetPortalPlane());
      masks[i] = tentacle.GetRenderMask();
    }
    GetModelData()->RenderModelMultipleTimesWithFlags(which, GetTransform(), GetActorLights(),
                                                      flags.data(), masks, colors.data(),
                                                      planes.data(), 8);
    if (alpha != -1) {
      gpRender->DisableDestinationAlpha();
    }
  }
  if (GetPointGeneratorParticles()) {
    CSkinnedModel::ClearPointGeneratorFunc();
    mgr.GetActorModelParticles()->Render(mgr, *this);
  }
  RenderSystemsToBeDrawnLast(mgr, mask, target);
}

void CEmperorIngStage1::PreRender(CStateManager& mgr) { CPatterned::PreRender(mgr); }

void CEmperorIngStage1::UpdateHealth(float dt, CStateManager& mgr) {
  CCollisionActor* heart = GetHeartCollisionActor(mgr);
  const float hp = heart->HealthInfo()->GetHP();
  if (hp < mLastHeartHealth) {
    mLastHeartHealth = hp;
    mDamageEffectTimer = 1.f / 60.f;
    PlayCustomSound(heart->GetTranslation(), CVector3f::Zero(), mHeartDamageSound, false);
  }
  switch (mPhase) {
  case 0:
    if (hp < mSecondPhaseHealth) {
      mPhase = 1;
    }
    break;
  case 1:
    if (hp < mThirdPhaseHealth) {
      mPhase = 2;
    }
    break;
  default:
    break;
  }
  GetHeartCollisionActor(mgr)->HealthInfo()->SetHP(hp);
  HealthInfo()->SetHP(hp);
}

void CEmperorIngStage1::UpdateBrain(float dt, CStateManager& mgr) {
  CActor* brain = GetBrainActor(mgr);
  CActor* sphere = GetSphereActor(mgr);
  CActor* heartEffect = GetHeartEffectActor(mgr);

  const CTransform4f heartXf =
      brain->GetTransform() *
      brain->GetModelData()->GetScaledLocatorTransform(rstl::string_l(skBrainLocator));
  heartEffect->SetTranslation(heartXf.GetTranslation());

  const CTransform4f baseXf =
      brain->GetTransform() *
      brain->GetModelData()->GetScaledLocatorTransform(rstl::string_l(skSphereBaseLocator));
  const CVector3f base = baseXf.GetTranslation();

  const float accel = (M_PIF / 180.f) * mTurnSpeedAccel;
  const float speedDelta = mTurnAngle - mCurrentTurnSpeed;
  const float newSpeed = dt * CMath::Clamp(-accel, speedDelta, accel) + mCurrentTurnSpeed;
  const float maxSpeed = (M_PIF / 180.f) * mMaxTurnSpeed;
  mCurrentTurnSpeed = CMath::Clamp(-maxSpeed, newSpeed, maxSpeed);

  CVector3f forward = sphere->GetTransform().GetColumn(kDY);
  forward.SetZ(0.f);
  forward.Normalize();
  const CVector3f direction =
      CTransform4f::RotateZ(CRelAngle::FromRadians(dt * mCurrentTurnSpeed)) * forward;
  sphere->SetTransform(CTransform4f::LookAt(base, base + direction, baseXf.GetColumn(kDZ)));

  const CTransform4f& sphereXf = sphere->GetTransform();
  SetTransform(sphereXf * sphere->GetModelData()->GetScaledLocatorTransform(CSegId(1)) *
               CTransform4f::Translate(1.5f * sphereXf.GetColumn(kDZ)));
}

void CEmperorIngStage1::UpdateEffects(float dt, CStateManager& mgr) {
  CActor* brain = GetBrainActor(mgr);
  if (!brain) {
    return;
  }
  {
    if (CParticleGenInfo* info =
            brain->ModelData()->AnimationData()->GetFirstParticleEffect(rstl::string_l("Damage"))) {
      CElementGen* gen = static_cast< CElementGen* >(
          static_cast< CParticleGenInfoGeneric* >(info)->GetParticleSystem().GetPtr());
      SetExternalParamRecursive(gen, 0, mDamageEffectTimer > 0.f ? 1.f : 0.f);
    }
    brain->ModelData()->AnimationData()->SetEffectState(rstl::string_l("Damage"), true, mgr);
    mDamageEffectTimer -= dt;
  }
  {
    if (CParticleGenInfo* info = brain->ModelData()->AnimationData()->GetFirstParticleEffect(
            rstl::string_l("Telegraph"))) {
      CElementGen* gen = static_cast< CElementGen* >(
          static_cast< CParticleGenInfoGeneric* >(info)->GetParticleSystem().GetPtr());
      SetExternalParamRecursive(gen, 0, mTelegraphEffectTimer > 0.f ? 1.f : 0.f);
    }
    brain->ModelData()->AnimationData()->SetEffectState(rstl::string_l("Telegraph"), true, mgr);
    mTelegraphEffectTimer -= dt;
  }
}

bool CEmperorIngStage1::AllTentaclesRetracted(CStateManager&, const CTriggerData&) const {
  const int count = mTentacles.size();
  for (int i = 0; i < count; ++i) {
    if (mTentacles[i].mState != STentacle::kS_Retracted) {
      return false;
    }
  }
  return true;
}

bool CEmperorIngStage1::HeartStageOver(CStateManager&, const CTriggerData&) const {
  return mHeartExposedTimer <= 0.f;
}

bool CEmperorIngStage1::IsDead(CStateManager&, const CTriggerData&) const {
  return GetHealthInfo()->GetHP() <= 0.f;
}

bool CEmperorIngStage1::ShouldMeleeAttack(CStateManager& mgr, const CTriggerData&) const {
  CVector3f delta = GetTranslation() - mgr.GetPlayer(0)->GetTranslation();
  delta.SetZ(0.f);
  return delta.MagSquared() < 900.f;
}

bool CEmperorIngStage1::ShouldAttack(CStateManager&, const CTriggerData&) const {
  return mAttackTimer < 0.f;
}

bool CEmperorIngStage1::ShouldTaunt(CStateManager&, const CTriggerData&) const {
  return mTauntTimer > mTauntFrequency;
}

void CEmperorIngStage1::BeginSubStage(CStateManager&, float) {}

void CEmperorIngStage1::ResetTentacles(CStateManager& mgr, float) {
  const int tentacleCount = GetTentacleCount();
  for (int i = 0; i < tentacleCount; ++i) {
    mTentacles[i].Reset();
    mTentacles[i].mStayRetractedTime = mStayRetractedTime;
  }
  EnableTentacles(true);
  switch (mPhase) {
  case 1:
    SendScriptMsgs(kSS_InternalState6, mgr, kSM_None);
    break;
  case 2:
    SendScriptMsgs(kSS_InternalState7, mgr, kSM_None);
    break;
  }
}

void CEmperorIngStage1::ChooseRandomTentacleCount(CStateManager& mgr, float) {
  const float roll = mgr.Random()->Range(0.f, 100.f);
  switch (mPhase) {
  case 0:
    mTentacleCountMode = roll < 50.f ? 0 : 1;
    break;
  case 1:
    mTentacleCountMode = roll < 50.f ? 1 : 2;
    break;
  default:
    mTentacleCountMode = 2;
    break;
  }
}

void CEmperorIngStage1::SetupCollisionActors(CStateManager& mgr) {
  {
    rstl::vector< CJointCollisionDescription > descs;
    descs.reserve(1);
    const CAnimData* animData = GetBrainActor(mgr)->GetModelData()->GetAnimationData();
    const CSegId brainSeg = animData->GetLocatorSegId(rstl::string_l("brainSpine_scale"));
    {
      const CJointCollisionDescription desc = CJointCollisionDescription::SphereCollision(
          brainSeg, CVector3f::Zero(), 5.f, rstl::string_l("HeartCollision"), 0.001f);
      descs.push_back_unsafe(desc);
    }
    mHeartManager = rs_new CCollisionActorManager(mgr, mBrainId, GetCurrentAreaId(), descs, true);
    if (CCollisionActor* actor = GetHeartCollisionActor(mgr)) {
      *actor->HealthInfo() = *GetHealthInfo();
      actor->SetDamageVulnerability(*GetDamageVulnerability());
      actor->AddMaterial(kMT_Target, kMT_Orbit, kMT_SeekerTarget, mgr);
    }
  }
  {
    rstl::vector< CJointCollisionDescription > descs;
    descs.reserve(1);
    const CJointCollisionDescription desc = CJointCollisionDescription::SphereCollision(
        CSegId(1), CVector3f::Zero(), 9.f, rstl::string_l("SphereCollision"), 0.001f);
    descs.push_back_unsafe(desc);
    mSphereManager = rs_new CCollisionActorManager(mgr, mSphereId, GetCurrentAreaId(), descs, true);
    const CMaterialFilter filter = CMaterialFilter::MakeIncludeExclude(
        CMaterialList(skSphereIncludeProjectile, skSphereIncludePlayer),
        CMaterialList(skSphereExcludePlatform, skSphereExcludeCollisionActor,
                      skSphereExcludeNoStaticCollision, skSphereExcludeNoPlatformCollision));
    if (CCollisionActor* actor = TCastToPtr< CCollisionActor >(
            mgr.ObjectById(mSphereManager->GetCollisionDescFromIndex(0).GetCollisionActorId()))) {
      actor->SetMaterialFilter(filter);
    }
  }
  {
    rstl::vector< CJointCollisionDescription > descs;
    descs.reserve(40);
    const CAnimData* animData = GetModelData()->GetAnimationData();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 5; ++j) {
        const SJointPair& joint = skTentacleJoints[i][j];
        const float scale = joint.scale;
        const CSegId from = animData->GetLocatorSegId(rstl::string_l(joint.from));
        const CSegId to = animData->GetLocatorSegId(rstl::string_l(joint.to));
        const CJointCollisionDescription desc = CJointCollisionDescription::OBBAutoSizeCollision(
            from, to, CVector3f(scale, scale, scale), CJointCollisionDescription::kOT_BetweenJoints,
            rstl::string_l(joint.from) + rstl::string("->") + rstl::string_l(joint.to), 10.f);
        descs.push_back_unsafe(desc);
      }
    }
    mTentacleManager =
        rs_new CCollisionActorManager(mgr, GetUniqueId(), GetCurrentAreaId(), descs, true);
  }
  SetMaterialFilter(CMaterialFilter::MakeIncludeExclude(
      CMaterialList(skTentacleInclude),
      CMaterialList(skTentacleExcludeCollisionActor, skTentacleExcludeCharacter,
                    skTentacleExcludeAIPassthrough, skTentacleExcludePlayer)));
  RemoveMaterial(kMT_Target, kMT_Orbit, kMT_SeekerTarget, mgr);
  for (int i = 0; i < 8; ++i) {
    SetTentacleCollisionActive(mgr, i, false);
  }
  SetTentacleCollisionActive(mgr, 9, true);
  SetTentacleCollisionActive(mgr, 8, false);
}

void CEmperorIngStage1::SetTentacleCollisionActive(CStateManager& mgr, int index, bool active) {
  if (index >= 0 && index <= 7) {
    for (int i = 0; i < 5; ++i) {
      if (CCollisionActor* actor = TCastToPtr< CCollisionActor >(mgr.ObjectById(
              mTentacleManager->GetCollisionDescFromIndex(index * 5 + i).GetCollisionActorId()))) {
        actor->SetActive(active);
      }
    }
  } else {
    switch (index) {
    case 9:
      if (CCollisionActor* actor = TCastToPtr< CCollisionActor >(
              mgr.ObjectById(mSphereManager->GetCollisionDescFromIndex(0).GetCollisionActorId()))) {
        actor->SetActive(active);
      }
      break;
    case 8:
      if (CCollisionActor* actor = TCastToPtr< CCollisionActor >(
              mgr.ObjectById(mHeartManager->GetCollisionDescFromIndex(0).GetCollisionActorId()))) {
        actor->SetActive(active);
      }
      break;
    }
  }
}

int CEmperorIngStage1::SegIdToTentacleIndex(const CSegId& seg) {
  if (mTentacleSegments.empty()) {
    mTentacleSegments.reserve(56);
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 7; ++j) {
        mTentacleSegments.push_back_unsafe(GetModelData()->GetAnimationData()->GetLocatorSegId(
            rstl::string_l(skTentacleLocators[i][j])));
      }
    }
  }
  const int count = mTentacleSegments.size();
  for (int i = 0; i < count; ++i) {
    if (mTentacleSegments[i] == seg) {
      return i / 7;
    }
  }
  return -1;
}

void CEmperorIngStage1::OnTentacleDestroyed(CStateManager& mgr, const CVector3f& position) {
  if (CActor* effect = TCastToPtr< CActor >(mgr.ObjectById(mDestroyedEffectId))) {
    effect->SetTranslation(position);
  }
  SendScriptMsgs(kSS_InternalState3, mgr, kSM_None);
}

void CEmperorIngStage1::DestroyCollisionActors(CStateManager& mgr) {
  mTentacleManager->Destroy(mgr);
  mHeartManager->Destroy(mgr);
  mSphereManager->Destroy(mgr);
}

void CEmperorIngStage1::UpdateCollisionActors(float dt, CStateManager& mgr) {
  mTentacleManager->Update(dt, mgr, CCollisionActorManager::kUO_ObjectSpace);
  mHeartManager->Update(dt, mgr, CCollisionActorManager::kUO_ObjectSpace);
  mSphereManager->Update(dt, mgr, CCollisionActorManager::kUO_ObjectSpace);
}

CCollisionActor* CEmperorIngStage1::GetTentacleCollisionActor(CStateManager& mgr, int tentacle,
                                                              int segment) const {
  return TCastToPtr< CCollisionActor >(mgr.ObjectById(
      mTentacleManager->GetCollisionDescFromIndex(tentacle * 5 + segment).GetCollisionActorId()));
}

void CEmperorIngStage1::SetupTentacles(CStateManager& mgr) {
  const CPASDatabase& pasDatabase = GetModelData()->GetAnimationData()->GetPASDatabase();
  for (int i = 0; i < 8; ++i) {
    int extendAnim = -1;
    int retractAnim = -1;
    int idleAnim = -1;
    rstl::pair< float, int > best = pasDatabase.FindBestAnimation(
        CPASAnimParmData(pas::kAS_Unknown27, CPASAnimParm::FromInt32(i), CPASAnimParm::FromEnum(0)),
        -1);
    if (best.first > FLT_EPSILON) {
      extendAnim = best.second;
    }
    best = pasDatabase.FindBestAnimation(
        CPASAnimParmData(pas::kAS_Unknown27, CPASAnimParm::FromInt32(i), CPASAnimParm::FromEnum(2)),
        -1);
    if (best.first > FLT_EPSILON) {
      retractAnim = best.second;
    }
    best = pasDatabase.FindBestAnimation(
        CPASAnimParmData(pas::kAS_Unknown27, CPASAnimParm::FromInt32(i), CPASAnimParm::FromEnum(1)),
        -1);
    if (best.first > FLT_EPSILON) {
      idleAnim = best.second;
    }
    if (extendAnim != -1 && retractAnim != -1) {
      STentacle tentacle(this, i, extendAnim, retractAnim, idleAnim);
      tentacle.SetHealthInfo(mgr);
      mTentacles.push_back(tentacle);
    }
  }
}

void CEmperorIngStage1::UpdateTentacles(float dt, CStateManager& mgr) {
  for (int i = 0; i < static_cast< int >(mTentacles.size()); ++i) {
    mTentacles[i].Update(dt, mgr);
  }
}

void CEmperorIngStage1::SetVulnerabilityMode(CStateManager& mgr, EVulnerabilityMode mode) {
  const CDamageVulnerability* vulnerability;
  bool dark = false;
  bool light = false;
  switch (mode) {
  case kVM_Melee:
    vulnerability = &mMeleeVulnerability;
    break;
  case kVM_Dark:
    vulnerability = &mProjectileVulnerability;
    dark = true;
    break;
  case kVM_Light:
    vulnerability = &mWarpVulnerability;
    light = true;
    break;
  default:
    vulnerability = &mNormalVulnerability;
    break;
  case kVM_Immune:
    vulnerability = &CDamageVulnerability::ImmuneVulnerabilty();
    break;
  }
  AnimationData()->SetEffectState(rstl::string_l(skDarkVulnerabilityEffect), dark, mgr);
  AnimationData()->SetEffectState(rstl::string_l(skLightVulnerabilityEffect), light, mgr);
  if (vulnerability) {
    for (int i = 0; i < static_cast< int >(mTentacles.size()); ++i) {
      mTentacles[i].SetVulnerability(mgr, *vulnerability);
    }
  }
}

void CEmperorIngStage1::FindAnimations(CStateManager& mgr) {
  mSphereId = FindConnectedObject(mgr, kSS_Connect, kSM_Play);
  const CActor* sphere = TCastToConstPtr< CActor >(mgr.GetObjectById(mSphereId));
  const CPASDatabase& pasDatabase = sphere->GetModelData()->GetAnimationData()->GetPASDatabase();
  rstl::pair< float, int > best = pasDatabase.FindBestAnimation(
      CPASAnimParmData(pas::kAS_Locomotion, CPASAnimParm::FromEnum(pas::kLT_Crouch),
                       CPASAnimParm::FromEnum(0)),
      -1);
  if (best.first > FLT_EPSILON) {
    mIdleAnim = best.second;
  }
  best = pasDatabase.FindBestAnimation(CPASAnimParmData(pas::kAS_Locomotion,
                                                        CPASAnimParm::FromEnum(pas::kLT_Crouch),
                                                        CPASAnimParm::FromEnum(7)),
                                       -1);
  if (best.first > FLT_EPSILON) {
    mBeginHeartStageAnim = best.second;
  }
  best = pasDatabase.FindBestAnimation(CPASAnimParmData(pas::kAS_Locomotion,
                                                        CPASAnimParm::FromEnum(pas::kLT_Crouch),
                                                        CPASAnimParm::FromEnum(6)),
                                       -1);
  if (best.first > FLT_EPSILON) {
    mEndHeartStageAnim = best.second;
  }
}

CActor* CEmperorIngStage1::GetSphereActor(CStateManager& mgr) const {
  return TCastToPtr< CActor >(mgr.ObjectById(mSphereId));
}

CActor* CEmperorIngStage1::GetBrainActor(CStateManager& mgr) const {
  return TCastToPtr< CActor >(mgr.ObjectById(mBrainId));
}

CActor* CEmperorIngStage1::GetHeartEffectActor(CStateManager& mgr) const {
  return TCastToPtr< CActor >(mgr.ObjectById(mHeartEffectId));
}

CCollisionActor* CEmperorIngStage1::GetHeartCollisionActor(CStateManager& mgr) const {
  return TCastToPtr< CCollisionActor >(
      mgr.ObjectById(mHeartManager->GetCollisionDescFromIndex(0).GetCollisionActorId()));
}

int CEmperorIngStage1::GetTentacleCount() const {
  switch (mTentacleCountMode) {
  case 0:
    return 4;
  case 1:
    return 6;
  default:
    return 8;
  }
}

void CEmperorIngStage1::EnableTentacles(bool enable) {
  const int tentacleCount = GetTentacleCount();
  for (int i = 0; i < static_cast< int >(mTentacles.size()); ++i) {
    if (enable && i < tentacleCount) {
      mTentacles[i].mEnabled = true;
    } else {
      mTentacles[i].mEnabled = false;
    }
  }
}

int CEmperorIngStage1::CountRetractedTentacles() const {
  int count = 0;
  const int tentacleCount = GetTentacleCount();
  for (int i = 0; i < tentacleCount; ++i) {
    if (mTentacles[i].mState == STentacle::kS_Retracted) {
      ++count;
    }
  }
  return count;
}

void CEmperorIngStage1::SetAllCollisionMaterials(CStateManager& mgr, bool enable) {
  for (int i = 0; i < static_cast< int >(mTentacles.size()); ++i) {
    mTentacles[i].SetCollisionMaterials(mgr, enable);
  }
}

void CEmperorIngStage1::SetTurnTarget(CStateManager& mgr, const CVector3f& target) {
  CActor* brain = GetBrainActor(mgr);
  CActor* sphere = GetSphereActor(mgr);
  const CTransform4f baseXf =
      brain->GetTransform() *
      brain->GetModelData()->GetScaledLocatorTransform(rstl::string_l(skSphereBaseLocator));
  (void)baseXf;
  CVector3f forward(sphere->GetTransform().GetColumn(kDY).GetX(),
                    sphere->GetTransform().GetColumn(kDY).GetY(), 0.f);
  forward.Normalize();
  mTurnAngle = CVector3f::GetAngleDiff(forward, target);
  const CVector3f cross = CVector3f::Cross(forward, target);
  if (CVector3f::Dot(cross, CVector3f::Up()) < 0.f) {
    mTurnAngle = -mTurnAngle;
  }
}

void CEmperorIngStage1::TurnToSphereForward(CStateManager& mgr) {
  const CTransform4f& xf = GetSphereActor(mgr)->GetTransform();
  SetTurnTarget(mgr, xf.GetColumn(kDY));
}

CEmperorIngStage1::STentacle::STentacle(CEmperorIngStage1* owner, int index, int extendAnim,
                                        int retractAnim, int idleAnim)
: mOwner(owner)
, mTime(0.f)
, mIndex(index)
, mExtendAnim(extendAnim)
, mRetractAnim(retractAnim)
, mIdleAnim(idleAnim)
, mStayRetractedTime(0.f)
, mDamageHitTimer(CPatterned::skDamageHitTime)
, mLastHealth(0.f)
, mEnabled(false)
, mExtended(false)
, mCollisionActive(false)
, mHit(false)
, mPortalActive(false)
, mForceExtended(false)
, mState(kS_Retracted)
, mPortalEffectId(kInvalidUniqueId)
, mPortalPosition(CVector3f::Zero())
, mPortalNormal(CVector3f::Up()) {}

void CEmperorIngStage1::STentacle::Update(float dt, CStateManager& mgr) {
  UpdateAnimation(dt, mgr);
  Think(dt, mgr);
  UpdateEffects();
}

void CEmperorIngStage1::STentacle::UpdateAnimation(float dt, CStateManager& mgr) {
  CAnimData* animData = mOwner->ModelData()->AnimationData();
  const EState state = mForceExtended ? kS_Extended : mState;
  switch (state) {
  case kS_Retracted:
    mExtended = false;
    if (!animData->IsAdditiveAnimationActive(mIdleAnim)) {
      animData->DelAdditiveAnimationImmediately(mRetractAnim);
      animData->DelAdditiveAnimationImmediately(mExtendAnim);
      animData->AddAdditiveAnimation(mIdleAnim, 1.f, true, false);
    }
    mTime += dt;
    break;
  case kS_Extending:
    if (!animData->IsAdditiveAnimationActive(mExtendAnim)) {
      if (!mHit) {
        OnHit(mgr);
      }
      mExtended = true;
      animData->DelAdditiveAnimationImmediately(mRetractAnim);
      animData->DelAdditiveAnimationImmediately(mIdleAnim);
      animData->AddAdditiveAnimation(mExtendAnim, 1.f, false, false);
    } else {
      const rstl::rc_ptr< CAnimTreeNode > tree = animData->GetAdditiveAnimationTree(mExtendAnim);
      if (!tree || close_enough(tree->VGetTimeRemaining().GetSeconds(), 0.f)) {
        mState = kS_Extended;
      }
    }
    break;
  case kS_Extended:
    if (!mHit) {
      OnHit(mgr);
    }
    mExtended = true;
    animData->DelAdditiveAnimationImmediately(mRetractAnim);
    animData->DelAdditiveAnimationImmediately(mIdleAnim);
    animData->DelAdditiveAnimationImmediately(mExtendAnim);
    break;
  case kS_Retracting:
    mExtended = false;
    if (!animData->IsAdditiveAnimationActive(mRetractAnim)) {
      animData->DelAdditiveAnimationImmediately(mIdleAnim);
      animData->DelAdditiveAnimationImmediately(mExtendAnim);
      animData->AddAdditiveAnimation(mRetractAnim, 1.f, false, false);
    } else {
      const rstl::rc_ptr< CAnimTreeNode > tree = animData->GetAdditiveAnimationTree(mRetractAnim);
      if (!tree || close_enough(tree->VGetTimeRemaining().GetSeconds(), 0.f)) {
        mState = kS_Retracted;
      }
    }
    break;
  }

  if (mExtended) {
    if (!mCollisionActive) {
      mOwner->SetTentacleCollisionActive(mgr, mIndex, true);
      mCollisionActive = true;
    }
  } else if (mCollisionActive) {
    mOwner->SetTentacleCollisionActive(mgr, mIndex, false);
    mCollisionActive = false;
  }
}

void CEmperorIngStage1::STentacle::Think(float dt, CStateManager& mgr) {
  sfxTimer -= dt;
  CCollisionActor* actor = mOwner->GetTentacleCollisionActor(mgr, mIndex, 4);
  if (actor && mHit) {
    const float hp = actor->HealthInfo()->GetHP();
    if (hp < mLastHealth) {
      if (sfxTimer < 0.f && hp > 0.f) {
        CSfxManager::AddEmitter(mOwner->mTentacleDamagedSound, actor->GetTranslation(),
                                mOwner->GetCurrentAreaId().Value(), true, false,
                                CSfxManager::kMedPriority);
        sfxTimer = mgr.Random()->Range(3.f, 7.f);
      }
      mDamageHitTimer = 0.f;
      mLastHealth = hp;
    }
    if (hp <= 0.f) {
      mHit = false;
      mState = kS_Retracting;
      mOwner->OnTentacleDestroyed(mgr, actor->GetTranslation());
    }
  }
  mDamageHitTimer = rstl::min_val(CPatterned::skDamageHitTime, mDamageHitTimer + dt);
}

void CEmperorIngStage1::STentacle::UpdateEffects() {
  SetEffectParams(rstl::string_l(skDarkVulnerabilityEffect));
  SetEffectParams(rstl::string_l(skLightVulnerabilityEffect));
}

void CEmperorIngStage1::STentacle::SetEffectParams(const rstl::string& name) {
  CAnimData* animData = mOwner->ModelData()->AnimationData();
  CParticleDatabase& particleDb = animData->GetParticleDB();
  const CCECharacterInfo::TEffectList& effects = animData->GetCharacterInfo().GetEffects();
  CCECharacterInfo::TEffectList::const_iterator it = rstl::find_by_key(effects, name);
  if (it == effects.end()) {
    return;
  }
  for (rstl::vector< CEffectComponent >::const_iterator comp = it->second.begin();
       comp != it->second.end(); ++comp) {
    if (mOwner->SegIdToTentacleIndex(comp->GetSegmentId()) == mIndex) {
      const uint hash = comp->GetComponentNameHash();
      particleDb.SetParticleExternalParam(hash, 0, mPortalPosition.GetX());
      particleDb.SetParticleExternalParam(hash, 1, mPortalPosition.GetY());
      particleDb.SetParticleExternalParam(hash, 2, mPortalPosition.GetZ());
      particleDb.SetParticleExternalParam(hash, 3, -mPortalNormal.GetX());
      particleDb.SetParticleExternalParam(hash, 4, -mPortalNormal.GetY());
      particleDb.SetParticleExternalParam(hash, 5, -mPortalNormal.GetZ());
    }
  }
}

void CEmperorIngStage1::STentacle::OnHit(CStateManager& mgr) {
  if (CCollisionActor* actor = mOwner->GetTentacleCollisionActor(mgr, mIndex, 4)) {
    mHit = true;
    mLastHealth = actor->HealthInfo()->GetInitialHP();
    actor->HealthInfo()->SetHP(actor->HealthInfo()->GetInitialHP());
  }
}

void CEmperorIngStage1::STentacle::SetCollisionMaterials(CStateManager& mgr, bool enable) {
  if (CCollisionActor* actor = mOwner->GetTentacleCollisionActor(mgr, mIndex, 4)) {
    if (enable) {
      actor->AddMaterial(kMT_Target, kMT_Orbit, kMT_SeekerTarget, kMT_ExcludeFromRadar, mgr);
    } else {
      actor->RemoveMaterial(kMT_Target, kMT_Orbit, kMT_SeekerTarget, kMT_ExcludeFromRadar, mgr);
    }
  }
}

void CEmperorIngStage1::STentacle::SetHealthInfo(CStateManager& mgr) {
  if (CCollisionActor* actor = mOwner->GetTentacleCollisionActor(mgr, mIndex, 4)) {
    *actor->HealthInfo() = mOwner->mHealthInfo;
  }
  SetCollisionMaterials(mgr, true);
}

void CEmperorIngStage1::STentacle::SetVulnerability(CStateManager& mgr,
                                                    const CDamageVulnerability& vulnerability) {
  if (CCollisionActor* actor = mOwner->GetTentacleCollisionActor(mgr, mIndex, 4)) {
    actor->SetDamageVulnerability(vulnerability);
  }
}

void CEmperorIngStage1::STentacle::Reset() {
  mTime = 0.f;
  if (mState != kS_Extended) {
    mState = kS_Extending;
  }
}

CModelFlags CEmperorIngStage1::STentacle::GetModelFlags() const {
  if (mPortalActive) {
    const CModelFlags flags = CModelFlags::Normal();
    return CModelFlags(flags.GetTrans(), flags.GetShaderSet(),
                       CModelFlags::EFlags(flags.GetOtherFlags() | CModelFlags::kF_Unknown80),
                       flags.GetColor());
  }
  return CModelFlags::Normal();
}

u64 CEmperorIngStage1::STentacle::GetRenderMask() const {
  return skTentacleRenderMasks[mIndex] | (mIndex == 0 ? 0x02000000ull : 0ull);
}

CColor CEmperorIngStage1::STentacle::GetDamageColor() const {
  return CColor::Lerp(CColor::Black(), CPatterned::skDamageColor,
                      mDamageHitTimer / CPatterned::skDamageHitTime);
}

CPlane CEmperorIngStage1::STentacle::GetPortalPlane() const {
  return CPlane(mPortalPosition, mPortalNormal);
}

void CEmperorIngStage1::STentacle::ActivatePortal(CStateManager& mgr, const CVector3f& position,
                                                  const CUnitVector3f& normal) {
  if (mState == kS_Retracted) {
    return;
  }
  mPortalActive = true;
  mPortalPosition = position;
  mPortalNormal = normal;
  if (mPortalEffectId == kInvalidUniqueId) {
    mPortalEffectId =
        CreateWarpPortalEffect(mgr, mOwner->mPortalEffect, mOwner->GetCurrentAreaId());
  }
  if (CScriptEffect* effect = TCastToPtr< CScriptEffect >(mgr.ObjectById(mPortalEffectId))) {
    effect->SetTransform(
        CTransform4f::LookAt(mPortalPosition, mPortalPosition + mPortalNormal, CVector3f::Up()));
    effect->AcceptScriptMsg(mgr, CScriptMsg(mOwner->GetUniqueId(), mPortalEffectId, kSM_Activate));
    CAudioSys::C3DEmitterParmData parms(50.f, 0.1f, 1, 0x7f);
    parms.mPos = effect->GetTranslation();
    parms.mSfxId = mOwner->mWarpSound;
    CSfxManager::AddEmitter(parms, effect->GetCurrentAreaId().Value(), true, false,
                            CSfxManager::kMedPriority);
  }
}

void CEmperorIngStage1::STentacle::DeactivatePortal(CStateManager& mgr) {
  mPortalActive = false;
  mPortalPosition = CVector3f::Zero();
  mPortalNormal = CVector3f::Up();
  if (CScriptEffect* effect = TCastToPtr< CScriptEffect >(mgr.ObjectById(mPortalEffectId))) {
    effect->AcceptScriptMsg(mgr,
                            CScriptMsg(mOwner->GetUniqueId(), mPortalEffectId, kSM_Deactivate));
  }
}

CEmperorIngStage1Tip::CEmperorIngStage1Tip(TUniqueId uid, const rstl::string& name,
                                           const CEntityInfo& info, const CTransform4f& xf,
                                           const CModelData& modelData,
                                           const CActorParameters& actorParams,
                                           CAssetId portalEffect, const CDamageInfo& damage,
                                           ushort portalSound, ushort tipSound)
: CPhysicsActor(uid, name, info, 0, xf, modelData, CMaterialList(),
                CAABox(-2.f * CVector3f::One(), 2.f * CVector3f::One()), SMoverData(1.f),
                actorParams, CPhysicsActor::skDefaultStepData)
, mActiveTime(0.f)
, mPortalEffectId(kInvalidUniqueId)
, mHasHitPlayer(false)
, mPortalEffect(portalEffect)
, mDamage(damage)
, mPortalSound(portalSound)
, mTipSound(tipSound) {}

void CEmperorIngStage1Tip::AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) {
  switch (msg.GetMessage()) {
  case kSM_Create: {
    mPortalEffectId = CreateWarpPortalEffect(mgr, mPortalEffect, mgr.GetNextAreaId());
    if (CScriptEffect* effect = TCastToPtr< CScriptEffect >(mgr.ObjectById(mPortalEffectId))) {
      effect->SetTransform(GetTransform());
      effect->AcceptScriptMsg(mgr, CScriptMsg(GetUniqueId(), mPortalEffectId, kSM_Activate));
      CSfxManager::AddEmitter(mPortalSound, effect->GetTranslation(),
                              effect->GetCurrentAreaId().Value(), true, false,
                              CSfxManager::kMedPriority);
    }
    const CModelFlags flags = CModelFlags::Normal();
    SetModelFlags(CModelFlags(flags, flags.GetOtherFlags() | CModelFlags::kF_Unknown80));
    break;
  }
  case kSM_Delete:
    mgr.DeleteObjectRequest(mPortalEffectId);
    break;
  default:
    break;
  }
}

void CEmperorIngStage1Tip::Think(float dt, CStateManager& mgr) {
  AnimationData()->SetIsAnimating(true);
  CActor::Think(dt, mgr);
  mActiveTime += dt;
  if (mActiveTime > 0.75f) {
    SetActive(true);
    if (HasAnimation()) {
      if (!AnimationData()->IsAnimTimeRemaining(dt - FLT_EPSILON, rstl::string_l("Whole Body"))) {
        SetActive(false);
        if (CScriptEffect* effect = TCastToPtr< CScriptEffect >(mgr.ObjectById(mPortalEffectId))) {
          if (effect->GetActive()) {
            CSfxManager::AddEmitter(mTipSound, effect->GetTranslation(),
                                    effect->GetCurrentAreaId().Value(), true, false,
                                    CSfxManager::kMedPriority);
            effect->AcceptScriptMsg(mgr,
                                    CScriptMsg(GetUniqueId(), mPortalEffectId, kSM_Deactivate));
          }
        }
      }
      if (mActiveTime > 5.f) {
        mgr.DeleteObjectRequest(GetUniqueId());
      }
      UpdateAnimation(dt, mgr, true);
    }
  }

  if (!mHasHitPlayer) {
    const rstl::optional_object< CAABox > bounds = mgr.GetPlayer(0)->GetTouchBounds();
    if (bounds) {
      for (int i = 0; i < 7; ++i) {
        const CVector3f pos =
            GetTransform() *
            AnimationData()
                ->GetLocatorTransform(rstl::string_l(skTipSpineLocators[i]), nullptr)
                .GetTranslation();
        const CAABox box(pos - CVector3f(2.f, 2.f, 2.f), pos + CVector3f(2.f, 2.f, 2.f));
        if (bounds->DoBoundsOverlap(box)) {
          mHasHitPlayer = true;
          mgr.ApplyDamage(
              GetUniqueId(), mgr.GetPlayer(0)->GetUniqueId(), GetUniqueId(), mDamage,
              CMaterialFilter::MakeIncludeExclude(CMaterialList(skDamageMaterial), CMaterialList()),
              CVector3f::Zero());
          break;
        }
      }
    }
  }
}

void CEmperorIngStage1Tip::Render(const CStateManager& mgr) const {
  gpRender->SetModelMatrix(GetTransform());
  const CPlane plane(GetTranslation(),
                     CUnitVector3f(GetTransform().GetColumn(kDY).GetX(),
                                   GetTransform().GetColumn(kDY).GetY(),
                                   GetTransform().GetColumn(kDY).GetZ(), CUnitVector3f::kN_No));
  GetModelData()->SetupWorldSpacePortalPlane(GetTransform(), plane);
  CPhysicsActor::Render(mgr);
}

bool CEmperorIngStage1::CreateBeam(float dt, CStateManager& mgr) {
  if (mBeamId == kInvalidUniqueId) {
    const CBeamInfo beamInfo = TLdrToBeamInfo(mBeamInfo, 0x201);
    const TUniqueId uid = mgr.AllocateUniqueId();
    CDamageInfo damage = mBeamProjectileInfo.GetDamage();
    damage.SetDamage(dt * damage.GetDamage());
    damage.SetNoImmunity(true);
    CPlasmaProjectile* beam = rs_new CPlasmaProjectile(
        mBeamProjectileInfo.Token(), rstl::string_l("EmperorIngStage1Beam"), kWT_AI, beamInfo,
        CTransform4f::Identity(), kMT_ProjectilePassthrough, damage, uid, GetCurrentAreaId(),
        GetBrainActor(mgr)->GetUniqueId(), CWeaponAssetInfo(), false,
        CWeapon::kPA_KeepInCinematic | CWeapon::kPA_BigStrike);
    if (beam) {
      beam->SetDamageDuration(1.5f);
      mgr.AddObject(*beam);
      mBeamId = uid;
    }
    const CVector3f pos = GetBrainLocatorPosition(mgr);
    CAudioSys::C3DEmitterParmData parms(1000.f, 0.1f, 1, 0x7f, 0x14);
    parms.mPos = pos;
    parms.mDir = CVector3f::Up();
    parms.mSfxId = mBeamSound;
    mBeamSfx = CSfxManager::AddEmitter(parms, GetCurrentAreaId().Value(), true, true,
                                       CSfxManager::kMedPriority);
    mTelegraphEffectTimer = 59.f / 60.f;
    mBeamTimeLeft = 1.5f;
    return true;
  }
  return false;
}

void CEmperorIngStage1::UpdateBeam(float dt, CStateManager& mgr) {
  CPlasmaProjectile* beam = static_cast< CPlasmaProjectile* >(mgr.ObjectById(mBeamId));
  if (!beam) {
    return;
  }
  mBeamTimeLeft -= dt;
  if (!beam->GetActive() && mBeamTimeLeft < 0.f) {
    const CVector3f pos = GetBrainLocatorPosition(mgr);
    mBeamTarget = mgr.GetPlayer(0)->GetTranslation();
    mBeamTarget.SetZ(GetBrainActor(mgr)->GetTranslation().GetZ());
    const CTransform4f fireXf = CTransform4f::LookAt(pos, mBeamTarget, CVector3f::Up());
    beam->Fire(fireXf, mgr, false);
  }
  const CVector3f pos = GetBrainLocatorPosition(mgr);
  CVector3f dir = mgr.GetPlayer(0)->GetTranslation() - mBeamTarget;
  dir.SetZ(0.f);
  if (dir.CanBeNormalized()) {
    dir.Normalize();
    mBeamTarget += dt * (10.f * dir);
  }
  const CTransform4f fxXf = CTransform4f::LookAt(pos, mBeamTarget, CVector3f::Up());
  beam->UpdateFx(fxXf, dt, mgr);
  CSfxManager::UpdateEmitter(mBeamSfx, beam->GetCurrentPos(), CVector3f::Up(), 0x7f);
}

void CEmperorIngStage1::DeleteBeam(CStateManager& mgr) {
  if (CPlasmaProjectile* beam = static_cast< CPlasmaProjectile* >(mgr.ObjectById(mBeamId))) {
    beam->ResetBeam(mgr, false);
    CSfxManager::RemoveEmitter(mBeamSfx);
    mgr.DeleteObjectRequest(mBeamId);
    mBeamId = kInvalidUniqueId;
    mBeamSfx = CSfxHandle();
  }
}

CVector3f CEmperorIngStage1::GetBrainLocatorPosition(CStateManager& mgr) {
  CActor* brain = GetBrainActor(mgr);
  const CTransform4f xf = brain->GetTransform() * brain->GetModelData()->GetScaledLocatorTransform(
                                                      rstl::string_l(skBrainLocator));
  return xf.GetTranslation();
}

void CEmperorIngStage1::ResetAttackTimer(CStateManager& mgr) {
  mAttackTimer = mgr.Random()->Range(mAttackIntervalMin, mAttackIntervalMax);
}

void CEmperorIngStage1::SendInternalState(CStateManager& mgr, int phase) {
  switch (phase) {
  case 1:
    SendScriptMsgs(kSS_InternalState1, mgr, kSM_None);
    break;
  case 2:
    SendScriptMsgs(kSS_InternalState2, mgr, kSM_None);
    break;
  default:
    SendScriptMsgs(kSS_InternalState0, mgr, kSM_None);
    break;
  }
}

void CEmperorIngStage1::UpdateMeleeDamage(float dt, CStateManager& mgr) {
  if (!mMeleeActive) {
    return;
  }
  const rstl::optional_object< CAABox > playerBounds = mgr.GetPlayer(0)->GetTouchBounds();
  if (!playerBounds) {
    return;
  }
  const CAABox& bounds = *playerBounds;
  mDamageCooldown -= dt;
  if (mDamageCooldown <= 0.f) {
    const CAnimData* animData = GetModelData()->GetAnimationData();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 5; ++j) {
        const SJointPair& joint = skTentacleJoints[i][j];
        const CSegId from = animData->GetLocatorSegId(rstl::string_l(joint.from));
        const CSegId to = animData->GetLocatorSegId(rstl::string_l(joint.to));
        const CVector3f start = GetTransform() * GetScaledLocatorTransform(from).GetTranslation();
        const CVector3f end = GetTransform() * GetScaledLocatorTransform(to).GetTranslation();
        CAABox box = CAABox::MakeMaxInvertedBox();
        box.AccumulateBounds(start);
        box.AccumulateBounds(end);
        box.AccumulateBounds(box.GetMinPoint() - CVector3f(4.f, 4.f, 4.f));
        box.AccumulateBounds(box.GetMaxPoint() + CVector3f(4.f, 4.f, 4.f));
        if (bounds.DoBoundsOverlap(box)) {
          mgr.ApplyDamage(
              GetUniqueId(), mgr.GetPlayer(0)->GetUniqueId(), GetUniqueId(), mMeleeDamage,
              CMaterialFilter::MakeIncludeExclude(CMaterialList(skDamageMaterial), CMaterialList()),
              CVector3f::Zero());
          mDamageCooldown = 0.2f;
          return;
        }
      }
    }
  }
}

void CEmperorIngStage1::UpdateJumpRopeDamage(float dt, CStateManager& mgr) {
  if (!mJumpRopeActive) {
    return;
  }
  const rstl::optional_object< CAABox > playerBounds = mgr.GetPlayer(0)->GetTouchBounds();
  if (!playerBounds) {
    return;
  }
  const CAABox& bounds = *playerBounds;
  mDamageCooldown -= dt;
  if (mDamageCooldown <= 0.f) {
    const CAnimData* animData = GetModelData()->GetAnimationData();
    for (int i = 0; i < 8; ++i) {
      for (int j = 0; j < 5; ++j) {
        const SJointPair& joint = skTentacleJoints[i][j];
        const CSegId from = animData->GetLocatorSegId(rstl::string_l(joint.from));
        const CSegId to = animData->GetLocatorSegId(rstl::string_l(joint.to));
        const CVector3f start = GetTransform() * GetScaledLocatorTransform(from).GetTranslation();
        const CVector3f end = GetTransform() * GetScaledLocatorTransform(to).GetTranslation();
        CAABox box = CAABox::MakeMaxInvertedBox();
        box.AccumulateBounds(start);
        box.AccumulateBounds(end);
        if (j == 4) {
          CVector3f dir = end - start;
          dir.SetZ(0.f);
          if (dir.CanBeNormalized()) {
            dir.Normalize();
            box.AccumulateBounds(end + 8.f * dir);
          }
        }
        if (bounds.DoBoundsOverlap(box)) {
          mgr.ApplyDamage(
              GetUniqueId(), mgr.GetPlayer(0)->GetUniqueId(), GetUniqueId(), mMeleeDamage,
              CMaterialFilter::MakeIncludeExclude(CMaterialList(skDamageMaterial), CMaterialList()),
              CVector3f::Zero());
          mDamageCooldown = 0.2f;
          return;
        }
      }
    }
  }
}
