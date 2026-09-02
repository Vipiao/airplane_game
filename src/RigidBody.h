#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class RigidBody {
protected:
public:
   RigidBody(glm::dvec3 position);

   glm::dvec3 getAngularVel();
   void applyTorque(glm::dvec3 torque);

   glm::dvec3 m_pos{ 0,0,0 };
   glm::dvec3 m_posPrev{ 0,0,0 };
   glm::dvec3 m_vel{ 0,0,0 };
   glm::dvec3 m_rotationAxis{ 0,0,0 };
   double m_rotationVel{ 0. };
   glm::dquat m_ori{ 1,0,0,0 };
   glm::dquat m_oriPrev{ 1,0,0,0 };
   double m_mass{1.};

};