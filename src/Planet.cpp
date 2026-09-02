






#include "Planet.h"

#include "ProceduralGeneration.h"
#include "GlobalConstants.h"
#include "HelperFunctions.h"

#include <iostream>
#include <fstream>
#include <unordered_map>

Planet::Planet() {
   generateTextures();
   createHeightMap();
}

Planet::~Planet() {
   //std::cout << "~Planet()" << std::endl;
   if (m_textureNoiseIsLoaded) {
      glDeleteTextures(1, &m_textureNoise);
   }
   if (m_textureNoiseNormalIsLoaded) {
      glDeleteTextures(1, &m_textureNoiseNormal);
   }
}

void Planet::writeData(std::string fileName, char* data, int size) {

   std::ofstream fout{};
   try {
      fout.open(fileName, std::ios::binary);
      fout.write(data, size);
   } catch (const std::exception&) {
      fout.close();
      std::cout << "ERROR: Error writing to \"" + fileName + "\"." << std::endl;
      throw "ERROR: Error writing to \"" + fileName + "\".";
   }
   fout.close();
}

void Planet::readData(std::string fileName, char* data, int size) {
   std::ifstream fin{};
   try {
      fin.open(fileName, std::ios::binary);
      fin.read(data, size);
   } catch (const std::exception&) {
      fin.close();
      std::cout << "ERROR: Error reading from \"" + fileName + "\"." << std::endl;
      throw "ERROR: Error reading from \"" + fileName + "\".";
   }
   fin.close();
}

void Planet::generateTextures() {

   // Create texture noise.
   glGenTextures(1, &m_textureNoise);
   m_textureNoiseIsLoaded = true;
   glBindTexture(GL_TEXTURE_2D, m_textureNoise);
   // set the texture wrapping parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   //const int size{ 1024 };
   float* noiseData = new float[m_textureSize * m_textureSize * 4];
#ifdef GENERATE_DATA
   float scale{ 0.5f };
   int numLayers{ 6 };
   double warpScale{ 0.2 };
   std::vector<glm::ivec2> offsets{ {0,0},{1,0},{0,1}, {1,1}, };
   for (size_t yy = 0; yy < m_textureSize; yy++) {
      for (size_t xx = 0; xx < m_textureSize; xx++) {
         for (size_t ii = 0; ii < offsets.size(); ii++) {
            noiseData[xx * 4 + yy * m_textureSize * 4 + ii] = scale * (float)ProceduralGeneration::layeredWarpedBoxNoise(
               glm::dvec3(offsets[ii].x + xx, offsets[ii].y + yy, 0) / 128., warpScale, numLayers, m_textureSize / 128
            );
         }
      }
   }
   writeData("temp/noiseData.dat", reinterpret_cast<char*>(noiseData), sizeof(float) * m_textureSize * m_textureSize * 4);
#else // GENERATE_DATA
   readData("temp/noiseData.dat", reinterpret_cast<char*>(noiseData), sizeof(float) * m_textureSize * m_textureSize * 4);
#endif // GENERATE_DATA
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureSize, m_textureSize, 0, GL_RGBA, GL_FLOAT, noiseData);
   glGenerateMipmap(GL_TEXTURE_2D);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -10);
   delete[] noiseData;

   // Create texture noise normal.
   glGenTextures(1, &m_textureNoiseNormal);
   m_textureNoiseNormalIsLoaded = true;
   glBindTexture(GL_TEXTURE_2D, m_textureNoiseNormal);
   // set the texture wrapping parameters.
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
   //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

   float* noiseNormalData = new float[m_textureSize * m_textureSize * 4];
#ifdef GENERATE_DATA
   double diff{ 0.0001 };
   for (size_t yy = 0; yy < m_textureSize; yy++) {
      for (size_t xx = 0; xx < m_textureSize; xx++) {
         double ff = scale * ProceduralGeneration::layeredWarpedBoxNoise(
            glm::dvec3((double)xx, (double)yy, 0) / 128., warpScale, numLayers, m_textureSize / 128
         );
         double ffx = scale * ProceduralGeneration::layeredWarpedBoxNoise(
            glm::dvec3(diff + (double)xx, (double)yy, 0) / 128., warpScale, numLayers, m_textureSize / 128
         );
         double ffy = scale * ProceduralGeneration::layeredWarpedBoxNoise(
            glm::dvec3((double)xx, diff + (double)yy, 0) / 128., warpScale, numLayers, m_textureSize / 128
         );
         glm::dvec3 normal{ glm::normalize(glm::dvec3{
            -(ffx - ff) / diff,
            -(ffy - ff) / diff,
            0.001
         }) };
         // Make normal from [-1, 1] to [0, 1] since this is the span supported by textures.
         if (xx + yy % 100 == 0) {
            //std::cout << normal.x << std::endl;
            //std::cout << normal.y << std::endl;
            //std::cout << normal.z << std::endl << std::endl;
         }
         normal = (normal + 1.) / 2.;
         noiseNormalData[xx * 4 + yy * m_textureSize * 4 + 0] = (float)normal.x;
         noiseNormalData[xx * 4 + yy * m_textureSize * 4 + 1] = (float)normal.y;
         noiseNormalData[xx * 4 + yy * m_textureSize * 4 + 2] = (float)normal.z;
         noiseNormalData[xx * 4 + yy * m_textureSize * 4 + 3] = 0.f;
      }
   }
   writeData("temp/noiseDataNormal.dat", reinterpret_cast<char*>(noiseNormalData), sizeof(float) * m_textureSize * m_textureSize * 4);
#else // GENERATE_DATA
   readData("temp/noiseDataNormal.dat", reinterpret_cast<char*>(noiseNormalData), sizeof(float) * m_textureSize * m_textureSize * 4);
#endif // GENERATE_DATA
   glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_textureSize, m_textureSize, 0, GL_RGBA, GL_FLOAT, noiseNormalData);
   delete[] noiseNormalData;
   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

   glGenerateMipmap(GL_TEXTURE_2D);
   glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -10);

   // Save generated noise.
}

void Planet::createHeightMap() {
   float* noiseData = new float[m_textureSize * m_textureSize * 4];
   readData("temp/noiseData.dat", reinterpret_cast<char*>(noiseData), sizeof(float) * m_textureSize * m_textureSize * 4);
   // noiseData shape is (width, height, 4 depth channels).
   // The 4 channels represent the depth of the 4 corners of the cell.
   for (size_t ii = 0; ii < m_textureSize * m_textureSize * 4; ii++) {
      // The noise data uploaded to the gpu buffer only has 8 bit precision,
      // so this must be reflected in the height map too.
      noiseData[ii] = glm::round(noiseData[ii] * 255.f) / 255.f;
   }
   for (size_t xx = 0; xx < m_textureSize; xx++) {
      for (size_t yy = 0; yy < m_textureSize; yy++) {
         Corner corner{
            noiseData[xx * 4 + yy * m_textureSize * 4 + 0], // Bottom left.
            noiseData[xx * 4 + yy * m_textureSize * 4 + 1], // Bottom right.
            noiseData[xx * 4 + yy * m_textureSize * 4 + 2], // Top left.
            noiseData[xx * 4 + yy * m_textureSize * 4 + 3], // Top Right.
         };
         m_coordsToHeight[posToKey(glm::uvec2{ xx, yy })] = corner;
      }
   }
}

void Planet::setSurface(Surface* surface) {
   m_surface = surface;
   bool success{ refreshShaders() };
   if (!success) {
       std::cout << "ERROR: Could not compile shader." << std::endl;
       throw "ERROR: Could not compile shader.";
   }
}

bool Planet::refreshShaders() {
   std::string mapFunction{ HelperFunctions::loadTextFileFromPath("common.glsl") };
   bool success{ m_surface->setProceduralGenerationFunction(mapFunction) };
   if (!success) {
       return false;
   }
   m_surface->setTexture(m_textureNoise, 0, "textureSampler");
   m_surface->setTexture(m_textureNoiseNormal, 1, "textureSampler2");
   return true;
}

double Planet::noise(glm::dvec2 pos) {
   pos = glm::mod(pos, (double)m_textureSize);
   glm::dvec2 ff = glm::fract(pos);
   Corner corner = m_coordsToHeight[posToKey(glm::uvec2{ pos })];
   return glm::mix(
      glm::mix(corner.bottomLeft, corner.bottomRight, ff.x),
      glm::mix(corner.topLeft, corner.topRight, ff.x),
   ff.y);
   /*
   float noise(vec2 pos, vec2 offset){
      //debug += mod(pos.x, 1.);
      float textureSize = float(textureSize(textureSampler2, 0).x);
      pos = pos + mod(offset * float(offsetScale), textureSize);
      vec2 ff = pos + 0.5;
      pos = round(pos - 0.5) + 0.5;
      ff -= pos;
   
      vec4 texel = texture(textureSampler, (pos.xy)/textureSize);
      float result = mix(
        mix(texel.x, texel.y, ff.x),
        mix(texel.z, texel.w, ff.x),
      ff.y);
   
      return result;
   }
   */
}

double Planet::layeredNoise(glm::dvec2 pos, int numLayers) {
   double ff{ 1. };
   double result{ 0. };
   for (size_t ii = 0; ii < numLayers; ii++) {
      result += noise(pos * ff + ff * 0.5) / ff;
      ff *= 64.;
   }
   return result;
   /*
   float layeredNoise(vec2 pos, vec2 offset, int numLayers){
      float ff = 1.;
      float result = 0.;
      for (int ii = 0; ii < numLayers; ii++) {
         result += noise(pos.xy * ff + ff * 0.5, offset * ff) / ff;
         ff *= 64.;
      }
      return result;
   }
   */
}

glm::dvec3 Planet::findGroundIntersectionPoint(
   const glm::dvec3& pos, const glm::dvec3& vel, size_t iterations
) {
   glm::dvec3 groundPos{ pos };
   double jumpSize{ 0.5 };
   double elevation;
   for (size_t kk = 0; kk < iterations; kk++) {
      groundPos += vel * jumpSize;
      elevation = map(groundPos, 3).z;
      jumpSize *= 0.5;
      if (groundPos.z > elevation) {
         jumpSize = glm::abs(jumpSize);
      } else {
         jumpSize = -glm::abs(jumpSize);
      }
   }
   groundPos += vel * jumpSize;
   return groundPos;
}

glm::dvec3 Planet::map(glm::dvec3 pos, int detailLevel) {
   double elevation{ layeredNoise(glm::dvec2{pos.x, pos.y} * 0.0122, detailLevel)};
   elevation = glm::mix(
      glm::sin((elevation - 0.5) * 32.),
      elevation * 16.,
      0.25
   ) * 800.;
   /*
   elevation = mix(
    sin((elevation - 0.5) * 32.),
    elevation * 16.,
    0.5
  ) * 700.;
   */
   elevation += (
      glm::sin(pos.x * 0.0000305) +
      glm::sin(pos.y * 0.0000305)
   ) * 3276.8;
   
   elevation = glm::max(elevation, 0.);

   /*
   float scale = pow(2.,15);
   float elevation = layeredNoise(pos.xy*400./scale, offset.xy*400./scale, detailLevel);
   elevation = sin((elevation - 0.5) * 32.) * 0.01 * scale +
      elevation * 0.1 * scale;
   elevation += (
      sin(pos.x/scale + mod(offset.x * float(offsetScale)/scale, 2.*PI)) +
      sin(pos.y/scale + mod(offset.y * float(offsetScale)/scale, 2.*PI))
   ) * 0.1 * scale;
   if(elevation < 0.){
      elevation = 0.0;
   }

  vec3 result = vec3(pos.xy, elevation);
   */

   glm::dvec3 result{ pos.x, pos.y, elevation };

   return result;
}

glm::dvec3 Planet::mapNormal(glm::dvec3 pos, int detailLevel) {
   double dx{ 0.0000001 };
   double mapPos{ map(pos, detailLevel).z };
   return glm::normalize(glm::dvec3{
      -(map(pos + glm::dvec3{dx,0,0}, detailLevel).z - mapPos) / dx,
      -(map(pos + glm::dvec3{0,dx,0}, detailLevel).z - mapPos) / dx,
      1.
   });
}

uint64_t Planet::posToKey(glm::uvec2 pos) {
   return (uint64_t)pos.x |
      (uint64_t)pos.y << 32;
}
