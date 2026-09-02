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




class ParticleEffect {
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
   bool m_hasTexture{ false };
   bool m_particleMotionFunctionIsSet{ false };
public:
   glm::dvec3 m_pos{ 0 };
   glm::dquat m_ori{ 1,0,0,0 };
   ShaderProgram* m_shaderProgram{ nullptr };
   std::vector<Texture> m_textures{};
   double m_animationPace{1.};
   int m_modelLoc{};
   int m_viewLoc{};
   int m_projLoc{};
   int m_camPosLoc{};
   int m_timeLoc{};
   int m_numParticlesLoc{};
   int m_animationPaceLoc{};
   
   ParticleEffect();
   ~ParticleEffect();

   void setUniformInt(std::string uniformName, int integer);
   void setParticleTimeFunction(
      std::string particleMotionFunction,
      std::string particleColorFunction
   );
   void render(
      glm::dmat4 model, glm::dmat4 view, glm::dmat4 projection, glm::vec3 camPos, uint64_t time,
      int numParticles
   );
   //void setRenderParams(
   //   glm::dmat4 view, glm::dmat4 proj, glm::dvec3 camPos, glm::dquat camOri,
   //   double fov, int time
   //);
   static std::string injectCode(std::string code, std::string tagStart, std::string tagEnd, std::string injection);
   void setTexture(std::string texturePath, std::string samplerName);
};

