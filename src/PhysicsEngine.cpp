







#include "PhysicsEngine.h"

#include "CollisionDetection.h"
#include "GlobalConstants.h"

PhysicsEngine::~PhysicsEngine() {
   for (size_t ii = 0; ii < m_rigidBodies.size(); ii++) {
      delete m_rigidBodies[ii];
   }
}

void PhysicsEngine::run() {

   // Simulate.
   // Acceleration.
   for (size_t ii = 0; ii < m_rigidBodies.size(); ii++) {
      m_rigidBodies[ii]->m_vel.z -= m_gravity;
   }

   // Movement.
   for (size_t ii = 0; ii < m_rigidBodies.size(); ii++) {
      RigidBody* rigidBody = m_rigidBodies[ii];
      rigidBody->m_posPrev = rigidBody->m_pos;
      rigidBody->m_pos += rigidBody->m_vel;
      glm::dvec3 rotationAxis{};
      double rotationAxisLength = glm::length(rigidBody->m_rotationAxis);
      if (rotationAxisLength > 0.0) {
         rotationAxis = rigidBody->m_rotationAxis / rotationAxisLength;
      } else {
         rotationAxis = glm::dvec3{ 1,0,0 };
      }
      rigidBody->m_oriPrev = rigidBody->m_ori;
      rigidBody->m_ori = glm::angleAxis(rigidBody->m_rotationVel, rotationAxis) * rigidBody->m_ori;
   }

   // Collision.
}

RigidBody* PhysicsEngine::createRigidBody(glm::dvec3 position) {
   RigidBody* rigidBody = new RigidBody(position);
   m_rigidBodies.push_back(rigidBody);

   return rigidBody;
}

void PhysicsEngine::removeRigidBody(RigidBody* rigidBody) {
   for (size_t ii = 0; ii < m_rigidBodies.size(); ii++) {
      if (rigidBody == m_rigidBodies[ii]) {
         if (ii != m_rigidBodies.size() - 1) {
            m_rigidBodies[ii] = m_rigidBodies[m_rigidBodies.size() - 1];
         }
         m_rigidBodies.pop_back();
         delete rigidBody;
      }
   }
}

//void PhysicsEngine::runCollisionDetection() {
//   std::vector<std::vector<CollisionBox::Edge*>> dimensions{ m_edgesX, m_edgesY, m_edgesZ };
//   // Update all edges according to the colliders.
//   for (size_t ii = 0; ii < m_edgesX.size(); ii++) {
//      CollisionBox::Edge* edge{ m_edgesX[ii] };
//      if (&edge->m_collisionBox->m_xMax == edge) {
//         continue; // Dont run the update on both min and max as it is redundant.
//      }
//      edge->m_collisionBox->m_collider->updateCollisionBox();
//   }
//   // Insertion sort collision boxes to check for collision.
//   for (size_t ii = 0; ii < dimensions.size(); ii++) {
//      std::vector<CollisionBox::Edge*>* dimension{ &dimensions[ii] };
//      for (size_t jj = 0; jj < (*dimension).size()-1; jj++) {
//         if ((*dimension)[jj]->m_v > (*dimension)[jj+1]->m_v) {
//            size_t kk{ jj };
//            do {
//               // Check for collision.
//               CollisionBox* cbA{ (*dimension)[jj]->m_collisionBox };
//               CollisionBox* cbB{ (*dimension)[jj+1]->m_collisionBox };
//               if (!(
//                  cbA->m_xMin.m_v >= cbB->m_xMax.m_v && cbA->m_xMin.m_vPrev >= cbB->m_xMax.m_vPrev ||
//                  cbA->m_xMax.m_v <= cbB->m_xMin.m_v && cbA->m_xMax.m_vPrev <= cbB->m_xMin.m_vPrev ||
//                  cbA->m_yMin.m_v >= cbB->m_yMax.m_v && cbA->m_yMin.m_vPrev >= cbB->m_yMax.m_vPrev ||
//                  cbA->m_yMax.m_v <= cbB->m_yMin.m_v && cbA->m_yMax.m_vPrev <= cbB->m_yMin.m_vPrev ||
//                  cbA->m_zMin.m_v >= cbB->m_zMax.m_v && cbA->m_zMin.m_vPrev >= cbB->m_zMax.m_vPrev ||
//                  cbA->m_zMax.m_v <= cbB->m_zMin.m_v && cbA->m_zMax.m_vPrev <= cbB->m_zMin.m_vPrev
//                  )) {
//                  /*
//                  std::unordered_map<uint64_t, int> coordsToIndices{};
//                  auto toKey = [](glm::ivec2 pos) -> uint64_t {
//                     return (uint64_t)(pos.x < 0 ? 1 : 0) | (uint64_t)abs(pos.x) << 1 |
//                        (uint64_t)(pos.y < 0 ? 1 : 0) << 32 | (uint64_t)abs(pos.y) << 33;
//                  };
//                  */
//               }
//               //
//               CollisionBox::Edge* temp{ (*dimension)[kk] };
//               (*dimension)[kk] = (*dimension)[kk + 1];
//               (*dimension)[kk + 1] = temp;
//               kk++;
//            } while ((*dimension)[kk]->m_v > (*dimension)[kk+1]->m_v);
//         }
//      }
//   }
//}
//
//SphereCollider* PhysicsEngine::createSphereCollider(glm::dvec3 position, double radius) {
//   SphereCollider* sc{ new SphereCollider() };
//   sc->m_position = position;
//   sc->m_radius = radius;
//   CollisionBox* cb{ new CollisionBox() };
//   cb->m_collider = sc;
//   sc->m_collisionBox = cb;
//   m_edgesX.push_back(&cb->m_xMin);
//   m_edgesX.push_back(&cb->m_xMax);
//   m_edgesY.push_back(&cb->m_yMin);
//   m_edgesY.push_back(&cb->m_yMax);
//   m_edgesZ.push_back(&cb->m_zMin);
//   m_edgesZ.push_back(&cb->m_zMax);
//
//   sc->updateCollisionBox();
//   sc->updateCollisionBox(); // Call it twice to update the previous positions too.
//
//   return sc;
//}
//
//void PhysicsEngine::removeCollider(Collider* collider) {
//   CollisionBox* collisionBox{ collider->m_collisionBox };
//   std::vector<std::vector<CollisionBox::Edge*>> dimensions{m_edgesX, m_edgesY, m_edgesZ};
//   for (size_t ii = 0; ii < dimensions.size(); ii++) {
//      std::vector<CollisionBox::Edge*>* dimension{ &dimensions[ii] };
//      for (size_t jj = 0; jj < (*dimension).size(); jj++) {
//         CollisionBox::Edge* edge{ (*dimension)[jj] };
//         if ((*edge).m_collisionBox == collisionBox) {
//            (*dimension).erase((*dimension).begin() + jj);
//            jj--;
//         }
//      }
//   }
//   delete collider;
//   delete collisionBox;
//}
