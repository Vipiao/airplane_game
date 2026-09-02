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
   uint64_t m_shotIntervalTime{ 6 };

   int64_t m_turretMaxHeat{ 144 * 10 };
   int64_t m_nextTimeCoolDown{ 0 };
   int64_t m_heatPerBullet{ 20 };

   glm::dvec3 m_centerOfMass{};
   uint64_t lastTimeHit{ 0 };
   bool m_doAimAssist{ true };
   double m_shootingSpeed{ 32.* 2. };
   double m_thrust{ 0.0131072 };
   double m_thrustMultiplier{ 1 };
   double m_projectileDamage{ 0.1 };
   double m_scale{ 3.2768 };
   double m_shootSpread{ 0.01 };

   double m_yawMax{ 0.00004 * 1. };
   double m_pitchUpMaxBase{ 0.0002 * 1. };
   double m_pitchUpMax{ m_pitchUpMaxBase };
   double m_pitchDownMax{ 0.0001 * 1. };
   double m_rollMax{ 0.0005 * 1.4 };

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

