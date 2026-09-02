#pragma once





#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ParticleEffect.h"

class ParticleEffectInstance {
private:
public:
   ParticleEffectInstance(uint64_t spawnTime);

   ParticleEffect* m_particleEffect{ nullptr };
   glm::dvec3 m_position{ 0,0,0 };
   glm::dquat m_orientation{ 1,0,0,0 };
   glm::dvec3  m_scale{ 1 };
   size_t m_index{ 0 };
   int m_numParticles{ 1 };
   uint64_t m_spawnTime{};
   bool m_isVisible{ true };
   double m_animationPace{ 1. };
};

