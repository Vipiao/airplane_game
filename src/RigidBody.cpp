

#include "RigidBody.h"



RigidBody::RigidBody(glm::dvec3 position) {
   m_pos = position;
   m_posPrev = position;
   m_oriPrev = m_ori;
}

glm::dvec3 RigidBody::getAngularVel() {
   double axisLength{ glm::length(m_rotationAxis) };
   if (axisLength > 0.) {
      return m_rotationAxis / axisLength * m_rotationVel;
   } else {
      return glm::dvec3{ 0,0,0 };
   }
}

void RigidBody::applyTorque(glm::dvec3 torque) {
   double axisLength{ glm::length(m_rotationAxis) };
   if (axisLength > 0.) {
      glm::dvec3 angVel{ m_rotationAxis / axisLength * m_rotationVel };
      angVel += torque;
      m_rotationAxis = angVel;
      m_rotationVel = glm::length(angVel);
   } else {
      m_rotationAxis = torque;
      m_rotationVel = glm::length(torque);
      return;
   }
}
