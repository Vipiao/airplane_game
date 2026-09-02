



#include "ProceduralGeneration.h"

#include "Hash.h"

#include <glm/gtc/integer.hpp>

double ProceduralGeneration::boxNoise(glm::dvec3 pos, int64_t tileSize) {
   glm::i64vec3 ind{ glm::floor(pos) };
   glm::i64vec3 indDiff{ ind + glm::i64vec3{1,1,1} };
   // Set tileSize to -1 to not do tiling.
   if (tileSize > 0) {
      ind = ind - ind / tileSize * tileSize;
      if (ind.x < 0) ind.x += tileSize;
      if (ind.y < 0) ind.y += tileSize;
      if (ind.z < 0) ind.z += tileSize;
      indDiff = indDiff - indDiff / tileSize * tileSize;
      if (indDiff.x < 0) indDiff.x += tileSize;
      if (indDiff.y < 0) indDiff.y += tileSize;
      if (indDiff.z < 0) indDiff.z += tileSize;
   }
   indDiff = indDiff - ind;
   glm::dvec3 f{ glm::fract(pos) };

   glm::dvec3 u = f * f * (3.0 - 2.0 * f);

   return glm::mix(
      glm::mix(
         glm::mix(
            Hash::pcgUnit(ind + glm::i64vec3(        0, 0, 0)),
            Hash::pcgUnit(ind + glm::i64vec3(indDiff.x, 0, 0)), u.x
         ),
         glm::mix(
            Hash::pcgUnit(ind + glm::i64vec3(        0, indDiff.y, 0)),
            Hash::pcgUnit(ind + glm::i64vec3(indDiff.x, indDiff.y, 0)), u.x
         ), u.y
      ),
      glm::mix(
         glm::mix(
            Hash::pcgUnit(ind + glm::i64vec3(        0, 0, indDiff.z)),
            Hash::pcgUnit(ind + glm::i64vec3(indDiff.x, 0, indDiff.z)), u.x
         ),
         glm::mix(
            Hash::pcgUnit(ind + glm::i64vec3(        0, indDiff.y, indDiff.z)),
            Hash::pcgUnit(ind + glm::i64vec3(indDiff.x, indDiff.y, indDiff.z)), u.x
         ), u.y
      ), u.z);
}

double ProceduralGeneration::layeredBoxNoise(glm::dvec3 pos, int numLayers, int64_t tileSize) {
   double ff{ 1. };
   double result{ 0. };
   int64_t currentTileSize{ tileSize };
   for (size_t ii = 0; ii < numLayers; ii++) {
      result += ProceduralGeneration::boxNoise(pos * ff + ff*0.5, currentTileSize) / ff;
      ff *= 2;
      currentTileSize *= 2;
   }
   return result;
}

double ProceduralGeneration::layeredWarpedBoxNoise(glm::dvec3 pos, double warpSize, int numLayers, int64_t tileSize) {
    glm::dvec3 shift{
        ProceduralGeneration::boxNoise(glm::dvec3{pos.x, pos.y, 0.0} * 2., tileSize),
        ProceduralGeneration::boxNoise(glm::dvec3{pos.x, pos.y, 1.0} * 2., tileSize),
        ProceduralGeneration::boxNoise(glm::dvec3{pos.x, pos.y, 2.0} * 2., tileSize)
    };
    return ProceduralGeneration::layeredBoxNoise(pos + shift * warpSize, numLayers, tileSize);
}

double ProceduralGeneration::layeredWarpedBoxNoise2(glm::dvec3 pos, double warpSize, int numLayers, int64_t tileSize) {
   glm::dvec3 shift{
       ProceduralGeneration::layeredBoxNoise(glm::dvec3{pos.x, pos.y, 0.0} *2., numLayers, tileSize),
       ProceduralGeneration::layeredBoxNoise(glm::dvec3{pos.x, pos.y, 1.0} *2., numLayers, tileSize),
       ProceduralGeneration::layeredBoxNoise(glm::dvec3{pos.x, pos.y, 2.0} *2., numLayers, tileSize)
   };
   return ProceduralGeneration::boxNoise(pos + shift * warpSize, tileSize);
}


