#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class CollisionDetection {
protected:
public:
   static glm::dvec3 project(glm::dvec3 a, glm::dvec3 b);
   static bool roundedCylinderSphereCollisionIsColliding(
      glm::dvec3 spherePos0, glm::dvec3 spherePos1, double sphereRadius,
      glm::dvec3 cylinderStart0, glm::dvec3 cylinderEnd0,
      glm::dvec3 cylinderStart1, glm::dvec3 cylinderEnd1, double cylinderRadius
   );
};

