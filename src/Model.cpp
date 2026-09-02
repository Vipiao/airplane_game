


#include "Model.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

//#include "MeshGenerator.h"
#include "AssimpLoader.h"
#include "STBImageLoader.h"


// Render shader program.
ShaderProgram Model::m_shaderProgram{};
int Model::m_modelLoc{ -1 };
int Model::m_viewLoc{ -1 };
int Model::m_projLoc{ -1 };
int Model::m_camPosLoc{ -1 };
int Model::m_textureLoc{ -1 };
int Model::m_renderModeLoc{ -1 };
int Model::m_colorLoc{ -1 };
int Model::m_timeLoc{ -1 };
// Shadows.
int Model::m_shadowMapLoc{ -1 };
int Model::m_lightSpaceMatrixLoc{ -1 };
int Model::m_shadowMapLoc2{ -1 };
int Model::m_lightSpaceMatrixLoc2{ -1 };

// Shadow shader program.
ShaderProgram Model::m_shaderProgramShadows{};
int Model::m_shadowModelLoc{ -1 };
int Model::m_shadowLightSpaceMatrixLoc{ -1 };

unsigned int Model::RENDER_MODE_SELF_ILLUMINATION{ 0x1 };
unsigned int Model::RENDER_MODE_USE_TEXTURE{ 0x2 };

int Model::m_numObjects{ 0 };

void Model::loadGeometry(
    const std::vector<double>* vertexData, const std::vector<int>* indices, std::string texturePath
) {
    m_geometryIsLoaded = true;

    m_numTriangles = ((int)indices->size()) / 3;

#pragma warning( push )
#pragma warning( disable : 4244 )
    std::vector<float> vertexDataFloat(vertexData->begin(), vertexData->end());
#pragma warning( pop )
    //unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure...
    // ...vertex attributes(s).
    glBindVertexArray(m_VAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(
        GL_ARRAY_BUFFER, sizeof(float) * vertexDataFloat.size(), &vertexDataFloat[0], GL_STATIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices->size(), &(*indices)[0], GL_STATIC_DRAW
    );

    //
    GLsizei stride;
    m_hasTexture = texturePath != "";
    if (m_hasTexture) {
        stride = 8 * sizeof(float); // x, y, z pos + x, y, z normal + x, y texture.
    } else {
        stride = 6 * sizeof(float); // x, y, z pos + x, y, z normal.
    }

    // Position.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    // Normal.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture.
    if (m_hasTexture) {
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex...
    // ...attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // remember: do NOT unbind the EBO while a VAO is active as the bound element buffer object IS...
    // ...stored in the VAO; keep the EBO bound.
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO,...
    // ...but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs...
    // ...(nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Load texture.
    if (m_hasTexture) {
        glGenTextures(1, &m_texture);
        m_textureIsLoaded = true;
        glBindTexture(GL_TEXTURE_2D, m_texture);
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
        glGenerateMipmap(GL_TEXTURE_2D);
        STBImageLoader::free(data);
        //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -10);
        //static void load(bool flipVertically, std::string path, int* width, int* height, int* nrChannels);
        //static void free(unsigned char* data);

        //stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the...
        //// ...y-axis.
        //// The FileSystem::getPath(...) is part of the GitHub repository so we can find files on...
        //// ...any IDE/platform; replace it with your own image path.
        //const char* texturePathC = texturePath.c_str();
        //unsigned char* data = stbi_load(
        //    texturePathC,
        //    &width, &height, &nrChannels, 0
        //);
        //if (data) {
        //    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        //    glGenerateMipmap(GL_TEXTURE_2D);
        //    //glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_LOD, -10);
        //} else {
        //    throw std::runtime_error("ERROR; Failed to load texture.");
        //}
        //stbi_image_free(data);


    }

    // Render flags.
    if (m_hasTexture) {
        //m_renderMode |= Model::RENDER_MODE_USE_TEXTURE;
        m_useTextures = true;
    }
}

void Model::loadGeometry(const std::string modelPath, const std::string texturePath) {
    std::vector<std::vector<double>> vertexData{};
    std::vector<std::vector<int>> indices{};
    bool modelHasTextures{ false };
    bool ignoreTextureCoordinates = texturePath == "";
    AssimpLoader::load(
        modelPath, &vertexData, &indices, &modelHasTextures, ignoreTextureCoordinates
    );
    if (texturePath != "" && !modelHasTextures) {
        std::cout << "ERROR: Texture coordinates not found in model." << std::endl;
        throw std::runtime_error("ERROR: Texture coordinates not found in model.");
    }
    loadGeometry(&vertexData[0], &indices[0], texturePath);
}

Model::Model() {
    m_numObjects++;
    if (m_numObjects == 1) {
        // Create shader program when the first mash is created.
        m_shaderProgram.loadVertexShaderFromPath("model_vertex_shader.vert");
        m_shaderProgram.loadFragmentShaderFromPath("model_fragment_shader.frag");
        m_shaderProgram.linkShaders();

        //m_shaderProgramShadows.loadVertexShaderFromPath("shaddow_vertex_shader.vert");
        //m_shaderProgramShadows.loadFragmentShaderFromPath("shaddow_fragment_shader.frag");
        //m_shaderProgramShadows.linkShaders();

        // Render shader program.
        m_modelLoc = glGetUniformLocation(m_shaderProgram.getID(), "model");
        m_viewLoc = glGetUniformLocation(m_shaderProgram.getID(), "view");
        m_projLoc = glGetUniformLocation(m_shaderProgram.getID(), "projection");
        m_camPosLoc = glGetUniformLocation(m_shaderProgram.getID(), "camPos");
        m_textureLoc = glGetUniformLocation(m_shaderProgram.getID(), "textureSampler");
        m_renderModeLoc = glGetUniformLocation(m_shaderProgram.getID(), "renderMode");
        m_colorLoc = glGetUniformLocation(m_shaderProgram.getID(), "color");
        m_timeLoc = glGetUniformLocation(m_shaderProgram.getID(), "time");
        // Shadows.
        //m_shadowMapLoc = glGetUniformLocation(m_shaderProgram.getID(), "shadowSampler");
        //m_lightSpaceMatrixLoc = glGetUniformLocation(m_shaderProgram.getID(), "lightSpaceMatrix");
        //m_shadowMapLoc2 = glGetUniformLocation(m_shaderProgram.getID(), "shadowSampler2");
        //m_lightSpaceMatrixLoc2 = glGetUniformLocation(m_shaderProgram.getID(), "lightSpaceMatrix2");

        // Shadow shader program.
        //m_shadowModelLoc = glGetUniformLocation(m_shaderProgramShadows.getID(), "model");
        //m_shadowLightSpaceMatrixLoc = glGetUniformLocation(m_shaderProgramShadows.getID(), "lightSpaceMatrix");
    }
}

Model::~Model() {
    m_numObjects--;
    if (m_numObjects == 0) {
        // Remove shader program when the last object is removed.
        m_shaderProgram.~ShaderProgram();
        m_shaderProgram = ShaderProgram{};
        m_shaderProgramShadows.~ShaderProgram();
        m_shaderProgramShadows = ShaderProgram{};
    }
    if (m_geometryIsLoaded) {
        glDeleteVertexArrays(1, &m_VAO);
        glDeleteBuffers(1, &m_VBO);
        glDeleteBuffers(1, &m_EBO);
    }
    if (m_textureIsLoaded) {
       glDeleteTextures(1, &m_texture);
    }
}

void Model::render(
    glm::dmat4 model, glm::dmat4 view, glm::dmat4 projection, glm::vec3 camPos, uint64_t time
) {
    m_shaderProgram.use();

    //double test = (shadowLightSpaceMatrix * glm::dvec4{1,2,3,1}).w;
    //std::cout << test << std::endl;

    // Update uniforms.
    glUniformMatrix4fv(m_modelLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)model));
    glUniformMatrix4fv(m_viewLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)view));
    glUniformMatrix4fv(m_projLoc, 1, GL_FALSE, glm::value_ptr((glm::mat4)projection));
    glUniform3f(m_camPosLoc, camPos.x, camPos.y, camPos.z);
    // Textures.
    glUniform1i(m_textureLoc, 0); // GL_TEXTURE0
    unsigned int renderMode =
        (m_selfIllumination ? Model::RENDER_MODE_SELF_ILLUMINATION : 0) +
        (m_useTextures ? Model::RENDER_MODE_USE_TEXTURE : 0);
    glUniform1i(m_renderModeLoc, renderMode);
    glUniform1i(m_timeLoc, (unsigned int)time);
    glUniform3fv(m_colorLoc, 1, &((glm::vec3)m_color)[0]);

    // Bind textures.
    // -Object texture.
    if (m_hasTexture) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture);
    }

    // Bind geometry.
    glBindVertexArray(m_VAO);

    // Render.
    glDrawElements(GL_TRIANGLES, m_numTriangles * 3, GL_UNSIGNED_INT, 0);
    //std::cout << "test!!!! " << m_numTriangles << std::endl;
}
