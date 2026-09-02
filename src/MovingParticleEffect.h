#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "ParticleEffectInstance.h"
#include "RigidBody.h"

class MovingParticleEffect {
protected:
public:
   MovingParticleEffect(uint64_t spawnTime);
   ~MovingParticleEffect();

   void updateMeshPositions();
   RigidBody* m_rigidBody{ nullptr };
   ParticleEffectInstance* m_particleEffectInstance{ nullptr };
   uint64_t m_lifeTime{ 100 };
   uint64_t m_spawnTime{ 0 };
   double airResistanceFactor{ 0. };
};

