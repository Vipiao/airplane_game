#pragma once

#include "GraphicsEngine.h"
#include "PhysicsEngine.h"
#include "SpaceShip.h"
#include "Projectile.h"
#include "MovingParticleEffect.h"
#include "Debree.h"
#include "SoundPlayer.h"

class GameEngineCallback {
public:
   virtual void preRenderCallback(uint64_t frameNum) = 0;
};

class GameEngine : public GraphicsEngineCallback {
public:
   struct FrameLog {
      // Log containing this iterations data.
      std::vector<int> m_removedSpaceShipIds{};
      void emptyLog() {
         m_removedSpaceShipIds = {};
      }
   } m_frameLog{};
   GraphicsEngine m_graphicsEngine{};
   PhysicsEngine m_phyisicsEngine{};
   SoundPlayer m_soundPlayer{};
   int m_flaunchSound{};
   int m_explosionLargeSound{};
   int m_explosionSound{};
   int m_explosionMiniSound{};
   int m_scifiSound{};
   int m_windSound{};
   int m_rocketSound{};
   int m_1Sound{};
   int m_2Sound{};
   int m_3Sound{};
   int m_4Sound{};
   int m_victorySound{};
   int m_lossSound{};
   int64_t m_prevFrameStartTime{};
   double m_rollingFrameRate{ 144. };
   std::vector<Model*> m_spaceShipModels{};
   std::vector<Model*> m_spaceShipModelsBroken{};
   Model* m_sphereModel{};
   Model* m_spaceShipEnginePlasmaModel{};
   ParticleEffect* m_plasmaParticleEffect{};
   ParticleEffect* m_plasmaExplosionEffect{};
   ParticleEffect* m_plasmaExplosionEffectWater{};
   std::vector<glm::dvec3> m_spaceShipModelOffsets{};
   std::vector<glm::dvec3> m_spaceShipModelBrokenOffsets{};
   std::vector<SpaceShip*> m_spaceShips{};
   std::vector<Projectile*> m_projectiles{};
   std::vector<MovingParticleEffect*> m_movingParticleEffects{};
   Model* m_projectileModel{ nullptr };
   std::vector<GameEngineCallback*> m_gameEngineCallbacks{};
   std::vector<Planet*> m_planets{};
   std::vector<Debree*> m_debrees{};
   bool m_pause{ false };
   double m_globalVolumeMultiplier{ 0.5 };

   GameEngine();
   ~GameEngine();

   void loadModelWithOffsets(
      std::string modelPath, std::string texturePath,
      std::vector<Model*>* models,
      std::vector<glm::dvec3>* offsets
   );
   void playSoundAtLocation(glm::dvec3 position, int sound, double pitch, double gain, double maxVolume=1.);
   void startRenderLoop();
   void handleStatistics(uint64_t frameNum);
   void damageSpaceShip(int partIndex, SpaceShip* ss, double damage);
   void handleCloseToGroundEffect();
   void handleProjectileGroundCollision();
   void handleProjectileSpaceShipCollision();
   void handleSpaceShipGroundCollision();
   void handleDebreeGroundCollision();
   void handleGameLogic(uint64_t frameNum);
   void handlePhysics(uint64_t frameNum);
   void handleGraphics(uint64_t frameNum);
   void handleSoundEffects();
   void preRenderCallback(uint64_t frameNum);
   void framebufferSizeCallback(int width, int height);
   SpaceShip* createSpaceShip(glm::dvec3 position);
   void removeSpaceShip(SpaceShip* spaceShip);
   Projectile* createProjectile(glm::dvec3 position);
   void removeProjectile(Projectile* projectile);
   MovingParticleEffect* createMovingParticleEffect(ParticleEffect* particleEffect, glm::dvec3 position);
   void removeMovingParticleEffect(MovingParticleEffect* projectile);
   Planet* createPlanet();
   void removePlanet(Planet* planet);
   Debree* createDebree(glm::dvec3 position, Model* model);
   void removeDebree(Debree* debree);
};
