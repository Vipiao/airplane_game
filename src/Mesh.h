#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Model.h"

class Mesh {
private:
public:
    Model* m_model;
    glm::dvec3 m_position{ 0,0,0 };
    glm::dquat m_orientation{ 1,0,0,0 };
    glm::dvec3  m_scale{ 1 };
    size_t m_index{ 0 };
    glm::dvec3 m_color{};
    bool m_useTexture{ true };
    bool m_doCulling{ true };
    bool m_isVisible{ true };
};

