#pragma once

#include "ShaderProgram.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <string>

class Surface {
   class Texture {
   protected:
   public:
      Texture(unsigned int texture, unsigned int textureUnit, std::string samplerName) {
         m_texture = texture;
         m_textureUnit = textureUnit;
         m_samplerName = samplerName;
      }
      unsigned int m_texture{};
      unsigned int m_textureUnit{};
      std::string m_samplerName{};
   };
protected:
   unsigned int m_VBO{}, m_VAO{}, m_EBO{};
   GLsizei m_numVertices{};
   int m_level{0};
public:
   glm::dvec3 m_pos{ 0 };
   glm::dquat m_ori{ 1,0,0,0 };
   ShaderProgram* m_shaderProgram{ nullptr };
   std::vector<Texture> m_textures{};

   Surface();
   ~Surface();

   bool setProceduralGenerationFunction(std::string proceduralGenerationFunction);
   void render();
   void setRenderParams(
       glm::dmat4 view, glm::dmat4 proj, glm::dvec3 camPos, glm::dquat camOri,
       glm::dvec3 resolutionCenter, double fov, int time
   );
   static void createSurface(
      int numLayers, int resolution, int resFirstLayerAdd, int resIncrease,
      std::vector<glm::dvec3>* verts, std::vector<int>* indices,
      std::vector<double>* radii, std::vector<double>* totRadii,
      std::vector<double>* levels
   );
   static std::string injectCode(std::string code, std::string tagStart, std::string tagEnd, std::string injection);
   void setTexture(unsigned int texture, unsigned int textureUnit, std::string samplerName);
   void setLevel(int level);
   int getLevel();
   template<typename T>
   static std::string vectorVec3ToGeogebra(const std::vector<T>* verts, const std::string name);
   template<typename T>
   static std::string vectorAndIndicesVec3ToGeogebraTriangles(
      const std::vector<T>* verts,
      const std::vector<int>* indices,
      const std::string name
   );
   template<typename T>
   static std::string vectorAndIndicesVec3ToGeogebraSquares(
      const std::vector<T>* verts,
      const std::vector<int>* indices,
      const std::string name
   );
};

