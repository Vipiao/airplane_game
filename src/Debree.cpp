#include "Debree.h"

Debree::Debree(uint64_t spawnTime) {
   m_spawnTime = spawnTime;
}

void Debree::updateMeshPositions() {
   m_mesh->m_position = m_rigidBody->m_pos;
   m_mesh->m_orientation = m_rigidBody->m_ori;
}
