#pragma once


#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Surface.h"
#include "Planet.h"
#include "Mesh.h"
#include "ParticleEffect.h"
#include "ParticleEffectInstance.h"
#include "MouseHandler.h"

class GraphicsEngineCallback {
public:
   virtual void preRenderCallback(uint64_t frameNum) = 0;
   virtual void framebufferSizeCallback(int width, int height) = 0;
};

class GraphicsEngine {
protected:
   static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

   //void (*m_framebufferSizeCallbackFunction)(GraphicsEngine*, int width, int height) { nullptr };
   //void (*m_preRenderCallback)(GraphicsEngine*) { nullptr };
   GraphicsEngineCallback* m_graphicsEngineCallback{ nullptr };
   std::vector<Surface*> m_surfaces;
   bool m_renderTriangleMode{ false };
   bool m_windowOnTop{ false };
   std::vector<Mesh*> m_meshes{};
   std::vector<Mesh*> m_transparentMeshes{};
   std::vector<Model*> m_models{};
   std::vector<ParticleEffect*> m_particleEffects{};
   std::vector<ParticleEffectInstance*> m_particleEffectInstances{};

   glm::dvec3 m_camPosPrev{};
public:
   GraphicsEngine();
   ~GraphicsEngine();
   GraphicsEngine(const GraphicsEngine&) = delete;
   GraphicsEngine& operator= (const GraphicsEngine&) = delete;

   void setSwapInterval(int swapInterval);
   void setCallbackObject(GraphicsEngineCallback* graphicsEngineCallback);
   glm::dmat4 createModel(glm::dvec3 scale, glm::dquat orientation, glm::dvec3 position);
   //void handleMouse(uint64_t frameNum);
   void renderLoop();
   void setTriangleRenderMode(bool useTriangles);
   bool getTriangleRenderMode();
   Surface* createSurface();
   void removeSurface(Surface* surface);
   Model* createModel(
      const std::vector<double>* vertexData,
      const std::vector<int>* indices,
      std::string texturePath
   );
   Model* createModel(
      std::string filePath,
      std::string texturePath
   );
   Mesh* createMesh(Model* model);
   void removeMesh(Mesh* mesh);
   Mesh* createTransparentMesh(Model* model);
   void removeTransparentMesh(Mesh* mesh);
   ParticleEffect* createParticleEffect();
   ParticleEffectInstance* createParticleEffectInstance(
      ParticleEffect* particleEffect
   );
   void removeParticleEffectInstance(
      ParticleEffectInstance* particleEffectInstance
   );

   GLFWwindow* m_window{};
   unsigned int m_screen_width{ 800 };
   unsigned int m_screen_height{ 600 };
   glm::dvec3 m_camPos{0,0,1};
   glm::dvec3 m_camVel{};
   glm::dquat m_camOri{ glm::sqrt(2.) / 2., -glm::sqrt(2.) / 2.,0,0 };
   uint64_t m_frameNum{ 0 };
   double m_fieldOfView{ glm::radians(100.0) };
   // Mouse.
   MouseHandler* m_mouseHandler{ nullptr };
};

