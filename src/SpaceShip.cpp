#include "SpaceShip.h"

#include "Hash.h"

int SpaceShip::m_numSpaceShips{0};

SpaceShip::SpaceShip() {
   m_id = m_numSpaceShips;
   m_numSpaceShips++;
}

void SpaceShip::applyTorqueCapped(glm::dvec3 torqueLocal) {
   
   if (torqueLocal.z > m_yawMax) {
      torqueLocal.z = m_yawMax;
   } else if (torqueLocal.z < -m_yawMax) {
      torqueLocal.z = -m_yawMax;
   }
   if (torqueLocal.x > 0) {
      if (torqueLocal.x > m_pitchUpMax) {
         torqueLocal.x = m_pitchUpMax;
      }
   } else {
      if (torqueLocal.x < -m_pitchDownMax) {
         torqueLocal.x = -m_pitchDownMax;
      }
   }
   if (torqueLocal.y > m_rollMax) {
      torqueLocal.y = m_rollMax;
   } else if (torqueLocal.y < -m_rollMax) {
      torqueLocal.y = -m_rollMax;
   }

   //
   //constexpr double minimum{ 0.5 };
   //torqueLocal.y *=
   //   (minimum + glm::max(m_spaceShipParts[1].m_health, 0.) / m_spaceShipParts[1].m_maxHealth * (1.- minimum)) *
   //   (minimum + glm::max(m_spaceShipParts[4].m_health, 0.) / m_spaceShipParts[4].m_maxHealth * (1. - minimum));
   double rWingHealth{ glm::max(m_spaceShipParts[1].m_health, 0.) / m_spaceShipParts[1].m_maxHealth };
   double lWingHealth{ glm::max(m_spaceShipParts[4].m_health, 0.) / m_spaceShipParts[4].m_maxHealth };
   torqueLocal.y *=
      1. - glm::pow(1. - rWingHealth, 2.) *
      1. - glm::pow(1. - lWingHealth, 2.);
   //double sideWaysFactor{
   //   1. - glm::pow(1. - rWingHealth, 4.) *
   //   1. - glm::pow(1. - lWingHealth, 4.)
   //};
   //torqueLocal.x *= sideWaysFactor;
   //torqueLocal.z *= sideWaysFactor;

   //
   applyTorque(m_rigidBody->m_ori * torqueLocal);
}

void SpaceShip::applyTorque(glm::dvec3 torque) {
   m_rigidBody->applyTorque(torque);
}

void SpaceShip::updateMeshPositions(uint64_t frameNum, glm::dvec3 camPos, double fov) {
   for (size_t ii = 0; ii < m_spaceShipParts.size(); ii++) {
      SpaceShipPart* spaceShipPart{ &m_spaceShipParts[ii] };
      spaceShipPart->m_meshCurrent->m_position = m_rigidBody->m_pos +
         m_rigidBody->m_ori * (spaceShipPart->m_offset - m_centerOfMass);
      spaceShipPart->m_meshCurrent->m_orientation = m_rigidBody->m_ori;
   }

   // Plasma.
   glm::dvec3 offset{ 10.,-7.86432,-2.29376 };
   m_plasmaR->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * offset;
   offset.x = -offset.x;
   m_plasmaL->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * offset;
   glm::dquat orientation{ m_rigidBody->m_ori };
   double random{ Hash::pcgUnit(frameNum) };
   if (random > 0.5) {
      m_plasmaR->m_scale.x = -m_plasmaR->m_scale.x;
      m_plasmaL->m_scale.x = -m_plasmaL->m_scale.x;
   }
   orientation = orientation * glm::angleAxis(random * 99., glm::dvec3{ 0,1,0 });
   m_plasmaL->m_orientation = orientation;
   m_plasmaR->m_orientation = orientation;

   // Plasma effect.
   offset.x = -offset.x;
   m_plasmaEffectR->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * offset;
   offset.x = -offset.x;
   m_plasmaEffectL->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * offset;
   //orientation{ m_rigidBody->m_ori };
   //double random{ Hash::pcgUnit(frameNum) };
   //if (random > 0.5) {
   //   m_plasmaR->m_scale.x = -m_plasmaR->m_scale.x;
   //   m_plasmaL->m_scale.x = -m_plasmaL->m_scale.x;
   //}
   //orientation = orientation * glm::angleAxis(random * 99., glm::dvec3{ 0,1,0 });
   orientation = orientation * glm::angleAxis(glm::radians(180.), glm::dvec3{1,0,0});
   m_plasmaEffectR->m_orientation = orientation;
   m_plasmaEffectL->m_orientation = orientation;

   // Debug collision cylinders.
   for (size_t ii = 0; ii < m_spaceShipParts.size(); ii++) {
      SpaceShipPart* spaceShipPart{ &m_spaceShipParts[ii] };
      
      for (size_t jj = 0; jj < spaceShipPart->m_collisionSpheres.size(); jj++) {
         SpaceShipPart::CollisionSphere* collisionSphere{ &spaceShipPart->m_collisionSpheres[jj] };
         Mesh* mesh0{ collisionSphere->m_mesh0 };
         Mesh* mesh1{ collisionSphere->m_mesh1 };
         if (mesh0 == nullptr) {
            continue;
         }
         mesh0->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * collisionSphere->m_pos0;
         mesh1->m_position = m_rigidBody->m_pos + m_rigidBody->m_ori * collisionSphere->m_pos1;
         double ss{ collisionSphere->m_radius };
         mesh0->m_scale = { ss,ss,ss };
         mesh1->m_scale = { ss,ss,ss };
      }
   }

   // Make ship visible from further away.
   double distance{ glm::length(camPos - m_rigidBody->m_pos) };
   double scale{ 1. };
   //bool tt{ false };
   double limit{ 12000.0 / fov };
   if (distance > limit) {
      scale = distance / limit;
   }
   scale *= m_scale;
   glm::dvec3 scaleV{ scale, scale, scale };
   for (size_t ii = 0; ii < m_spaceShipParts.size(); ii++) {
      SpaceShipPart* ssp{ &m_spaceShipParts[ii] };
      ssp->m_meshCurrent->m_scale = scaleV;
   }

   //
   double current{ m_plasmaEffectR->m_scale.y };
   double target{
      m_plasmaEffectL->m_scale.x *
      glm::pow(glm::abs(m_thrustMultiplier), 2.5)
   };
   double newL{ glm::mix(current, target, PhysicsUnits::blendHalfLife(0.033129419504020347)) };
   m_plasmaEffectR->m_scale.y = newL;
   m_plasmaEffectL->m_scale.y = newL;
}

glm::dvec3 SpaceShip::getShootingPosition() {
   return m_rigidBody->m_pos + m_rigidBody->m_ori * m_shootingPosition;
}

void SpaceShip::nextShootingPosition() {
   m_shootingPosition.x = -m_shootingPosition.x;
}

void SpaceShip::updateCenterOfMass() {
   m_centerOfMass = glm::dvec3{ 0,0,0 };
   double mass{ 0. };
   for (size_t ii = 0; ii < m_spaceShipParts.size(); ii++) {
      if (m_spaceShipParts[ii].m_health > 0.) {
         m_centerOfMass += m_spaceShipParts[ii].m_offset;
         mass += 1.;
      }
   }
   m_centerOfMass /= mass;
   m_centerOfMass.z += 3.2768; // Due to multiple smalle models at the bottom.
}

void SpaceShip::updateCollisionBoxes() {
   m_rightPrev = m_right;
   m_leftPrev = m_left;
   m_frontPrev = m_front;
   m_backPrev = m_back;
   m_topPrev = m_top;
   m_bottomPrev = m_bottom;

   m_right = m_rigidBody->m_pos.x + m_size;
   m_left = m_rigidBody->m_pos.x - m_size;
   m_front = m_rigidBody->m_pos.y + m_size;
   m_back = m_rigidBody->m_pos.y - m_size;
   m_top = m_rigidBody->m_pos.z + m_size;
   m_bottom = m_rigidBody->m_pos.z - m_size;
}
