#include "MovingParticleEffect.h"

MovingParticleEffect::MovingParticleEffect(uint64_t spawnTime) {
   m_spawnTime = spawnTime;
}

MovingParticleEffect::~MovingParticleEffect() {}

void MovingParticleEffect::updateMeshPositions() {
   m_particleEffectInstance->m_position = m_rigidBody->m_pos;
   //m_particleEffectInstance->m_particleEffect->setUniformInt("startTime", (int)m_spawnTime);
}
