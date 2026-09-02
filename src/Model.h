#pragma once

#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ShaderProgram.h"

class Model {
private:
    unsigned int m_VBO{ 0 };
    unsigned int m_VAO{ 0 };
    unsigned int m_EBO{ 0 };
    unsigned int m_texture{ 0 };
    bool m_textureIsLoaded{ false };
    int m_numTriangles{ 0 };

    static ShaderProgram m_shaderProgram;
    static int m_modelLoc;
    static int m_viewLoc;
    static int m_projLoc;
    static int m_camPosLoc;
    static int m_textureLoc;
    static int m_renderModeLoc;
    static int m_colorLoc;
    static int m_timeLoc;
    // Shadows.
    static int m_shadowMapLoc;
    static int m_lightSpaceMatrixLoc;
    static int m_shadowMapLoc2;
    static int m_lightSpaceMatrixLoc2;

    // Shadow shader program.
    static ShaderProgram m_shaderProgramShadows;
    static int m_shadowModelLoc;
    static int m_shadowLightSpaceMatrixLoc;

    static int m_numObjects;
    bool m_geometryIsLoaded{ false };
public:
    bool m_hasTexture{ false };
    glm::dvec3 m_color{ 1,1,1 };
    //unsigned int m_renderMode{ 0 };
    bool m_selfIllumination{ false };
    bool m_useTextures{ false };

    static unsigned int RENDER_MODE_SELF_ILLUMINATION;
    static unsigned int RENDER_MODE_USE_TEXTURE;

    void loadGeometry(
        const std::vector<double>* vertexData,
        const std::vector<int>* indices,
        std::string texturePath
    );
    void loadGeometry(const std::string modelPath, const std::string texturePath);
    Model();
    ~Model();
    void render(
        glm::dmat4 model, glm::dmat4 view, glm::dmat4 projection, glm::vec3 camPos, uint64_t time
    );
};

