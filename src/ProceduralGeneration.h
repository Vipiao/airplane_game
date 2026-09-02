#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/integer.hpp>


class ProceduralGeneration {
protected:
public:
   static double boxNoise(glm::dvec3 pos, int64_t tileSize);
   static double layeredBoxNoise(glm::dvec3 pos, int numLayers, int64_t tileSize);
   static double layeredWarpedBoxNoise(glm::dvec3 pos, double warpSize, int numLayers, int64_t tileSize);
   static double layeredWarpedBoxNoise2(glm::dvec3 pos, double warpSize, int numLayers, int64_t tileSize);
};

