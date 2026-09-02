#include "HelperFunctions.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/projection.hpp>

#include <iostream>
#include <fstream>

#include <numeric>

std::string HelperFunctions::loadTextFileFromPath(std::string path) {
   std::ifstream ifs(path);
   if (ifs.fail()) {
      std::cout << "ERROR: Could not find file at: " << path << std::endl;
      throw std::runtime_error("ERROR: Could not find file at: " + path);
   }
   std::string text((std::istreambuf_iterator<char>(ifs)),
      (std::istreambuf_iterator<char>()));
   return text;
}

std::string HelperFunctions::vectorVec3ToGeogebra(std::vector<glm::dvec3> verts, std::vector<std::string> names) {
   std::vector<std::string> result{};
   result.push_back("Execute({");
   for (size_t ii = 0; ii < verts.size(); ii++) {
      if (ii != 0) {
         result.push_back(",");
      }
      result.push_back("\"" + names[ii] + "_{" + std::to_string(ii) + "}=(" +
         std::to_string(verts[ii].x) + "," +
         std::to_string(verts[ii].y) + "," +
         std::to_string(verts[ii].z) + ")\""
      );
   }
   result.push_back("})");

   return std::accumulate(result.begin(), result.end(), std::string{});
}



glm::dvec3 HelperFunctions::rotateAroundVector(const glm::dvec3& A, const glm::dvec3& B, double angle) {
   // P = B (A B) / (abs(B))^2
   glm::dvec3 P = glm::proj(A, B);

   // D = P - A
   glm::dvec3 D = P - A;

   // U = B cross D / abs(B)
   glm::dvec3 U = glm::cross(B, D) / glm::length(B);

   // R = P - D cos(angle) - U sin(angle)
   glm::dvec3 R = P - D * std::cos(angle) - U * std::sin(angle);

   return R;
}
