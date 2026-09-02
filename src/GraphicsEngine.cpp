#include "GraphicsEngine.h"

#include "Surface.h"
#include "GlobalConstants.h"
#include "AssimpLoader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <chrono>
#include <thread>

void GraphicsEngine::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
   GraphicsEngine* graphicsEngine{ static_cast<GraphicsEngine*>(glfwGetWindowUserPointer(window)) };
   glfwMakeContextCurrent(graphicsEngine->m_window);
   glViewport(0, 0, width, height);
   graphicsEngine->m_screen_width = width;
   graphicsEngine->m_screen_height = height;
   if(graphicsEngine->m_graphicsEngineCallback != nullptr)
      graphicsEngine->m_graphicsEngineCallback->framebufferSizeCallback(width, height);
}

GraphicsEngine::GraphicsEngine() {
   // glfw: initialize and configure
   // ------------------------------
   glfwInit();
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef FLOATING_WINDOW
   glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);
#endif

#ifdef __APPLE__
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

   //
#ifdef FULLSCREEN
   //GLFWmonitor* monitor = glfwGetPrimaryMonitor();
   //const GLFWvidmode* mode = glfwGetVideoMode(monitor);
   //m_screen_width = mode->width;
   //m_screen_height = mode->height;
#endif

   // glfw window creation
   // --------------------
   m_window = glfwCreateWindow(m_screen_width, m_screen_height, "LearnOpenGL", NULL, NULL);
   if (m_window == NULL) {
      glfwTerminate();
      std::cout << "Failed to create GLFW window" << std::endl;
      throw "Failed to create GLFW window";
   }
   glfwSetWindowUserPointer(m_window, this);
   glfwMakeContextCurrent(m_window);
   glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);

   //

   // glad: load all OpenGL function pointers
   // ---------------------------------------
   if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
      glfwTerminate();
      std::cout << "Failed to initialize GLAD" << std::endl;
      throw "Failed to initialize GLAD";
   }

   // Clobal configuration.
#ifdef V_SYNC_OFF
   glfwSwapInterval(0);
#else
   glfwSwapInterval(1);
#endif
   glEnable(GL_DEPTH_TEST);
   glEnable(GL_CULL_FACE);
   //glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

   // Mouse.
   m_mouseHandler = new MouseHandler{m_window};

   //
#ifdef FULLSCREEN
   glfwMaximizeWindow(m_window);
#endif //FULLSCREEN
}

GraphicsEngine::~GraphicsEngine() {
   //std::cout << "~GraphicsEngine()" << std::endl;
   for (size_t ii = 0; ii < m_surfaces.size(); ii++) {
      delete m_surfaces[ii];
   }
   for (size_t ii = 0; ii < m_models.size(); ii++) {
       delete m_models[ii];
   }
   for (size_t ii = 0; ii < m_meshes.size(); ii++) {
       delete m_meshes[ii];
   }
   for (size_t ii = 0; ii < m_transparentMeshes.size(); ii++) {
      delete m_transparentMeshes[ii];
   }
   for (size_t ii = 0; ii < m_particleEffects.size(); ii++) {
      delete m_particleEffects[ii];
   }
   for (size_t ii = 0; ii < m_particleEffectInstances.size(); ii++) {
      delete m_particleEffectInstances[ii];
   }
   glfwTerminate();

   delete m_mouseHandler;
}

void GraphicsEngine::setSwapInterval(int swapInterval) {
   glfwSwapInterval(swapInterval);
}

void GraphicsEngine::setCallbackObject(GraphicsEngineCallback* graphicsEngineCallback) {
   m_graphicsEngineCallback = graphicsEngineCallback;
}

glm::dmat4 GraphicsEngine::createModel(glm::dvec3 scale, glm::dquat orientation, glm::dvec3 position) {
   glm::dmat4 model = glm::scale(glm::dmat4{ 1. }, scale);
   model = glm::toMat4(orientation) * model;
   model = glm::translate(glm::dmat4{ 1. }, position) * model;
   return model;
}

void GraphicsEngine::renderLoop() {
   const std::chrono::duration<double> targetFrameDuration(1.0 / 120.0); // Target 60 FPS
   auto nextFrameTime = std::chrono::high_resolution_clock::now();

   while (!glfwWindowShouldClose(m_window)) {
      nextFrameTime += std::chrono::duration_cast<std::chrono::high_resolution_clock::duration>(targetFrameDuration);
      if (m_frameNum == 500) {
         //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
      }

      //
      //handleMouse(m_frameNum);
      m_mouseHandler->update();

      // Clear.
      // calculate a value that oscillates between 0 and 1 over time
      float timeValue = sin(m_frameNum * 0.01f) * 0.5f + 0.5f;
      timeValue = 1.;

      // interpolate between two colors based on timeValue
      glm::vec3 dayColor{ 0.6f, 0.7f, 0.8f };
      glm::vec3 nightColor{ 0.2f, 0.2f, 0.3f };
      glm::vec3 backgroundColor = nightColor * (1.0f - timeValue) + dayColor * timeValue;

      glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


      //
      if(m_graphicsEngineCallback != nullptr) m_graphicsEngineCallback->preRenderCallback(m_frameNum);

      if (m_frameNum == 0) {
         m_camVel = {0,0,0};
      } else {
         m_camVel = m_camPos - m_camPosPrev;
      }
      m_camPosPrev = m_camPos;

      glm::dmat4 viewMatrix{ 1 };
      double ss{ glm::sqrt(2.) / 2. };
      viewMatrix = glm::toMat4(glm::dquat{ ss, -ss, 0., 0. } *glm::conjugate(m_camOri));
      viewMatrix = glm::translate(viewMatrix, -m_camPos);

      glm::dmat4 projectionMatrix;
      double aspectRatio{};
      if (m_screen_width == 0) aspectRatio = 1.;
      else if (m_screen_height == 0) aspectRatio = 1.;
      else aspectRatio = (double)m_screen_width / (double)m_screen_height;
      //if (m_frameNum % 60 == 0) std::cout << aspectRatio << std::endl;
      projectionMatrix =
         glm::perspective(m_fieldOfView/aspectRatio, aspectRatio, 1., 1000000.);

      glm::dvec3 resolutionCenter{ glm::round(m_camPos*1000.)/1000. };

      // Render surface.
      glEnable(GL_CULL_FACE);
      for (size_t ii = 0; ii < m_surfaces.size(); ii++) {
         m_surfaces[ii]->setRenderParams(
             viewMatrix, projectionMatrix, m_camPos, m_camOri,
             m_camPos, m_fieldOfView, (int)m_frameNum
         );
         m_surfaces[ii]->render();
      }

      // Render meshes.
      glm::dmat4 model{ 1 };
      for (size_t ii = 0; ii < m_meshes.size(); ii++) {
         Mesh* mesh{ m_meshes[ii] };
         if (!mesh->m_isVisible) {
            continue;
         }
         model = glm::scale(glm::dmat4{ 1. }, mesh->m_scale);
         model = glm::toMat4(mesh->m_orientation) * model;
         model = glm::translate(glm::dmat4{ 1. }, mesh->m_position) * model;
         if (mesh->m_doCulling) {
            glEnable(GL_CULL_FACE);
         } else {
            glDisable(GL_CULL_FACE);
         }
         Model* mm{ mesh->m_model };
         mm->m_useTextures = mesh->m_useTexture;
         mm->m_color = mesh->m_color;
         mm->render(model, viewMatrix, projectionMatrix, m_camPos, m_frameNum);
      }
      glEnable(GL_CULL_FACE);

      // Render transparent meshes.
      glDepthMask(false);
      for (size_t ii = 0; ii < m_transparentMeshes.size(); ii++) {
         Mesh* mesh{ m_transparentMeshes[ii] };
         if (!mesh->m_isVisible) {
            continue;
         }
         model = glm::scale(glm::dmat4{ 1. }, mesh->m_scale);
         model = glm::toMat4(mesh->m_orientation) * model;
         model = glm::translate(glm::dmat4{ 1. }, mesh->m_position) * model;
         if (mesh->m_doCulling) {
            glEnable(GL_CULL_FACE);
         } else {
            glDisable(GL_CULL_FACE);
         }
         Model* mm{ mesh->m_model };
         mm->m_useTextures = mesh->m_useTexture;
         mm->m_color = mesh->m_color;
         mm->render(model, viewMatrix, projectionMatrix, m_camPos, m_frameNum);
      }
      glDepthMask(true);
      glEnable(GL_CULL_FACE);

      // Render particle effects.
      glDepthMask(false);
      for (size_t ii = 0; ii < m_particleEffectInstances.size(); ii++) {
         ParticleEffectInstance* pei(m_particleEffectInstances[ii]);
         if (!pei->m_isVisible) {
            continue;
         }
         model = glm::scale(glm::dmat4{ 1. }, pei->m_scale);
         model = glm::toMat4(pei->m_orientation) * model;
         model = glm::translate(glm::dmat4{ 1. }, pei->m_position) * model;
         ParticleEffect* particleEffect{ pei->m_particleEffect };
         particleEffect->m_animationPace = pei->m_animationPace;
         particleEffect->setUniformInt("startTime", (int)pei->m_spawnTime);
         particleEffect->render(
            model, viewMatrix, projectionMatrix, m_camPos, m_frameNum, pei->m_numParticles
         );
      }
      glDepthMask(true);

      // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
      // -------------------------------------------------------------------------------
      glfwSwapBuffers(m_window);
      glfwPollEvents();
      
      m_frameNum++;

      // Wait until it's time for the next frame
      auto now = std::chrono::high_resolution_clock::now();
      if (now < nextFrameTime) {
          std::this_thread::sleep_for(nextFrameTime - now);
      }
   }
}

void GraphicsEngine::setTriangleRenderMode(bool useTriangles) {
   m_renderTriangleMode = useTriangles;
   if (m_renderTriangleMode) {
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   }else{
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
   }
}

bool GraphicsEngine::getTriangleRenderMode() {
   return m_renderTriangleMode;
}

Surface* GraphicsEngine::createSurface() {
   Surface* newSurface = new Surface();
   m_surfaces.push_back(newSurface);
   return newSurface;
}

void GraphicsEngine::removeSurface(Surface* surface) {
   for (size_t ii = 0; ii < m_surfaces.size(); ii++) {
      if (surface == m_surfaces[ii]) {
         delete surface;
         if (ii != m_surfaces.size() - 1) {
            m_surfaces[ii] = m_surfaces[m_surfaces.size() - 1];
         }
         m_surfaces.pop_back();
      }
   }
}

Model* GraphicsEngine::createModel(const std::vector<double>* vertexData, const std::vector<int>* indices, std::string texturePath) {
    Model* model{ new Model() };
    model->loadGeometry(vertexData, indices, texturePath);
    m_models.push_back(model);

    return model;
}

Model* GraphicsEngine::createModel(std::string filePath, std::string texturePath) {
   std::vector<std::vector<double>> vertexData{};
   std::vector<std::vector<int>> indices{};
   bool hasTextures{};
   bool ignoreTextureCoordinates{ false };
   AssimpLoader::load(filePath, &vertexData, &indices, &hasTextures, ignoreTextureCoordinates);
   if (hasTextures && texturePath == "") {
      std::cout << "ERROR: Texture coordinates found in file at \"" + filePath +
         "\" but empty (\"\") texturePath provided" << std::endl;
      throw "ERROR: Texture coordinates found in file at \"" + filePath +
         "\" but empty (\"\") texturePath provided";
   }
   if (!hasTextures && texturePath != "") {
      std::cout << "ERROR: Texture coordinates not found in file at \"" + filePath +
         "\" but texturePath provided" << std::endl;
      throw "ERROR: Texture coordinates not found in file at \"" + filePath +
         "\" but texturePath provided";
   }
   Model* model{ createModel(&vertexData[0], &indices[0], texturePath) };
   return model;
}

Mesh* GraphicsEngine::createMesh(Model* model) {
    Mesh* mesh{ new Mesh() };
    m_meshes.push_back(mesh);
    mesh->m_model = model;

    return mesh;
}

void GraphicsEngine::removeMesh(Mesh* mesh) {
    for (size_t ii = 0; ii < m_meshes.size(); ii++) {
        if (m_meshes[ii] == mesh) {
            if (ii == m_meshes.size()-1) {
                m_meshes.pop_back();
            } else {
                m_meshes[ii] = m_meshes[m_meshes.size() - 1];
                m_meshes.pop_back();
            }
            delete mesh;
            return;
        }
    }
    std::cout << "ERROR: Tried removing mesh that does not exist." << std::endl;
    throw "ERROR: Tried removing mesh that does not exist.";
}

Mesh* GraphicsEngine::createTransparentMesh(Model* model) {
   Mesh* mesh{ new Mesh() };
   m_transparentMeshes.push_back(mesh);
   mesh->m_model = model;

   return mesh;
}

void GraphicsEngine::removeTransparentMesh(Mesh* mesh) {
   for (size_t ii = 0; ii < m_transparentMeshes.size(); ii++) {
      if (m_transparentMeshes[ii] == mesh) {
         if (ii == m_transparentMeshes.size() - 1) {
            m_transparentMeshes.pop_back();
         } else {
            m_transparentMeshes[ii] = m_transparentMeshes[m_transparentMeshes.size() - 1];
            m_transparentMeshes.pop_back();
         }
         delete mesh;
         return;
      }
   }
   std::cout << "ERROR: Tried removing mesh that does not exist." << std::endl;
   throw "ERROR: Tried removing mesh that does not exist.";
}

ParticleEffect* GraphicsEngine::createParticleEffect() {
   ParticleEffect* particleEffect{ new ParticleEffect() };
   m_particleEffects.push_back(particleEffect);

   return particleEffect;
}

ParticleEffectInstance* GraphicsEngine::createParticleEffectInstance(
   ParticleEffect* particleEffect
) {
   ParticleEffectInstance* particleEffectInstance{ new ParticleEffectInstance(m_frameNum) };
   m_particleEffectInstances.push_back(particleEffectInstance);
   particleEffectInstance->m_particleEffect = particleEffect;

   return particleEffectInstance;
}

void GraphicsEngine::removeParticleEffectInstance(ParticleEffectInstance* particleEffectInstance) {
   for (size_t ii = 0; ii < m_particleEffectInstances.size(); ii++) {
      if (m_particleEffectInstances[ii] == particleEffectInstance) {
         if (ii == m_particleEffectInstances.size() - 1) {
            m_particleEffectInstances.pop_back();
         } else {
            m_particleEffectInstances[ii] = m_particleEffectInstances[m_particleEffectInstances.size() - 1];
            m_particleEffectInstances.pop_back();
         }
         delete particleEffectInstance;
         return;
      }
   }
   std::cout << "ERROR: Tried removing mesh that does not exist." << std::endl;
   throw "ERROR: Tried removing mesh that does not exist.";
}
