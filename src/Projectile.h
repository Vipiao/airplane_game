#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Mesh.h"
#include "RigidBody.h"
#include "GlobalConstants.h"
#include "PhysicsUnits.h"

class Projectile {
protected:
public:
   Projectile(uint64_t spawnTime);
   ~Projectile();

   void updateCollisionBoxes();
   void updateMeshPositions(glm::dvec3 cameraPos, glm::dvec3 cameraVel, uint64_t time);
   RigidBody* m_rigidBody{ nullptr };
   Mesh* m_mesh{ nullptr };
   uint64_t m_spawnTime{0};
   uint64_t m_lifeTime{ (uint64_t)PhysicsUnits::ticks(3.6) };
   int m_owner{-1};
   double m_damage{ 0.3 };
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
   double m_size{ 4. };
};

