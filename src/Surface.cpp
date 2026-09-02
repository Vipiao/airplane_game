#include "Surface.h"

#include <vector>
#include <numeric>
#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <sstream>

#include "HelperFunctions.h"

int myPow(int x, unsigned int p) {
   if (p == 0) return 1;
   if (p == 1) return x;

   int tmp = myPow(x, p / 2);
   if (p % 2 == 0) return tmp * tmp;
   else return x * tmp * tmp;
}

Surface::Surface() {

   std::vector<glm::dvec3> vertices{};
   std::vector<int> indices{};
   std::vector<double> radii{};
   std::vector<double> totRadii{};
   std::vector<double> levels{};
   Surface::createSurface(12, 25, 0, 1, &vertices, &indices, &radii, &totRadii, &levels);
   std::cout << indices.size() / 4 << std::endl;
   std::vector<float> vertexData{};
   for (size_t ii = 0; ii < vertices.size(); ii++) {
      vertexData.push_back((float)vertices[ii].x);
      vertexData.push_back((float)vertices[ii].y);
      vertexData.push_back((float)vertices[ii].z);
      vertexData.push_back((float)radii[ii]);
      vertexData.push_back((float)totRadii[ii]);
      vertexData.push_back((float)levels[ii]);
   }

   glGenVertexArrays(1, &m_VAO);
   glGenBuffers(1, &m_VBO);
   glGenBuffers(1, &m_EBO);

   glBindVertexArray(m_VAO);

   glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
   glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexData.size(), &vertexData[0], GL_STATIC_DRAW);

   glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
   glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);

   int stride{ (3 + 1 + 1 + 1) * sizeof(float) };

   // Position.
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
   glEnableVertexAttribArray(0);

   // Radii.
   glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
   glEnableVertexAttribArray(1);

   // Total radii.
   glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)((3 + 1) * sizeof(float)));
   glEnableVertexAttribArray(2);

   // Levels.
   glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)((3 + 1 + 1) * sizeof(float)));
   glEnableVertexAttribArray(3);

   m_numVertices = (GLsizei)(indices.size());

   glBindBuffer(GL_ARRAY_BUFFER, 0);

   glBindVertexArray(0);
}

Surface::~Surface() {
   if (m_shaderProgram != nullptr) {
      delete m_shaderProgram;
   }
   glDeleteVertexArrays(1, &m_VAO);
   glDeleteBuffers(1, &m_VBO);
   glDeleteBuffers(1, &m_EBO);
}

bool Surface::setProceduralGenerationFunction(std::string proceduralGenerationFunction) {
   ShaderProgram* newShaderProgram = new ShaderProgram();

   try {
      newShaderProgram->loadVertexShaderFromPath("vertex_shader.vert");

      if (proceduralGenerationFunction == "") {
         newShaderProgram->loadTessellationEvaluationShaderFromPath("tessellation_evaluation_shader.tes");
         newShaderProgram->loadTessellationControlShaderFromPath("tessellation_control_shader.tcs");
         newShaderProgram->loadFragmentShaderFromPath("fragment_shader.frag");
      } else {
         std::string tessEvalCode{ HelperFunctions::loadTextFileFromPath("tessellation_evaluation_shader.tes") };
         std::string tessContrCode{ HelperFunctions::loadTextFileFromPath("tessellation_control_shader.tcs") };
         std::string fragCode{ HelperFunctions::loadTextFileFromPath("fragment_shader.frag") };
         tessEvalCode = Surface::injectCode(tessEvalCode,
            "// --- Procedural generation map function will be replaced FROM here ---",
            "// --- Procedural generation map function will be replaced TO here ---",
            proceduralGenerationFunction
         );
         tessContrCode = Surface::injectCode(tessContrCode,
            "// --- Procedural generation map function will be replaced FROM here ---",
            "// --- Procedural generation map function will be replaced TO here ---",
            proceduralGenerationFunction
         );
         fragCode = Surface::injectCode(fragCode,
            "// --- Procedural generation map function will be replaced FROM here ---",
            "// --- Procedural generation map function will be replaced TO here ---",
            proceduralGenerationFunction
         );
         newShaderProgram->loadTessellationEvaluationShader(tessEvalCode);
         newShaderProgram->loadTessellationControlShader(tessContrCode);
         newShaderProgram->loadFragmentShader(fragCode);
      }

      newShaderProgram->linkShaders();
   } catch (const std::exception&) {
      delete newShaderProgram;
      std::cout << "ERROR: Skipped refreshing shader due to compilation/linking errors." << std::endl;
      return false;
   }
   if (m_shaderProgram != nullptr) {
      delete m_shaderProgram;
   }
   m_shaderProgram = newShaderProgram;

   return true;
}

void Surface::render() {
   // Textures.
   for (size_t ii = 0; ii < m_textures.size(); ii++) {
      const Texture* texture = &m_textures[ii];
      // Bind texture.
      glActiveTexture(GL_TEXTURE0 + texture->m_textureUnit);
      glBindTexture(GL_TEXTURE_2D, texture->m_texture);
      int textureLoc = glGetUniformLocation(m_shaderProgram->getID(), texture->m_samplerName.c_str());
      glUniform1i(textureLoc, texture->m_textureUnit); //GL_TEXTURE(texture->textureUnit)
   }

   // Level.
   int loc = glGetUniformLocation(m_shaderProgram->getID(), "level");
   glUniform1i(loc, (GLint)m_level);

   //

   m_shaderProgram->use();
   glBindVertexArray(m_VAO);
   //glDrawElements(GL_TRIANGLES, m_numVertices, GL_UNSIGNED_INT, 0);
   //GLint numPatchVertices;
   //glGetIntegerv(GL_PATCH_VERTICES, &numPatchVertices);
   glPatchParameteri(GL_PATCH_VERTICES, 4);
   glDrawElements(GL_PATCHES, m_numVertices, GL_UNSIGNED_INT, 0);
}

void Surface::setRenderParams(
    glm::dmat4 view, glm::dmat4 proj, glm::dvec3 camPos, glm::dquat camOri,
    glm::dvec3 resolutionCenter, double fov, int time
) {

   // Model.
   glm::dmat4 modelMatrix{ glm::toMat4(m_ori) };
   modelMatrix = glm::translate(glm::dmat4{1}, m_pos) * modelMatrix;

   m_shaderProgram->use();

   // Offset to deal with floating point errors.
   uint64_t offsetScale{ 2048 };
   glm::dvec3 camPosOffset = glm::round(camPos / (double)offsetScale);
   camPosOffset.z = 0.;
   camPos = camPos - camPosOffset * (double)offsetScale;
   resolutionCenter = resolutionCenter - camPosOffset * (double)offsetScale;
   //if(time % 60 == 0) std::cout << camPos.y << std::endl;
   view = glm::translate(view, camPosOffset * (double)offsetScale);

   int loc = glGetUniformLocation(m_shaderProgram->getID(), "camPosOffset");
   glUniform3f(loc, (float)camPosOffset.x, (float)camPosOffset.y, (float)camPosOffset.z);
   //glUniform3d(loc, camPos.x, camPos.y, camPos.z);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "model");
   glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr((glm::mat4)modelMatrix));

   loc = glGetUniformLocation(m_shaderProgram->getID(), "view");
   glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr((glm::mat4)view));
   //glUniformMatrix4dv(loc, 1, GL_FALSE, glm::value_ptr(view));

   loc = glGetUniformLocation(m_shaderProgram->getID(), "proj");
   glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr((glm::mat4)proj));

   loc = glGetUniformLocation(m_shaderProgram->getID(), "camPos");
   glUniform3f(loc, (float)camPos.x, (float)camPos.y, (float)camPos.z);
   //glUniform3d(loc, camPos.x, camPos.y, camPos.z);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "resolutionCenter");
   glUniform3f(loc, (float)resolutionCenter.x, (float)resolutionCenter.y, (float)resolutionCenter.z);
   //glUniform3d(loc, camPos.x, camPos.y, camPos.z);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "camForward");
   glm::dvec3 camForward = camOri * glm::dvec3{ 0,1,0 };
   glUniform3f(loc, (float)camForward.x, (float)camForward.y, (float)camForward.z);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "camRight");
   glm::dvec3 camRight = camOri * glm::dvec3{ 1,0,0 };
   glUniform3f(loc, (float)camRight.x, (float)camRight.y, (float)camRight.z);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "fov");
   glUniform1f(loc, (float)fov);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "time");
   //glUniform1f(loc, (float)time);
   glUniform1ui(loc, (uint64_t)time);

   loc = glGetUniformLocation(m_shaderProgram->getID(), "offsetScale");
   //glUniform1f(loc, (float)time);
   glUniform1ui(loc, (GLuint)offsetScale);
}

void Surface::createSurface(
   int numLayers, int resolution, int resFirstLayerAdd, int resIncrease,
   std::vector<glm::dvec3>* verts, std::vector<int>* indices,
   std::vector<double>* radii, std::vector<double>* totRadii,
   std::vector<double>* levels
) {
   std::string test{};
   verts->clear();
   indices->clear();

   std::vector<glm::i64vec2> iverts{};
   //std::vector<double>* isEdge{ new std::vector<double>() };

   std::unordered_map<uint64_t, int> coordsToIndices{};
   auto toKey = [](glm::ivec2 pos) -> uint64_t {
      return (uint64_t)(pos.x < 0 ? 1 : 0) | (uint64_t)abs(pos.x) << 1 |
         (uint64_t)(pos.y < 0 ? 1 : 0) << 32 | (uint64_t)abs(pos.y) << 33;
   };

   int64_t rad{ 1 };
   int64_t radTotal{ 0 };
   int64_t radTotalPrev{ -1 };

   for (size_t ii = 0; ii < numLayers; ii++) {
      int ivertInitSize{ (int)iverts.size() };
      // Vertices.
      radTotal += rad * (resolution + (ii>0?0:resFirstLayerAdd));
      if (radTotal / rad % 2 != 0) {
         radTotal += rad;
      }
      glm::i64vec2 pos{-radTotal, -radTotal};
      while (pos.y <= radTotal) {
         pos.x = -radTotal;
         while (pos.x <= radTotal) {
            if (
               pos.x >= -radTotalPrev && pos.x <= radTotalPrev &&
               pos.y >= -radTotalPrev && pos.y <= radTotalPrev
            ) {
               pos.x += rad;
               continue;
            }
            coordsToIndices[toKey(pos)] = (int)iverts.size();
            iverts.push_back(pos);
            radii->push_back(0.);
            totRadii->push_back(0.);
            levels->push_back(0.);
            //test = Surface::vectorAndIndicesVec3ToGeogebraSquares(&iverts, indices, "T");
            pos.x += rad;
         }
         pos.y += rad;
      }
      
      // Indices.
      for (size_t jj = ivertInitSize; jj < iverts.size(); jj++) {
         glm::i64vec2 coord{ iverts[jj] };

         // Up right square.
         // Right.
         glm::ivec2 cR{ coord + glm::i64vec2{rad, 0} };
         auto vR{ coordsToIndices.find(toKey(cR)) };
         //bool hR{ vR != coordsToIndices.end() };
         // Up.
         glm::ivec2 cU{ coord + glm::i64vec2{0, rad} };
         auto vU{ coordsToIndices.find(toKey(cU)) };
         //bool hU{ vU != coordsToIndices.end() };
         // Up right.
         glm::ivec2 cUR{ coord + glm::i64vec2{rad, rad} };
         auto vUR{ coordsToIndices.find(toKey(cUR)) };
         bool hUR{ vUR != coordsToIndices.end() };

         // Down left square.
         // Left.
         glm::ivec2 cL{ coord + glm::i64vec2{-rad, 0} };
         auto vL{ coordsToIndices.find(toKey(cL)) };
         //bool hL{ vL != coordsToIndices.end() };
         // Down.
         glm::ivec2 cD{ coord + glm::i64vec2{0, -rad} };
         auto vD{ coordsToIndices.find(toKey(cD)) };
         //bool hD{ vD != coordsToIndices.end() };
         // Down left.
         glm::ivec2 cDL{ coord + glm::i64vec2{-rad, -rad} };
         auto vDL{ coordsToIndices.find(toKey(cDL)) };
         bool hDL{ vDL != coordsToIndices.end() };
         // Right half up.
         glm::ivec2 cRu{ coord + glm::i64vec2{rad, rad / 2} };
         auto vRu{ coordsToIndices.find(toKey(cRu)) };
         bool hRu{ vRu != coordsToIndices.end() };
         // Up half right.
         glm::ivec2 cUr{ coord + glm::i64vec2{rad / 2, rad} };
         auto vUr{ coordsToIndices.find(toKey(cUr)) };
         bool hUr{ vUr != coordsToIndices.end() };
         // Down half left.
         glm::ivec2 cDl{ coord + glm::i64vec2{-rad / 2, -rad} };
         auto vDl{ coordsToIndices.find(toKey(cDl)) };
         bool hDl{ vDl != coordsToIndices.end() };
         // Left half down.
         glm::ivec2 cLd{ coord + glm::i64vec2{-rad, -rad / 2} };
         auto vLd{ coordsToIndices.find(toKey(cLd)) };
         bool hLd{ vLd != coordsToIndices.end() };
         
         // Check for border squares.
         bool skipUpRightSquare{ false };
         bool skipDownLeftSquare{false};
         if (rad > 1) {
            //
            if (hRu) {
               indices->push_back((int)jj);
               indices->push_back((int)vR->second);
               indices->push_back((int)vRu->second);
               indices->push_back((int)jj); // Duplicate is intentional.

               indices->push_back((int)jj);
               indices->push_back((int)vRu->second);
               indices->push_back((int)vUR->second);
               indices->push_back((int)vU->second);

               (*radii)[jj] = (double)rad;
               (*totRadii)[jj] = (double)radTotal;
               (*levels)[jj] = (double)ii;

               skipUpRightSquare = true;
            } else if(hUr) {
               indices->push_back((int)jj);
               indices->push_back((int)vR->second);
               indices->push_back((int)vUR->second);
               indices->push_back((int)vUr->second);

               indices->push_back((int)jj);
               indices->push_back((int)vUr->second);
               indices->push_back((int)vU->second);
               indices->push_back((int)jj); // Duplicate is intentional.

               (*radii)[jj] = (double)rad;
               (*totRadii)[jj] = (double)radTotal;
               (*levels)[jj] = (double)ii;

               skipUpRightSquare = true;
            } else if (hDl) {
               indices->push_back((int)vDl->second);
               indices->push_back((int)vD->second);
               indices->push_back((int)jj);
               indices->push_back((int)vL->second);
               
               indices->push_back((int)vDL->second);
               indices->push_back((int)vDl->second);
               indices->push_back((int)vL->second);
               indices->push_back((int)vDL->second); // Duplicate is intentional.

               (*radii)[(int)vDl->second] = (double)rad;
               (*totRadii)[(int)vDl->second] = (double)radTotal;
               (*levels)[(int)vDl->second] = (double)ii;

               (*radii)[(int)vDL->second] = (double)rad;
               (*totRadii)[(int)vDL->second] = (double)radTotal;
               (*levels)[(int)vDL->second] = (double)ii;

               skipDownLeftSquare = true;
            } else if (hLd) {
               indices->push_back((int)vDL->second);
               indices->push_back((int)vD->second);
               indices->push_back((int)jj);
               indices->push_back((int)vLd->second);

               indices->push_back((int)vLd->second);
               indices->push_back((int)jj);
               indices->push_back((int)vL->second);
               indices->push_back((int)vLd->second); // Duplicate is intentional.

               (*radii)[(int)vDL->second] = (double)rad;
               (*totRadii)[(int)vDL->second] = (double)radTotal;
               (*levels)[(int)vDL->second] = (double)ii;

               (*radii)[(int)vLd->second] = (double)rad;
               (*totRadii)[(int)vLd->second] = (double)radTotal;
               (*levels)[(int)vLd->second] = (double)ii;

               skipDownLeftSquare = true;
            }
         }

         if (!skipUpRightSquare && hUR) {
            indices->push_back((int)jj);
            indices->push_back((int)vR->second);
            indices->push_back((int)vUR->second);
            indices->push_back((int)vU->second);
            (*radii)[jj] = (double)rad;
            (*totRadii)[jj] = (double)radTotal;
            (*levels)[jj] = (double)ii;
         }
         if (!skipDownLeftSquare && hDL && (
            coord.x - rad >= -radTotalPrev && coord.x - rad <= radTotalPrev &&
            coord.y - rad >= -radTotalPrev && coord.y - rad <= radTotalPrev
            )) {
            indices->push_back((int)vDL->second);
            indices->push_back((int)vD->second);
            indices->push_back((int)jj);
            indices->push_back((int)vL->second);
            (*radii)[(int)vDL->second] = (double)rad;
            (*totRadii)[(int)vDL->second] = (double)radTotal;
            (*levels)[(int)vDL->second] = (double)ii;
         }
         //test = Surface::vectorAndIndicesVec3ToGeogebraSquares(&iverts, indices, "T");
      }
      //test = Surface::vectorAndIndicesVec3ToGeogebraSquares(&iverts, indices, "T");

      //
      rad *= 2;
      radTotalPrev = radTotal;
      resolution += resIncrease;
   }

   // Convert to doubles.
   //test = Surface::vectorAndIndicesVec3ToGeogebraSquares(&iverts, indices, "T");
   //double radius{ (double)radTotal };
   double radius{ glm::pow(2., glm::round(glm::log2((double)radTotal))) };
   for (size_t ii = 0; ii < iverts.size(); ii++) {
      verts->push_back({ iverts[ii].x / radius, iverts[ii].y / radius, 0});
      (*radii)[ii] /= radius;
      (*totRadii)[ii] /= radius;
   }
}

std::string Surface::injectCode(std::string code, std::string tagStart, std::string tagEnd, std::string injection) {
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

void Surface::setTexture(unsigned int texture, unsigned int textureUnit, std::string samplerName) {
   m_textures.emplace_back(texture, textureUnit, samplerName);
}

void Surface::setLevel(int level) {
   m_level = level;
}

int Surface::getLevel() {
   return m_level;
}

template<typename T>
std::string Surface::vectorAndIndicesVec3ToGeogebraSquares(
   const std::vector<T>* verts,
   const std::vector<int>* indices,
   const std::string name
) {
   std::vector<std::string> result{};
   std::string vecRes{ Surface::vectorVec3ToGeogebra(verts, name) };
   result.push_back(vecRes.substr(0, vecRes.size() - 2)); // Removes "})".
   for (size_t ii = 3; ii < indices->size(); ii += 4) {
      if (ii != 0) {
         result.push_back(",");
      }
      // Polygon(Test_{0},Test_{4},Test_{5})
      result.push_back("\"P_ " + name + "_{" + std::to_string(ii) + "}=Polygon(" +
         name + "_" + "{" + std::to_string((*indices)[ii - 3]) + "}" + "," +
         name + "_" + "{" + std::to_string((*indices)[ii - 2]) + "}" + "," +
         name + "_" + "{" + std::to_string((*indices)[ii - 1]) + "}" + "," +
         name + "_" + "{" + std::to_string((*indices)[ii - 0]) + "}" + ")\""
      );
   }
   result.push_back("})");
   return std::accumulate(result.begin(), result.end(), std::string{});
}
template std::string Surface::vectorAndIndicesVec3ToGeogebraSquares(
   const std::vector<glm::dvec3>* verts,
   const std::vector<int>* indices,
   const std::string name);
template std::string Surface::vectorAndIndicesVec3ToGeogebraSquares(
   const std::vector<glm::ivec3>* verts,
   const std::vector<int>* indices,
   const std::string name);

template<typename T>
std::string Surface::vectorAndIndicesVec3ToGeogebraTriangles(
   const std::vector<T>* verts,
   const std::vector<int>* indices,
   const std::string name
) {
   std::vector<std::string> result{};
   std::string vecRes{ Surface::vectorVec3ToGeogebra(verts, name) };
   result.push_back(vecRes.substr(0, vecRes.size() - 2)); // Removes "})".
   for (size_t ii = 2; ii < indices->size(); ii+=3) {
      if (ii != 0) {
         result.push_back(",");
      }
      // Polygon(Test_{0},Test_{4},Test_{5})
      result.push_back("\"P_ " + name + "_{" + std::to_string(ii) + "}=Polygon(" +
         name + "_" + "{" + std::to_string((*indices)[ii - 2]) + "}" + "," +
         name + "_" + "{" + std::to_string((*indices)[ii-1]) + "}" + "," +
         name + "_" + "{" + std::to_string((*indices)[ii-0]) + "}" + ")\""
      );
   }
   result.push_back("})");
   return std::accumulate(result.begin(), result.end(), std::string{});
}
template std::string Surface::vectorAndIndicesVec3ToGeogebraTriangles(
   const std::vector<glm::dvec3>* verts,
   const std::vector<int>* indices,
   const std::string name);
template std::string Surface::vectorAndIndicesVec3ToGeogebraTriangles(
   const std::vector<glm::ivec3>* verts,
   const std::vector<int>* indices,
   const std::string name);

template<typename T>
std::string Surface::vectorVec3ToGeogebra(const std::vector<T>* verts, const std::string name) {
   std::vector<std::string> result{};
   result.push_back("Execute({");
   for (size_t ii = 0; ii < verts->size(); ii++) {
      if (ii != 0) {
         result.push_back(",");
      }
      result.push_back("\"" + name + "_{" + std::to_string(ii) + "}=(" +
         std::to_string((*verts)[ii].x) + "," +
         std::to_string((*verts)[ii].y) + ")\""
      );
   }
   result.push_back("})");

   return std::accumulate(result.begin(), result.end(), std::string{});
}
template std::string Surface::vectorVec3ToGeogebra(const std::vector<glm::dvec3>*, const std::string);
template std::string Surface::vectorVec3ToGeogebra(const std::vector<glm::ivec3>*, const std::string);


