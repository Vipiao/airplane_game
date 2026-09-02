#include "Projectile.h"
#include <glm/gtx/projection.hpp>
#include <iostream>

Projectile::Projectile(uint64_t spawnTime) {
   m_spawnTime = spawnTime;
}

Projectile::~Projectile() {}

void Projectile::updateCollisionBoxes() {
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

void Projectile::updateMeshPositions(glm::dvec3 cameraPos, glm::dvec3 cameraVel, uint64_t time) {
   m_mesh->m_position = m_rigidBody->m_pos;

   // Scale as seen from the side.
   glm::dvec3 camDir{ glm::normalize(cameraPos - m_mesh->m_position) };
   glm::dvec3 cameraSideVel{ cameraVel - glm::proj(cameraVel, camDir) * camDir };
   glm::dvec3 relVel{ m_rigidBody->m_vel - cameraVel };
   glm::dvec3 velDir{ glm::normalize(relVel) };
   glm::dvec3 right{ glm::normalize(glm::cross(camDir, velDir)) };
   glm::dvec3 forward{ glm::cross(velDir, right) };
   glm::dquat orientation{ glm::quatLookAt(forward, right) };
   glm::dquat offset{ glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 }) };
   m_mesh->m_orientation = orientation * offset;
   double scale{ 6. };
   m_mesh->m_scale = glm::dvec3{ glm::length(relVel) * 1., scale, scale };

   // Scale as seen from behind.
   glm::dquat orientationBehind{ glm::quatLookAt(glm::normalize(cameraPos - m_mesh->m_position), right) };
   orientationBehind = orientationBehind * offset;
   double scaleBehind{ scale };

   // Mix the two.
   double mixFactor{ glm::abs(glm::dot(camDir, forward)) };
   //mixFactor = 2. * mixFactor - mixFactor * mixFactor;
   //mixFactor = 1. - glm::pow(1. - mixFactor, 64.);
   //std::cout << "mixFactor: " << mixFactor << std::endl;
   //double mixFactorOrientation{ 1. - glm::pow(1. - mixFactor, 2.) };
   //mixFactor = 1.;
   //mixFactor = mixFactor * glm::sqrt(mixFactor);
   m_mesh->m_orientation = glm::slerp(orientationBehind, m_mesh->m_orientation, mixFactor);
   m_mesh->m_scale.x = scaleBehind + (m_mesh->m_scale.x - scaleBehind) * mixFactor;

   // Scale by life time.
   double timeFactor{ 1. - ((double)time - (double)m_spawnTime) / (double)m_lifeTime };
   timeFactor = glm::pow(timeFactor, 0.2);
   m_mesh->m_scale.z *= timeFactor;
}

/*
m_graphicsEngine.m_camOri =
         glm::quatLookAt(direction, glm::dvec3{ 0,0,1 });
const glm::dquat offset{ glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 }) };
*/
