#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mesh.h"
#include "RigidBody.h"
#include "ParticleEffectInstance.h"
#include "GlobalConstants.h"
#include "PhysicsUnits.h"

class SpaceShipPart {
protected:
public:
   class CollisionSphere {
   public:
      Mesh* m_mesh0{};
      Mesh* m_mesh1{};
      glm::dvec3 m_pos0{};
      glm::dvec3 m_pos1{};
      double m_radius{};
   };
   Mesh* m_meshCurrent{ nullptr };
   Mesh* m_meshHealthMax{ nullptr };
   Mesh* m_meshHealthMedium{ nullptr };
   Mesh* m_meshHealthLow{ nullptr };
   glm::dvec3 m_offset{0,0,0};
   double m_maxHealth{ 1. };
   double m_health{ 1. };
   std::vector<CollisionSphere> m_collisionSpheres{};
};

class SpaceShip {
protected:
public:
   enum AIState {
      CHACE,
      SHOOT,
      ESCAPE
   } m_aiState{ESCAPE};

   SpaceShip();
   void applyTorqueCapped(glm::dvec3 torqueLocal);
   void applyTorque(glm::dvec3 torque);
   void updateMeshPositions(uint64_t frameNum, glm::dvec3 camPos, double fov);
   glm::dvec3 getShootingPosition();
   void nextShootingPosition();
   void updateCenterOfMass();
   void updateCollisionBoxes();
   void updateCapacitor();
   double getEnginePower();
   double getEngineDuty();
   double getEnginePitchScale();

   int m_id{};
   int m_lastDamageId{-1};
   int m_numKills{ 0 };
   static int m_numSpaceShips;
   Mesh* m_plasmaL{ nullptr };
   Mesh* m_plasmaR{ nullptr };
   ParticleEffectInstance* m_plasmaEffectR{ nullptr };
   ParticleEffectInstance* m_plasmaEffectL{ nullptr };
   Mesh* m_res_0{ nullptr }; // Lowest res.
   Mesh* m_res_1{ nullptr }; // Higher res.
   //std::vector<Mesh*> meshes{};
   //std::vector<glm::dvec3> meshOffsets{};
   std::vector<SpaceShipPart> m_spaceShipParts{};
   RigidBody* m_rigidBody{ nullptr };
   glm::dvec3 m_shootingPosition{ 6.5536,0,0 };

   uint64_t m_nextTimeToShoot{ 0 };
   uint64_t m_shotIntervalTime{ (uint64_t)PhysicsUnits::ticks(0.05) };

   int64_t m_turretMaxHeat{ PhysicsUnits::ticks(12.) };
   int64_t m_nextTimeCoolDown{ 0 };
   int64_t m_heatPerBullet{ PhysicsUnits::ticks(1. / 6.) };

   glm::dvec3 m_centerOfMass{};
   uint64_t lastTimeHit{ 0 };
   bool m_doAimAssist{ true };
   double m_shootingSpeed{ PhysicsUnits::metersPerSecond(7680.) };
   double m_thrust{ PhysicsUnits::metersPerSecondSquared(188.74368) };
   double m_thrustMultiplier{ 1 };
   double m_boostThrust{ 2. };
   double m_sustainThrust{ 1.3 };
   double m_capacitorMax{ 10. }; // Seconds of full boost.
   double m_capacitor{ m_capacitorMax };
   double m_capacitorChargeScale{ 2.2 };
   double m_plasmaScale{ 4.9152 };
   int64_t m_enginePulsePeriod{ PhysicsUnits::ticks(0.1) };
   double m_engineMinDuty{ 0.5 };
   double m_engineMinPower{ 0.5 };
   double m_engineOffPower{ 0.35 };
   double m_engineGlowPulseDepth{ 0.4 };
   double m_engineMinPitch{ 0.35 };
   double m_projectileDamage{ 0.1 };
   double m_scale{ 3.2768 };
   double m_shootSpread{ 0.01 };

   double m_yawMax{ PhysicsUnits::radiansPerSecondSquared(0.576) };
   double m_pitchUpMaxBase{ PhysicsUnits::radiansPerSecondSquared(2.88) };
   double m_pitchUpMax{ m_pitchUpMaxBase };
   double m_pitchDownMax{ PhysicsUnits::radiansPerSecondSquared(1.44) };
   double m_rollMax{ PhysicsUnits::radiansPerSecondSquared(10.08) };

   // Collision.
   double m_right{};
   double m_left{};
   double m_front{};
   double m_back{};
   double m_top{};
   double m_bottom{};

   double m_rightPrev{};
   double m_leftPrev{};
   double m_frontPrev{};
   double m_backPrev{};
   double m_topPrev{};
   double m_bottomPrev{};
   double m_size{1.};
};

