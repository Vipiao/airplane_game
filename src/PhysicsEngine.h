#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "RigidBody.h"
#include "GlobalConstants.h"

//class Collider;
//class CollisionBox;
//
//class CollisionCallBack {
//public:
//   virtual void collisionCallback(Collider c0, Collider c1) = 0;
//};
//
//class Collider {
//protected:
//public:
//   enum Type {
//      NONE,
//      SPHERE,
//   };
//   Type m_type{ NONE };
//   uint64_t m_customFlag{0};
//   CollisionBox* m_collisionBox{ nullptr };
//   void updateCollisionBox() {
//      m_collisionBox->m_xMin.m_vPrev = m_collisionBox->m_xMin.m_v;
//      m_collisionBox->m_xMax.m_vPrev = m_collisionBox->m_xMax.m_v;
//      m_collisionBox->m_yMin.m_vPrev = m_collisionBox->m_yMin.m_v;
//      m_collisionBox->m_yMax.m_vPrev = m_collisionBox->m_yMax.m_v;
//      m_collisionBox->m_zMin.m_vPrev = m_collisionBox->m_zMin.m_v;
//      m_collisionBox->m_zMax.m_vPrev = m_collisionBox->m_zMax.m_v;
//   }
//};
//
//class CollisionBox{
//protected:
//public:
//
//   class Edge {
//   protected:
//   public:
//      CollisionBox* m_collisionBox{};
//      double m_v{};
//      double m_vPrev{};
//   };
//   CollisionBox() {
//      m_xMin.m_collisionBox = this;
//      m_xMax.m_collisionBox = this;
//      m_yMin.m_collisionBox = this;
//      m_yMax.m_collisionBox = this;
//      m_zMin.m_collisionBox = this;
//      m_zMax.m_collisionBox = this;
//   }
//
//   Edge m_xMin{};
//   Edge m_xMax{};
//   Edge m_yMin{};
//   Edge m_yMax{};
//   Edge m_zMin{};
//   Edge m_zMax{};
//   Collider* m_collider{};
//};
//
//class SphereCollider : public Collider {
//protected:
//public:
//   SphereCollider() {
//      m_type = SPHERE;
//   }
//   glm::dvec3 m_position{};
//   double m_radius{ 1. };
//   void updateCollisionBox() {
//      Collider::updateCollisionBox();
//      m_collisionBox->m_xMin.m_v = m_position.x - m_radius;
//      m_collisionBox->m_xMax.m_v = m_position.x + m_radius;
//      m_collisionBox->m_yMin.m_v = m_position.y - m_radius;
//      m_collisionBox->m_yMax.m_v = m_position.y + m_radius;
//      m_collisionBox->m_zMin.m_v = m_position.z - m_radius;
//      m_collisionBox->m_zMax.m_v = m_position.z + m_radius;
//   }
//};

class PhysicsEngine {
protected:
   std::vector<RigidBody*> m_rigidBodies{};
   uint64_t m_time{ 0 };
public:
   ~PhysicsEngine();

   void run();
   RigidBody* createRigidBody(glm::dvec3 position);
   void removeRigidBody(RigidBody* rigidBody);

   //double m_gravity{ 0.016384 };
   double m_gravity{ 0.00047309027 * 30. };

   //std::vector<CollisionBox::Edge*> m_edgesX{};
   //std::vector<CollisionBox::Edge*> m_edgesY{};
   //std::vector<CollisionBox::Edge*> m_edgesZ{};

   //void runCollisionDetection();
   //SphereCollider* createSphereCollider(glm::dvec3 position, double radius);
   //void removeCollider(Collider* collider);
};

