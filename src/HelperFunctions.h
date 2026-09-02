#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

class HelperFunctions {
protected:
public:


   static std::string loadTextFileFromPath(std::string path);
   static std::string vectorVec3ToGeogebra(std::vector<glm::dvec3> verts, std::vector<std::string> names);
   static glm::dvec3 rotateAroundVector(const glm::dvec3& A, const glm::dvec3& B, double angle);
};

