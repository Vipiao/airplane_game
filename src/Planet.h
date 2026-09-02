#pragma once

#include "Surface.h"
#include <unordered_map>

class Planet {
protected:
public:
   struct Corner {
      double bottomLeft{};
      double bottomRight{};
      double topLeft{};
      double topRight{};
   };
   Surface* m_surface{};
   unsigned int m_textureNoise{};
   bool m_textureNoiseIsLoaded{ false };
   unsigned int m_textureNoiseNormal{};
   bool m_textureNoiseNormalIsLoaded{ false };
   std::vector<std::vector<double>> texture{};
   const int m_textureSize{ 1024 };
   std::unordered_map<uint64_t, Corner> m_coordsToHeight{};

   Planet();
   ~Planet();
   void writeData(std::string fileName, char* data, int size);
   void readData(std::string fileName, char* data, int size);
   void generateTextures();
   void createHeightMap();
   void setSurface(Surface* surface);
   bool refreshShaders();
   double noise(glm::dvec2 pos);
   double layeredNoise(glm::dvec2 pos, int numLayers);
   /**
    * @brief Calculates ground intersection point for given position and velocity.
    *
    * @param pos Initial position. It should be above the ground.
    * @param vel Velocity vector. It should point below the ground.
    * @param iterations Number of iterations for refining ground intersection point.
    * @return Intersection point on the ground.
    */
   glm::dvec3 findGroundIntersectionPoint(const glm::dvec3& pos, const glm::dvec3& vel, size_t iterations);
   glm::dvec3 map(glm::dvec3 pos, int detailLevel);
   glm::dvec3 mapNormal(glm::dvec3 pos, int detailLevel);
   uint64_t posToKey(glm::uvec2 pos);
};

