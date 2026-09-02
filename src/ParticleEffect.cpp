#include "ParticleEffect.h"

#include <iostream>
#include <sstream>

#include "STBImageLoader.h"
#include "HelperFunctions.h"
#include "Hash.h"



ParticleEffect::ParticleEffect() {
   // Geometry.
   std::vector<float> vertexData{
      -1.f,-1.f, 0.f, 0.f, 0.f,
       1.f,-1.f, 0.f, 1.f, 0.f,
       1.f, 1.f, 0.f, 1.f, 1.f,
      -1.f, 1.f, 0.f, 0.f, 1.f
   };
   int vertexDataInitialSize{ (int)vertexData.size() };
   std::vector<float> customRandom{
      0.5f,0.5f,0.5f,                 // Center.
      0.0f,0.5f,0.5f, 1.0f,0.5f,0.5f, // Left, right.
      0.5f,0.0f,0.5f, 0.5f,1.0f,0.5f, // Back, forward.
      0.5f,0.5f,0.0f, 0.5f,0.5f,1.0f, // Down, up.
   };
   for (size_t ii = 0; ii < customRandom.size(); ii++) {
      float nextValue{ customRandom[ii] };
      if (ii >= 3) {
         float scale{ 0.2f };
         if (nextValue == 0.) {
            nextValue += (float)Hash::pcgUnit((uint64_t)ii) * scale;
         } else if (nextValue == 1.) {
            nextValue -= (float)Hash::pcgUnit((uint64_t)ii) * scale;
         } else {
            nextValue += (float)(Hash::pcgUnit((uint64_t)ii) - 0.5) * 2.f * scale;
         }
      }
      vertexData.push_back(nextValue);
   }
   for (size_t ii = 8; ii < 100*3; ii++) {
      vertexData.push_back((float)Hash::pcgUnit((uint64_t)ii));
   }
   std::vector<int> indices{0, 1, 2, 0, 2, 3};
   glGenVertexArrays(1, &m_VAO);
   glGenBuffers(1, &m_VBO);
   glGenBuffers(1, &m_EBO);

   glBindVertexArray(m_VAO);

   glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
   
   int stride{ (3+2) * sizeof(float) };

   // Position.
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
   glEnableVertexAttribArray(0);
   glVertexAttribDivisor(0, 0);

   // Texture.
   glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);
   glVertexAttribDivisor(1, 0);

   // Random.
   glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(vertexDataInitialSize * sizeof(float)));
   glEnableVertexAttribArray(2);
   glVertexAttribDivisor(2, 1);


   m_numVertices = (GLsizei)(indices.size());

   glBindBuffer(GL_ARRAY_BUFFER, 0);

   glBindVertexArray(0);
}

ParticleEffect::~ParticleEffect() {
   if (m_shaderProgram != nullptr) {
      delete m_shaderProgram;
   }
   glDeleteVertexArrays(1, &m_VAO);
   glDeleteBuffers(1, &m_VBO);
   glDeleteBuffers(1, &m_EBO);

   for (size_t ii = 0; ii < m_textures.size(); ii++) {
      glDeleteTextures(1, &m_textures[ii].m_texture);
   }
}

void ParticleEffect::setUniformInt(std::string uniformName, int integer) {
   m_shaderProgram->use();
   int loc = glGetUniformLocation(m_shaderProgram->getID(), uniformName.c_str());
   if (loc == -1) {
      // Notice that if the uniform is not used in the glsl code, the compiler will remove the uniform.
      // This could be the reason the uniform of this name is not found.
      //std::cout << "ERROR: Uniform name :\"" + uniformName + "\" does not exist in the shader." << std::endl;
      //throw "ERROR: Uniform name :\"" + uniformName + "\" does not exist in the shader.";
      return;
   }
   glUniform1i(loc, (unsigned int)integer);
}

void ParticleEffect::setParticleTimeFunction(
   std::string particleMotionFunction,
   std::string particleColorFunction
) {
   ShaderProgram* newShaderProgram = new ShaderProgram();

   try {
      //newShaderProgram->loadVertexShaderFromPath("vertex_shader.vert");

      if (particleMotionFunction == "") {
         newShaderProgram->loadVertexShaderFromPath("particle_vertex_shader.vert");
         newShaderProgram->loadFragmentShaderFromPath("particle_fragment_shader.frag");
      } else {
         std::string vertCode{ HelperFunctions::loadTextFileFromPath("particle_vertex_shader.vert") };
         vertCode = ParticleEffect::injectCode(vertCode,
            "// --- Particle Motion Function function will be replaced FROM here ---",
            "// --- Particle Motion Function function will be replaced TO here ---",
            particleMotionFunction
         );
         std::string fragCode{ HelperFunctions::loadTextFileFromPath("particle_fragment_shader.frag") };
         fragCode = ParticleEffect::injectCode(fragCode,
          "// --- Color chooser Function will be replaced FROM here ---",
          "// --- Color chooser Function function will be replaced TO here ---",
            particleColorFunction
         );
         newShaderProgram->loadVertexShader(vertCode);
         newShaderProgram->loadFragmentShader(fragCode);
      }

      newShaderProgram->linkShaders();
   } catch (const std::exception&) {
      delete newShaderProgram;
      throw;
   }
   if (m_shaderProgram != nullptr) {
      delete m_shaderProgram;
   }
   m_shaderProgram = newShaderProgram;
   m_shaderProgram->use();
   m_particleMotionFunctionIsSet = true;

   m_modelLoc = glGetUniformLocation(m_shaderProgram->getID(), "model");
   m_viewLoc = glGetUniformLocation(m_shaderProgram->getID(), "view");
   m_projLoc = glGetUniformLocation(m_shaderProgram->getID(), "projection");
   m_camPosLoc = glGetUniformLocation(m_shaderProgram->getID(), "camPos");
   m_timeLoc = glGetUniformLocation(m_shaderProgram->getID(), "time");
   m_numParticlesLoc = glGetUniformLocation(m_shaderProgram->getID(), "numParticles");
   m_animationPaceLoc = glGetUniformLocation(m_shaderProgram->getID(), "animationPace");
}

void ParticleEffect::render(
   glm::dmat4 model, glm::dmat4 view, glm::dmat4 projection, glm::vec3 camPos, uint64_t time,
   int numParticles
) {
   if (!m_particleMotionFunctionIsSet) {
      std::cout << "Error: setParticleMotionFunction(...) must be called before render()." << std::endl;
      throw "Error: setParticleMotionFunction(...) must be called before render().";
   }
   // Update uniforms.
   m_shaderProgram->use();
   glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)model));
   glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)view));
   glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)projection));
   glUniform3f(m_camPosLoc, camPos.x, camPos.y, camPos.z);
   glUniform1i(m_timeLoc, (unsigned int)time);
   glUniform1i(m_numParticlesLoc, (unsigned int)numParticles);
   glUniform1f(m_animationPaceLoc, (float)m_animationPace);

   // Textures.
   for (size_t ii = 0; ii < m_textures.size(); ii++) {
      const Texture* texture = &m_textures[ii];
      // Bind texture.
      glActiveTexture(GL_TEXTURE0 + texture->m_textureUnit);
      glBindTexture(GL_TEXTURE_2D, texture->m_texture);
      int textureLoc = glGetUniformLocation(m_shaderProgram->getID(), texture->m_samplerName.c_str());
      glUniform1i(textureLoc, texture->m_textureUnit); //GL_TEXTURE(texture->textureUnit)
   }

   glBindVertexArray(m_VAO);
   glDrawElementsInstanced(GL_TRIANGLES, m_numVertices, GL_UNSIGNED_INT, 0, numParticles);
   //glDrawArraysInstanced(GL_TRIANGLES, 0, m_numVertices,  1);
   //glDrawElements(GL_TRIANGLES, m_numVertices, GL_UNSIGNED_INT, 0);

}

std::string ParticleEffect::injectCode(std::string code, std::string tagStart, std::string tagEnd, std::string injection) {
   // TODO, Surface, and Particle effect has copy of this code!
   // Abstract function into string helper class.
   std::istringstream injectionStream(code);
   std::stringstream injectionStreamResult{};
   bool doSkip{ false };
   for (std::string line; std::getline(injectionStream, line); ) {
      //std::cout << line << std::endl;
      if (line == tagStart) {
         doSkip = true;
      }
      if (doSkip) {
         if (line == tagEnd) {
            doSkip = false;
            injectionStreamResult << injection << std::endl;
         }
      } else {
         injectionStreamResult << line << std::endl;
      }
   }
   return injectionStreamResult.str();
}

void ParticleEffect::setTexture(std::string texturePath, std::string samplerName) {
   unsigned int texture;
   glGenTextures(1, &texture);
   glBindTexture(GL_TEXTURE_2D, texture);
   // set the texture wrapping parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to...
   // ...GL_REPEAT (default wrapping method)
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   // set texture filtering parameters
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   // load image, create texture and generate mipmaps
   
   int width, height, nrChannels;

   unsigned char* data = STBImageLoader::load(true, texturePath, &width, &height, &nrChannels);
   if (nrChannels == 3) {
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   } else { // nrChannels == 4
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
   }
   //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
   glGenerateMipmap(GL_TEXTURE_2D);
   STBImageLoader::free(data);

   //
   m_textures.emplace_back(texture, (unsigned int)m_textures.size(), samplerName);
}
