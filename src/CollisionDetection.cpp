



#include "CollisionDetection.h"

#include <vector>
#include <string>

#include <glm/gtx/projection.hpp>

#include "HelperFunctions.h"

glm::dvec3 CollisionDetection::project(glm::dvec3 a, glm::dvec3 b) {
   double ll{ glm::length2(b) };
   if (ll == 0) {
      return glm::dvec3{ 0,0,0 };
   }
   return glm::dot(a, b)* b / ll;
}

bool CollisionDetection::roundedCylinderSphereCollisionIsColliding(
   glm::dvec3 spherePos0, glm::dvec3 spherePos1, double sphereRadius,
   glm::dvec3 cylinderStart0, glm::dvec3 cylinderEnd0,
   glm::dvec3 cylinderStart1, glm::dvec3 cylinderEnd1, double cylinderRadius
) {
   //
   double radSum = sphereRadius + cylinderRadius;
   std::string tt{ HelperFunctions::vectorVec3ToGeogebra(
      std::vector<glm::dvec3> {
      spherePos0, spherePos1, cylinderStart0, cylinderEnd0, cylinderStart1, cylinderEnd1
   },
      std::vector<std::string> {
         "SpherePos0", "SpherePos1", "CylinderStart0", "CylinderEnd0", "CylinderStart1", "CylinderEnd1"
      }
   )};
   //
   //std::vector<glm::dvec3> normals{
   //   cylinderStart0 - spherePos0,
   //   cylinderStart1 - spherePos1,
   //   cylinderEnd0 - spherePos0,
   //   cylinderEnd1 - spherePos1,
   //   CollisionDetection::project(spherePos0 - cylinderStart0, cylinderEnd0 - cylinderStart0) +
   //      cylinderStart0 - spherePos0,
   //   CollisionDetection::project(spherePos1 - cylinderStart1, cylinderEnd1 - cylinderStart1) +
   //      cylinderStart1 - spherePos1,
   //   (cylinderStart0 + cylinderEnd0 + cylinderStart1 + cylinderEnd1) * 0.5 - (spherePos0 + spherePos1)
   //   /*glm::cross(
   //      spherePos1 - cylinderStart1 - (spherePos0 - cylinderStart0),
   //      cylinderEnd0 - cylinderStart0
   //   )*/
   //};
   glm::dvec3 cross{
      glm::cross(spherePos1 - spherePos0 - (cylinderStart1 - cylinderStart0), cylinderEnd0 - cylinderStart0)
   };
   std::vector<glm::dvec3> normals{
      -glm::proj(
         cylinderStart0 -spherePos0, spherePos1 - spherePos0 + cylinderStart0 - cylinderStart1
      ) - spherePos0 + cylinderStart0,
      -glm::proj(
         cylinderEnd0 -spherePos0, spherePos1 - spherePos0 + cylinderEnd0 - cylinderEnd1
      ) - spherePos0 + cylinderEnd0,
      cross,
      -cross
   };

   for (size_t ii = 0; ii < normals.size(); ii++) {
      double ll{ glm::length(normals[ii]) };
      if (ll == 0) {
         continue;
      }
      glm::dvec3 nn{ normals[ii] / ll };
      double dS{ glm::dot(spherePos0, nn) };
      double d0{ glm::dot(cylinderStart0, nn) - dS };
      double d1{ glm::dot(cylinderEnd0, nn) - dS };
      if (glm::min(d0, d1) > radSum) {
         double dS_{ glm::dot(spherePos1, nn) };
         double d0_{ glm::dot(cylinderStart1, nn) - dS_ };
         double d1_{ glm::dot(cylinderEnd1, nn) - dS_ };
         if (glm::min(d0_, d1_) > radSum) {
            return false;
         }
      }
   }

   
   
   return true;
}
