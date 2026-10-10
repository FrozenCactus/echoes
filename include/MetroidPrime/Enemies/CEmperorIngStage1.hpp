#ifndef _CEMPERORINGSTAGE1
#define _CEMPERORINGSTAGE1

#include "types.h"

#include "Kyoto/Audio/CSfxHandle.hpp"
#include "Kyoto/TToken.hpp"
#include "MetroidPrime/CDamageInfo.hpp"
#include "MetroidPrime/CDamageVulnerability.hpp"
#include "MetroidPrime/CHealthInfo.hpp"
#include "MetroidPrime/CPhysicsActor.hpp"
#include "MetroidPrime/Enemies/CPatterned.hpp"
#include "MetroidPrime/ScriptLoader/Structs/SLdrAnimationSet.hpp"
#include "MetroidPrime/ScriptLoader/Structs/SLdrAudioPlaybackParms.hpp"
#include "MetroidPrime/ScriptLoader/Structs/SLdrPlasmaBeamInfo.hpp"
#include "MetroidPrime/Weapons/CProjectileInfo.hpp"
#include "MetroidPrime/Weapons/CShockWave.hpp"
#include "rstl/optional_object.hpp"
#include "rstl/reserved_vector.hpp"
#include "rstl/single_ptr.hpp"
#include "rstl/vector.hpp"

class CCollisionActor;
class CCollisionActorManager;
class CGenDescription;
class CWeaponDescription;
struct SLdrEmperorIngStage1Data;

// Guessed name: one of the stabbing tentacle tips the Emperor Ing's warp attack spawns next to the
// player. It plays out its animation, then damages the player if any of its spine joints touch it.
class CEmperorIngStage1Tip : public CPhysicsActor {
public:
  CEmperorIngStage1Tip(TUniqueId uid, const rstl::string& name, const CEntityInfo& info,
                       const CTransform4f& xf, const CModelData& modelData,
                       const CActorParameters& actorParams, CAssetId portalEffect,
                       const CDamageInfo& damage, ushort portalSound, ushort tipSound);

  // CEntity
  ~CEmperorIngStage1Tip() override {}
  void Think(float dt, CStateManager& mgr) override;
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) override;

  // CActor
  void Render(const CStateManager& mgr) const override;

private:
  float mActiveTime;         // Guessed name
  TUniqueId mPortalEffectId; // Guessed name
  bool mHasHitPlayer;        // Guessed name
  CAssetId mPortalEffect;    // Guessed name
  CDamageInfo mDamage;       // Guessed name
  ushort mPortalSound;       // Guessed name
  ushort mTipSound;          // Guessed name
};
CHECK_SIZEOF(CEmperorIngStage1Tip, 0x300)

// The first stage of the Emperor Ing fight: a stationary sphere of tentacles around a brain that
// turns to face the player, taunts, lashes out with the tentacles and finally exposes its heart.
class CEmperorIngStage1 : public CPatterned {
public:
  // Guessed names; which damage vulnerability the tentacles currently present.
  enum EVulnerabilityMode {
    kVM_Immune,
    kVM_Normal,
    kVM_Melee,
    kVM_Dark,
    kVM_Light,
  };

  // Guessed name: one of the eight tentacles surrounding the boss.
  struct STentacle {
    enum EState {
      kS_Retracted,
      kS_Extending,
      kS_Extended,
      kS_Retracting,
    };

    STentacle(CEmperorIngStage1* owner, int index, int extendAnim, int retractAnim, int idleAnim);

    void DeactivatePortal(CStateManager& mgr);
    void ActivatePortal(CStateManager& mgr, const CVector3f& position, const CUnitVector3f& normal);
    CPlane GetPortalPlane() const;
    CColor GetDamageColor() const;
    u64 GetRenderMask() const;
    CModelFlags GetModelFlags() const;
    void Reset();
    void SetVulnerability(CStateManager& mgr, const CDamageVulnerability& vulnerability);
    void SetHealthInfo(CStateManager& mgr);
    void SetCollisionMaterials(CStateManager& mgr, bool enable);
    void OnHit(CStateManager& mgr);
    void SetEffectParams(const rstl::string& name);
    void UpdateEffects();
    void Think(float dt, CStateManager& mgr);
    void UpdateAnimation(float dt, CStateManager& mgr);
    void Update(float dt, CStateManager& mgr);

    CEmperorIngStage1* mOwner;
    float mTime;
    int mIndex;
    int mExtendAnim;
    int mRetractAnim;
    int mIdleAnim;
    float mStayRetractedTime;
    float mDamageHitTimer;
    float mLastHealth;
    bool mEnabled : 1;
    bool mExtended : 1;
    bool mCollisionActive : 1;
    bool mHit : 1;
    bool mPortalActive : 1;
    bool mForceExtended : 1;
    EState mState;
    TUniqueId mPortalEffectId;
    CVector3f mPortalPosition;
    CUnitVector3f mPortalNormal;
  };

  CEmperorIngStage1(TUniqueId uid, const rstl::string& name, const CEntityInfo& info,
                    const CTransform4f& xf, const CModelData& modelData,
                    const CActorParameters& actorParams, const CPatternedInfo& patternedInfo,
                    const SLdrEmperorIngStage1Data& data);

  // CEntity
  ~CEmperorIngStage1() override;
  void Think(float dt, CStateManager& mgr) override;
  void AcceptScriptMsg(CStateManager& mgr, const CScriptMsg& msg) override;

  // CActor
  void PreRender(CStateManager& mgr) override;
  void Render(const CStateManager& mgr) const override;
  rstl::optional_object< CAABox > GetTouchBounds() const override;
  void DoUserAnimEvent(CStateManager& mgr, const CInt32POINode& node, EUserEventType type,
                       float dt) override;
  void ProcessSoundEvent(int sfxId, float weight, int flags, float fallOff, float maxDist,
                         const CSegId& locator, ushort pitchStart, ushort pitchEnd,
                         float pitchDuration, uchar minVol, uchar maxVol, float distanceSquared,
                         const CVector3f& position, int aid, CStateManager& mgr,
                         bool translateId) override;

  // CPatterned
  void SetupStateMachine(CStateManager& mgr) override;

  // CEmperorIngStage1
  void JumpRope(CStateManager& mgr, EStateMsg msg, float dt);         // Guessed name
  void Taunt(CStateManager& mgr, EStateMsg msg, float dt);            // Guessed name
  void TauntAttack(CStateManager& mgr, EStateMsg msg, float dt);      // Guessed name
  void WarpAttack(CStateManager& mgr, EStateMsg msg, float dt);       // Guessed name
  void ProjectileAttack(CStateManager& mgr, EStateMsg msg, float dt); // Guessed name
  void Attack(CStateManager& mgr, EStateMsg msg, float dt);           // Guessed name
  void MeleeAttack(CStateManager& mgr, EStateMsg msg, float dt);      // Guessed name
  void Dead(CStateManager& mgr, EStateMsg msg, float dt);
  void EndHeartStage(CStateManager& mgr, EStateMsg msg, float dt);   // Guessed name
  void HeartStage(CStateManager& mgr, EStateMsg msg, float dt);      // Guessed name
  void BeginHeartStage(CStateManager& mgr, EStateMsg msg, float dt); // Guessed name
  void TrackPlayer(CStateManager& mgr, EStateMsg msg, float dt);     // Guessed name

  bool AllTentaclesRetracted(CStateManager& mgr, const CTriggerData& data) const; // Guessed name
  bool HeartStageOver(CStateManager& mgr, const CTriggerData& data) const;        // Guessed name
  bool IsDead(CStateManager& mgr, const CTriggerData& data) const;                // Guessed name
  bool ShouldMeleeAttack(CStateManager& mgr, const CTriggerData& data) const;     // Guessed name
  bool ShouldAttack(CStateManager& mgr, const CTriggerData& data) const;          // Guessed name
  bool ShouldTaunt(CStateManager& mgr, const CTriggerData& data) const;           // Guessed name

  void BeginSubStage(CStateManager& mgr, float dt);             // Guessed name
  void ResetTentacles(CStateManager& mgr, float dt);            // Guessed name
  void ChooseRandomTentacleCount(CStateManager& mgr, float dt); // Guessed name

  CActor* GetBrainActor(CStateManager& mgr) const;  // Guessed name
  CActor* GetSphereActor(CStateManager& mgr) const; // Guessed name
  int SegIdToTentacleIndex(const CSegId& seg);      // Guessed name
  CCollisionActor* GetTentacleCollisionActor(CStateManager& mgr, int tentacle,
                                             int segment) const;               // Guessed name
  void SetTentacleCollisionActive(CStateManager& mgr, int index, bool active); // Guessed name
  void OnTentacleDestroyed(CStateManager& mgr, const CVector3f& position);     // Guessed name

private:
  friend struct STentacle;

  void UpdateJumpRopeDamage(float dt, CStateManager& mgr);                // Guessed name
  void UpdateMeleeDamage(float dt, CStateManager& mgr);                   // Guessed name
  void SendInternalState(CStateManager& mgr, int phase);                  // Guessed name
  void ResetAttackTimer(CStateManager& mgr);                              // Guessed name
  CVector3f GetBrainLocatorPosition(CStateManager& mgr);                  // Guessed name
  void DeleteBeam(CStateManager& mgr);                                    // Guessed name
  void UpdateBeam(float dt, CStateManager& mgr);                          // Guessed name
  bool CreateBeam(float dt, CStateManager& mgr);                          // Guessed name
  void TurnToSphereForward(CStateManager& mgr);                           // Guessed name
  void SetTurnTarget(CStateManager& mgr, const CVector3f& target);        // Guessed name
  void TurnTowardPlayer(CStateManager& mgr);                              // Guessed name
  void SetAllCollisionMaterials(CStateManager& mgr, bool enable);         // Guessed name
  int CountRetractedTentacles() const;                                    // Guessed name
  void EnableTentacles(bool enable);                                      // Guessed name
  int GetTentacleCount() const;                                           // Guessed name
  CCollisionActor* GetHeartCollisionActor(CStateManager& mgr) const;      // Guessed name
  CActor* GetHeartEffectActor(CStateManager& mgr) const;                  // Guessed name
  void FindAnimations(CStateManager& mgr);                                // Guessed name
  void SetVulnerabilityMode(CStateManager& mgr, EVulnerabilityMode mode); // Guessed name
  void UpdateTentacles(float dt, CStateManager& mgr);                     // Guessed name
  void SetupTentacles(CStateManager& mgr);                                // Guessed name
  void UpdateCollisionActors(float dt, CStateManager& mgr);               // Guessed name
  void DestroyCollisionActors(CStateManager& mgr);                        // Guessed name
  void SetupCollisionActors(CStateManager& mgr);                          // Guessed name
  void UpdateHealth(float dt, CStateManager& mgr);                        // Guessed name
  void UpdateBrain(float dt, CStateManager& mgr);                         // Guessed name
  void UpdateEffects(float dt, CStateManager& mgr);                       // Guessed name
  void ForceExtendTentacles();                                            // Guessed name

  int mPhase;                                                                      // Guessed name
  int mTentacleCountMode;                                                          // Guessed name
  rstl::single_ptr< CCollisionActorManager > mTentacleManager;                     // Guessed name
  rstl::single_ptr< CCollisionActorManager > mHeartManager;                        // Guessed name
  rstl::single_ptr< CCollisionActorManager > mSphereManager;                       // Guessed name
  rstl::reserved_vector< STentacle, 8 > mTentacles;                                // Guessed name
  TUniqueId mDestroyedEffectId;                                                    // Guessed name
  TUniqueId mBrainId;                                                              // Guessed name
  TUniqueId mHeartEffectId;                                                        // Guessed name
  TUniqueId mSphereId;                                                             // Guessed name
  int mIdleAnim;                                                                   // Guessed name
  int mBeginHeartStageAnim;                                                        // Guessed name
  int mEndHeartStageAnim;                                                          // Guessed name
  float mTurnAngle;                                                                // Guessed name
  float mCurrentTurnSpeed;                                                         // Guessed name
  float mMaxTurnSpeed;                                                             // Guessed name
  float mJumpRopeDuration;                                                         // Guessed name
  float mHeartExposedTimer;                                                        // Guessed name
  float mLastHeartHealth;                                                          // Guessed name
  float mWarpTimer;                                                                // Guessed name
  float mDamageCooldown;                                                           // Guessed name
  float mAttackTimer;                                                              // Guessed name
  float mTauntTimer;                                                               // Guessed name
  float mTauntElapsedTime;                                                         // Guessed name
  float mDamageEffectTimer;                                                        // Guessed name
  float mTelegraphEffectTimer;                                                     // Guessed name
  float mBeamTimeLeft;                                                             // Guessed name
  float mHeartBeamTimer;                                                           // Guessed name
  float mBeamLifetime;                                                             // Guessed name
  float mRetractedFraction;                                                        // Guessed name
  int mAttackIndex;                                                                // Guessed name
  bool mMeleeActive : 1;                                                           // Guessed name
  bool mJumpRopeActive : 1;                                                        // Guessed name
  bool mWarpAttackActive : 1;                                                      // Guessed name
  bool mWarpTipsEnabled : 1;                                                       // Guessed name
  bool mProjectileCharged : 1;                                                     // Guessed name
  float mProjectileChargeTimer;                                                    // Guessed name
  rstl::vector< CSegId > mTentacleSegments;                                        // Guessed name
  CSfxHandle mShockwaveSfx;                                                        // Guessed name
  TUniqueId mShockwaveId;                                                          // Guessed name
  CSfxHandle mProjectileChargeSfx;                                                 // Guessed name
  TUniqueId mBeamId;                                                               // Guessed name
  CSfxHandle mBeamSfx;                                                             // Guessed name
  CVector3f mBeamTarget;                                                           // Guessed name
  CHealthInfo mHealthInfo;                                                         // Guessed name
  CDamageVulnerability mNormalVulnerability;                                       // Guessed name
  CDamageVulnerability mWarpVulnerability;                                         // Guessed name
  CDamageVulnerability mMeleeVulnerability;                                        // Guessed name
  CDamageVulnerability mProjectileVulnerability;                                   // Guessed name
  CDamageInfo mMeleeDamage;                                                        // Guessed name
  float mHeartExposedTime;                                                         // Guessed name
  float mSecondPhaseHealth;                                                        // Guessed name
  float mThirdPhaseHealth;                                                         // Guessed name
  float mStayRetractedTime;                                                        // Guessed name
  TCachedToken< CWeaponDescription > mProjectileToken;                             // Guessed name
  CDamageInfo mProjectileDamage;                                                   // Guessed name
  rstl::optional_object< TLockedToken< CGenDescription > > mProjectileVisorEffect; // Guessed name
  float mMaxTurnSpeedNormal;                                                       // Guessed name
  float mMaxTurnSpeedMelee;                                                        // Guessed name
  float mJumpRopeTurnSpeed;                                                        // Guessed name
  float mTurnSpeedAccel;                                                           // Guessed name
  CAssetId mPortalEffect;                                                          // Guessed name
  SLdrAnimationSet mAttackTip;                                                     // Guessed name
  CDamageInfo mStabDamage;                                                         // Guessed name
  ushort mTentacleDamagedSound;                                                    // Guessed name
  ushort mWarpSound;                                                               // Guessed name
  ushort mTipSound;                                                                // Guessed name
  float mTauntFrequency;                                                           // Guessed name
  float mTauntLoopDuration;                                                        // Guessed name
  float mDestroyFraction;                                                          // Guessed name
  CShockWaveInfo mShockWaveInfo;                                                   // Guessed name
  ushort mShockWaveTravelSound;                                                    // Guessed name
  CProjectileInfo mBeamProjectileInfo;                                             // Guessed name
  SLdrPlasmaBeamInfo mBeamInfo;                                                    // Guessed name
  ushort mBeamSound;                                                               // Guessed name
  float mAttackIntervalMin;                                                        // Guessed name
  float mAttackIntervalMax;                                                        // Guessed name
  SLdrAudioPlaybackParms mHeartDamageSound;                                        // Guessed name
};
CHECK_SIZEOF(CEmperorIngStage1, 0xd18)

#endif // _CEMPERORINGSTAGE1
