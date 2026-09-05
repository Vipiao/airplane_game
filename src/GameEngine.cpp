



#include "GameEngine.h"

#include <iostream>

#include "GlobalConstants.h"
#include "DebugTimer.h"
#include "SpaceShip.h"
#include "AssimpLoader.h"
#include "HelperFunctions.h"
#include "CollisionDetection.h"
#include "Hash.h"
#include "PhysicsUnits.h"

GameEngine::GameEngine() {
   m_graphicsEngine.setCallbackObject(this);

   m_flaunchSound = m_soundPlayer.loadSound("../media/sound_effects/flaunch.wav");
   m_explosionLargeSound = m_soundPlayer.loadSound("../media/sound_effects/large_explosion.wav");
   m_explosionSound = m_soundPlayer.loadSound("../media/sound_effects/explode.wav");
   m_explosionMiniSound = m_soundPlayer.loadSound("../media/sound_effects/explodemini.wav");
   //m_scifiSound = m_soundPlayer.loadSound("../media/sound_effects/longscifisting.wav");
   m_windSound = m_soundPlayer.loadSound("../media/sound_effects/wind_2.wav");
   m_rocketSound = m_soundPlayer.loadSound("../media/sound_effects/rocket_launch.wav");
   m_1Sound = m_soundPlayer.loadSound("../media/sound_effects/down1.wav");
   m_2Sound = m_soundPlayer.loadSound("../media/sound_effects/down2.wav");
   m_3Sound = m_soundPlayer.loadSound("../media/sound_effects/down3.wav");
   m_4Sound = m_soundPlayer.loadSound("../media/sound_effects/down4.wav");
   m_victorySound = m_soundPlayer.loadSound("../media/sound_effects/victory.wav");
   m_lossSound = m_soundPlayer.loadSound("../media/sound_effects/loss.wav");

   m_projectileModel = m_graphicsEngine.createModel(
      "../media/models/projectile/projectile.obj",
      "../media/models/projectile/projectile_texture.png"
      //"../media/models/projectile/projectile_texture_64x64.png"
      //"../media/models/projectile/projectile_texture_samal_baby.png"
   );
   m_projectileModel->m_selfIllumination = true;

   loadModelWithOffsets(
      "../media/models/space_ship/space_ship_2.obj",
      "../media/models/space_ship/texture_v2.jpg",
      //"../media/models/space_ship/texture_samal.jpg",
      //"../media/models/space_ship/texture_samal_baby.jpg",
      //"../media/models/space_ship/texture_ryu.png",
      &m_spaceShipModels, &m_spaceShipModelOffsets
   );

   loadModelWithOffsets(
      "../media/models/space_ship/space_ship_broken.obj",
      "../media/models/space_ship/texture_v2.jpg",
      //"../media/models/space_ship/texture_samal.jpg",
      //"../media/models/space_ship/texture_samal_baby.jpg",
      //"../media/models/space_ship/texture_ryu.png",
      &m_spaceShipModelsBroken, &m_spaceShipModelBrokenOffsets
   );

   m_spaceShipEnginePlasmaModel = m_graphicsEngine.createModel(
      "../media/models/space_ship/engine_plasma.obj",
      "../media/models/space_ship/engine_plasma_128x128.png"
   );
   m_spaceShipEnginePlasmaModel->m_selfIllumination = true;

   // PlasmaParticleEffect.
   m_plasmaParticleEffect = m_graphicsEngine.createParticleEffect();
   m_plasmaParticleEffect->setTexture(
      "../media/models/space_ship/engine_plasma_128x128.png",
      "textureSampler"
   );
   std::string mapFunction{ HelperFunctions::loadTextFileFromPath("jet_effect.glsl") };
   m_plasmaParticleEffect->setParticleTimeFunction(mapFunction, mapFunction);

   // PlasmaExplosionEffect.
   m_plasmaExplosionEffect = m_graphicsEngine.createParticleEffect();
   m_plasmaExplosionEffect->setTexture(
      "../media/models/space_ship/engine_plasma_128x128.png",
      //"../media/models/projectile/projectile_texture_samal_baby.png",
      "textureSampler"
   );
   std::string mapFunction2{ HelperFunctions::loadTextFileFromPath("plasma_explosion_effect.glsl") };
   m_plasmaExplosionEffect->setParticleTimeFunction(mapFunction2, mapFunction2);

   // PlasmaExplosionEffectWater.
   m_plasmaExplosionEffectWater = m_graphicsEngine.createParticleEffect();
   m_plasmaExplosionEffectWater->setTexture(
      "../media/models/space_ship/engine_plasma_128x128.png",
      "textureSampler"
   );
   std::string mapFunction3{ HelperFunctions::loadTextFileFromPath("plasma_explosion_effect_water.glsl") };
   m_plasmaExplosionEffectWater->setParticleTimeFunction(mapFunction3, mapFunction3);

   //
   m_sphereModel = m_graphicsEngine.createModel(
      "../media/models/space_ship/sphere.obj",
      ""
   );
   m_sphereModel->m_selfIllumination = true;

   // Test.

   //ParticleEffect* particleEffect{
   //   m_graphicsEngine.createParticleEffect()
   //};
   //particleEffect->setTexture(
   //   "../media/models/space_ship/engine_plasma_128x128.png",
   //   "textureSampler"
   //);
   //std::string mf{ HelperFunctions::loadTextFileFromPath("plasma_explosion_effect.glsl") };
   //particleEffect->setParticleTimeFunction(mf, mf);
   //ParticleEffectInstance* particleEffectInstance{
   //   m_graphicsEngine.createParticleEffectInstance(particleEffect)
   //};
   //particleEffectInstance->m_position = glm::dvec3{ 0,0,0.1 };
   //particleEffectInstance->m_scale = glm::dvec3{ 0.1,0.1,0.1 };
   //particleEffectInstance->m_numParticles = 5;
   //particleEffectInstance->m_orientation = glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 });
}

GameEngine::~GameEngine() {
   //std::cout << "~GameEngine()" << std::endl;
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      delete m_spaceShips[ii];
   }
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      delete m_projectiles[ii];
   }
   for (size_t ii = 0; ii < m_planets.size(); ii++) {
      delete m_planets[ii];
   }
   for (size_t ii = 0; ii < m_movingParticleEffects.size(); ii++) {
      delete m_movingParticleEffects[ii];
   }
   for (size_t ii = 0; ii < m_debrees.size(); ii++) {
      delete m_debrees[ii];
   }
}

void GameEngine::loadModelWithOffsets(
   std::string modelPath, std::string texturePath,
   std::vector<Model*>* models,
   std::vector<glm::dvec3>* offsets
){
   std::vector<std::vector<double>> vertexData{};
   std::vector<std::vector<int>> indices{};

   bool hasTextures{};
   bool ignoreTextureCoordinates{ false };
   AssimpLoader::load(modelPath, &vertexData, &indices, &hasTextures, ignoreTextureCoordinates);

   for (size_t ii = 0; ii < vertexData.size(); ii++) {
      for (size_t jj = 0; jj < 2; jj++) {
         //if (jj == 1 && !(ii == 1 || ii == 2)) {
         if (jj == 1) {
            continue;
         }

         std::vector<double> vd{ vertexData[ii] };

         // Center model on average vertex position. Rough estimate of center of mass..
         // Data packing: xyz xyz    uv
         //               pos normal texture coordinate
         glm::dvec3 center{ 0,0,0 };
         for (size_t kk = 0; kk < vd.size(); kk += 8) {
            center += glm::dvec3{ vd[kk], vd[kk + 1], vd[kk + 2] };
         }
         center /= vd.size() / 8;
         offsets->push_back(center);
         for (size_t kk = 0; kk < vd.size(); kk += 8) {
            vd[kk] -= center.x;
            vd[kk + 1] -= center.y;
            vd[kk + 2] -= center.z;
         }
         Model* model{ m_graphicsEngine.createModel(
            &vd, &indices[ii],
            texturePath//"../media/models/space_ship/texture_v2.jpg"
         ) };
         //m_spaceShipModels.push_back(model);
         models->push_back(model);
      }
   }
}

void GameEngine::playSoundAtLocation(glm::dvec3 position, int sound, double pitch, double gain, double maxVolume) {
   double volume{ 1. / glm::length2(position - m_graphicsEngine.m_camPos)};
   volume *= gain * 100.;
   if (volume > maxVolume) {
      volume = maxVolume;
   }
   if (volume < 0.01) {
      return;
   }
   m_soundPlayer.playSound(sound, pitch, volume * m_globalVolumeMultiplier);
}

void GameEngine::startRenderLoop() {
   m_graphicsEngine.renderLoop();
}

void GameEngine::handleStatistics(uint64_t /*frameNum*/) {
   uint64_t currentTime{ (uint64_t)std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::system_clock::now().time_since_epoch()
   ).count() };
   double frameRate = 1.e9 / (double)(currentTime - m_prevFrameStartTime);
   m_rollingFrameRate = glm::mix(m_rollingFrameRate, frameRate, 0.01);
   if (m_graphicsEngine.m_frameNum % 144 == 0) {
      std::cout << m_rollingFrameRate << std::endl;
      //std::cout << m_graphicsEngine.m_camPos.z << std::endl;
   }
   m_prevFrameStartTime = currentTime;
}

void GameEngine::damageSpaceShip(int partIndex, SpaceShip* ss, double damage) {
   SpaceShipPart* ssp{ &ss->m_spaceShipParts[partIndex] };
   if (ssp->m_health < 0.) {
      return;
   }
   // Damage ship.
   double prevHealth{ ssp->m_health };
   //if (partIndex == 0) {
   //   damage *= 0.3;
   //}
   ssp->m_health -= damage;
   if (ssp->m_health < ssp->m_maxHealth * 0.5) {
      ssp->m_meshCurrent->m_isVisible = false;
      ssp->m_meshCurrent = ssp->m_meshHealthMedium;
      ssp->m_meshCurrent->m_isVisible = true;
   }
   if (prevHealth >= 0. && ssp->m_health < 0.) {
      ssp->m_meshCurrent->m_isVisible = false;
      if (partIndex == 2) {
         ss->m_plasmaEffectR->m_isVisible = false;
         ss->m_plasmaR->m_isVisible = false;
      }
      if (partIndex == 3) {
         ss->m_plasmaEffectL->m_isVisible = false;
         ss->m_plasmaL->m_isVisible = false;
      }
      // Create debree.
      Debree* debree{ createDebree(
         ss->m_rigidBody->m_pos + ss->m_rigidBody->m_ori * (ssp->m_offset - ss->m_centerOfMass),
         //ssp->m_meshCurrent->m_position,
         m_spaceShipModelsBroken[partIndex]
      ) };
      debree->m_rigidBody->m_ori = ss->m_rigidBody->m_ori;
      debree->m_rigidBody->m_vel = ss->m_rigidBody->m_vel +
         glm::cross(ss->m_rigidBody->getAngularVel(), debree->m_rigidBody->m_pos - ss->m_rigidBody->m_pos);
      debree->m_mesh->m_scale = ssp->m_meshCurrent->m_scale;
      debree->m_rigidBody->m_rotationAxis = ss->m_rigidBody->m_rotationAxis;
      debree->m_rigidBody->m_rotationVel = ss->m_rigidBody->m_rotationVel;

      // Add noise.
      if (debree->m_rigidBody->m_rotationVel < 0.2) {
         debree->m_rigidBody->m_rotationVel *= 2.;
      }
      
      //debree->m_rigidBody->m_vel +=
      //   (debree->m_rigidBody->m_pos - ss->m_rigidBody->m_pos) * 0.02;
      //debree->m_rigidBody->m_vel +=
      //   (Hash::pcgUnit3(m_graphicsEngine.m_frameNum) - glm::dvec3{ 0.5,0.5,0.5 }) * 0.00002;
      //debree->m_rigidBody->m_rotationAxis =
      //   (Hash::pcgUnit3(m_graphicsEngine.m_frameNum+1) - glm::dvec3{ 0.5,0.5,0.5 }) * 0.05;
      //debree->m_rigidBody->m_rotationVel = glm::length(debree->m_rigidBody->m_rotationAxis);
      if (glm::length2(debree->m_rigidBody->m_rotationAxis) > 0.) {
         debree->m_rigidBody->m_rotationVel += 0.004 * glm::sign(debree->m_rigidBody->m_rotationVel)*
            Hash::pcgUnit(m_graphicsEngine.m_frameNum + partIndex);
      }
      
      // Create explosion effect.
      MovingParticleEffect* mpe{ createMovingParticleEffect(
         m_plasmaExplosionEffect,
         debree->m_rigidBody->m_pos
      ) };
      double size{ 20. };
      mpe->m_particleEffectInstance->m_scale = { size,size,size };
      mpe->m_lifeTime = PhysicsUnits::ticks(3.6);
      mpe->m_particleEffectInstance->m_animationPace /= 3.;
      mpe->m_particleEffectInstance->m_numParticles = 1;
      mpe->m_rigidBody->m_vel = debree->m_rigidBody->m_vel;
      mpe->airResistanceFactor = PhysicsUnits::blendHalfLife(0.57472969947107067);
      double pitch{ 0.5 + Hash::pcgUnit(m_graphicsEngine.m_frameNum) };
      playSoundAtLocation(debree->m_rigidBody->m_pos, m_explosionSound, pitch, 500, 0.5);
      
      // If the destroyed part is central to another part, destroy it too.
      if (partIndex == 1) {
         damageSpaceShip(2, ss, 999.);
      }
      if (partIndex == 4) {
         damageSpaceShip(3, ss, 999.);
      }
      if (partIndex == 0) {
         for (size_t ii = 0; ii < ss->m_spaceShipParts.size(); ii++) {
            if (ss->m_spaceShipParts[ii].m_health >= 0.) {
               damageSpaceShip((int)ii, ss, 999.);
            }
         }
      }
   }
}

void GameEngine::handleCloseToGroundEffect() {// Close to the ground effect.
   //if (frameNum % (144 / 10) == (Hash::pcg(frameNum) % 10)) {
   if (m_graphicsEngine.m_frameNum % (144 / 20) == 0) {
      for (size_t ii = 0; ii < m_planets.size(); ii++) {
         Planet* planet{ m_planets[ii] };
         for (size_t jj = 0; jj < m_spaceShips.size(); jj++) {
            SpaceShip* spaceShip{ m_spaceShips[ii] };
            RigidBody* rigidBody{ spaceShip->m_rigidBody };
            double elevation{ planet->map(rigidBody->m_pos, 3).z };
            MovingParticleEffect* mpe{ nullptr };
            if (elevation == 0.0000 && rigidBody->m_pos.z < 66.) {
               glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
               glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
               glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };
               for (size_t kk = 0; kk < 2; kk++) {
                  glm::dvec3 effectPos{ rigidBody->m_pos };
                  if (kk == 1) {
                     effectPos += forward * 16.384 + right * 9.8304 * up.z;
                  } else {
                     effectPos += forward * 16.384 - right * 9.8304 * up.z;
                  }
                  effectPos.z = 0.;
                  mpe = createMovingParticleEffect(
                     m_plasmaExplosionEffectWater, effectPos
                  );
                  /*
                  mpe->m_lifeTime = 144 * 2;
                  mpe->m_rigidBody->m_vel.z = 0.00005;
                  mpe->m_particleEffectInstance->m_animationPace = 0.25;
                  double ss{ 0.00075 };
                  mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
                  */
                  mpe->m_lifeTime = PhysicsUnits::ticks(1.2);
                  mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(60.);
                  double ss{ 1.6384 };
                  mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
                  mpe->m_particleEffectInstance->m_numParticles = 1;
                  mpe->m_rigidBody->m_vel = rigidBody->m_vel;
                  mpe->m_rigidBody->m_vel.z = PhysicsUnits::metersPerSecond(39.3216);
                  mpe->airResistanceFactor = PhysicsUnits::blendHalfLife(0.57472969947107067);
               }
            }
         }
      }
   }
}

void GameEngine::handleProjectileGroundCollision() {

   // Projectile ground collision.
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      Projectile* pp{ m_projectiles[ii] };
      for (size_t jj = 0; jj < m_planets.size(); jj++) {
         Planet* planet{ m_planets[jj] };
         RigidBody* rr{ pp->m_rigidBody };
         double elevation{ planet->map(rr->m_pos, 3).z };
         if (rr->m_pos.z < elevation) {
            // Calculate intersection point.
            MovingParticleEffect* mpe{ nullptr };
            glm::dvec3 groundPos = planet->findGroundIntersectionPoint(rr->m_pos - rr->m_vel, rr->m_vel, 6);
            //double velL{ glm::length(rr->m_vel) };
            //if (velL > 0.) {
            //   groundPos -= rr->m_vel / velL * pp->m_size * 2.;
            //}
            glm::dvec3 normal{ planet->mapNormal(groundPos, 3) };
            groundPos += normal * pp->m_size * 2.;
            //groundPos.z = elevation;
            //
            if (elevation == 0.) {
               mpe = createMovingParticleEffect(
                  m_plasmaExplosionEffectWater, groundPos
               );
               mpe->m_lifeTime = PhysicsUnits::ticks(2.4);
               mpe->m_rigidBody->m_vel.z = PhysicsUnits::metersPerSecond(196.608);
               mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(30.);
               double ss{ 24.576 };
               mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
            } else {
               mpe = createMovingParticleEffect(
                  m_plasmaExplosionEffect, groundPos
               );
               mpe->m_lifeTime = PhysicsUnits::ticks(0.6);
               mpe->m_rigidBody->m_vel.z = PhysicsUnits::metersPerSecond(196.608);
               mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(120.);
               double ss{ 24.576 };
               mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
            }
            mpe->m_particleEffectInstance->m_numParticles = 1;
            //glm::dvec3 normal{ planet->mapNormal(rr->m_pos, 3) };
            //mpe->m_rigidBody->m_vel += normal * 0.00005;
            glm::dvec3 dir{ glm::mix(
               pp->m_rigidBody->m_vel - glm::dot(pp->m_rigidBody->m_vel, normal) * normal * 2.,
               normal,
               1.0
            ) };
            mpe->m_rigidBody->m_vel =
               dir * 1.;
            //mpe->m_rigidBody->m_vel = pp->m_rigidBody->m_vel * 0.1;
            playSoundAtLocation(
               rr->m_pos, m_explosionMiniSound,
               0.5 + Hash::pcgUnit(m_graphicsEngine.m_frameNum) * 4.0,
               1000., 0.06
            );
            //
            removeProjectile(pp);
            ii--;
            break;
         }
      }
      //MovingParticleEffect* mpe{ m_gameEngine->createMovingParticleEffect(m_gameEngine->m_plasmaExplosionEffect) };
      //mpe->m_rigidBody->m_pos = { 0,0,0.01 };
      //double ss{ 0.01 };
      //mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
      //mpe->m_lifeTime = 99999;
      ////mpe->m_rigidBody->m_vel.z = 0.001;
   }
}

void GameEngine::handleProjectileSpaceShipCollision() {

   // Projectile space ship collision.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      SpaceShip* ss{ m_spaceShips[ii] };
      for (size_t jj = 0; jj < m_projectiles.size(); jj++) {
         Projectile* pp{ m_projectiles[jj] };
         if (pp->m_owner == ss->m_id) {
            continue;
         }
         if (!(
            ss->m_left > pp->m_right && ss->m_leftPrev > pp->m_rightPrev ||
            ss->m_right < pp->m_left && ss->m_rightPrev < pp->m_leftPrev ||
            ss->m_back > pp->m_front && ss->m_backPrev > pp->m_frontPrev ||
            ss->m_front < pp->m_back && ss->m_frontPrev < pp->m_backPrev ||
            ss->m_bottom > pp->m_top && ss->m_bottomPrev > pp->m_topPrev ||
            ss->m_top < pp->m_bottom && ss->m_topPrev < pp->m_bottomPrev
            )) {
            // Collision box detected.
            bool didCollide{ false };
            for (size_t kk = 0; kk < ss->m_spaceShipParts.size(); kk++) {
               SpaceShipPart* sps{ &ss->m_spaceShipParts[kk] };
               if (sps->m_health < 0.) {
                  continue;
               }
               for (size_t ll = 0; ll < sps->m_collisionSpheres.size(); ll++) {
                  SpaceShipPart::CollisionSphere* cs{ &sps->m_collisionSpheres[ll] };
                  if (CollisionDetection::roundedCylinderSphereCollisionIsColliding(
                     pp->m_rigidBody->m_posPrev, pp->m_rigidBody->m_pos, pp->m_size,
                     ss->m_rigidBody->m_posPrev + ss->m_rigidBody->m_oriPrev * cs->m_pos0,
                     ss->m_rigidBody->m_posPrev + ss->m_rigidBody->m_oriPrev * cs->m_pos1,
                     ss->m_rigidBody->m_pos + ss->m_rigidBody->m_ori * cs->m_pos0,
                     ss->m_rigidBody->m_pos + ss->m_rigidBody->m_ori * cs->m_pos1,
                     cs->m_radius
                  )) {
                     //std::cout << "k: " << kk << std::endl;

                     // Damage ship.
                     damageSpaceShip((int)kk, ss, pp->m_damage);
                     ss->m_lastDamageId = pp->m_owner;
                     ss->lastTimeHit = m_graphicsEngine.m_frameNum;

                     //
                     didCollide = true;
                     // Do not break all the way out of loop, allowing multiple parts to be hit at the same time.
                     break;
                     //goto outer; // Break out of all loops.
                  }
               }
            }
            //outer:
            //
            if (didCollide) {

               // Create explosion effect.
               // --Calculate closest position.
               glm::dvec3 relativePos = ss->m_rigidBody->m_pos - pp->m_rigidBody->m_pos;
               glm::dvec3 relativeVel = ss->m_rigidBody->m_vel - pp->m_rigidBody->m_vel;
               double denominator{ glm::dot(relativeVel, relativeVel) };
               double t{ 0.0 };
               if (denominator != 0.0) {
                  t = -glm::dot(relativePos, relativeVel) / denominator;
               }
               glm::dvec3 pPosAtT{ pp->m_rigidBody->m_pos + pp->m_rigidBody->m_vel * t };
               //glm::dvec3 sPosAtT{ ss->m_rigidBody->m_pos + ss->m_rigidBody->m_vel * t };
               //
               MovingParticleEffect* mpe{ createMovingParticleEffect(
                  m_plasmaExplosionEffect,
                  pPosAtT
                  //pp->m_rigidBody->m_pos - (pp->m_rigidBody->m_vel - ss->m_rigidBody->m_vel) * 0.5
               ) };
               double size{ 10. };
               double scaleFactor{ Hash::pcgUnit(m_graphicsEngine.m_frameNum + ii) };
               size *= scaleFactor * scaleFactor * 0.7 + 0.3;
               mpe->m_particleEffectInstance->m_scale = { size,size,size };
               mpe->m_lifeTime = PhysicsUnits::ticks(1.8);
               mpe->m_particleEffectInstance->m_animationPace /= 3.;
               mpe->m_particleEffectInstance->m_numParticles = 1;
               mpe->m_rigidBody->m_vel = ss->m_rigidBody->m_vel;
               mpe->airResistanceFactor = PhysicsUnits::blendHalfLife(0.28591348742933892);
               //mpe->m_rigidBody->m_vel.z = 0.00005;
               playSoundAtLocation(mpe->m_rigidBody->m_pos, m_explosionMiniSound, 1.0, 500., 0.1);

               //
               removeProjectile(pp);
               jj--;
            }
         }
      }
   }
}

void GameEngine::handleSpaceShipGroundCollision() {

   // Space ship ground collision.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      SpaceShip* spaceShip{ m_spaceShips[ii] };
      glm::dvec3 spaceShipPos{ spaceShip->m_rigidBody->m_pos };
      glm::dquat spaceShipOri{ spaceShip->m_rigidBody->m_ori };
      for (size_t jj = 0; jj < m_planets.size(); jj++) {
         Planet* planet{ m_planets[jj] };
         double elevation{ planet->map(spaceShipPos, 3).z };
         double height{ spaceShipPos.z - elevation };
         if (height > spaceShip->m_size) {
            continue;
         }
         double maxPenetration{ -1. };
         for (size_t kk = 0; kk < spaceShip->m_spaceShipParts.size(); kk++) {
            SpaceShipPart* ssp{ &spaceShip->m_spaceShipParts[kk] };
            if (ssp->m_health < 0.) {
               continue;
            }
            for (size_t ll = 0; ll < ssp->m_collisionSpheres.size(); ll++) {
               SpaceShipPart::CollisionSphere cs{ ssp->m_collisionSpheres[ll] };
               glm::dvec3 shift{ spaceShipOri * cs.m_pos0 };
               glm::dvec3 sPos{ spaceShipPos + shift };
               for (size_t mm = 0; mm < 2; mm++) {
                  if (mm == 1) {
                     sPos = spaceShipPos + spaceShipOri * cs.m_pos1;
                  }
                  double elevationPart{ planet->map(sPos, 3).z };
                  double hh{ sPos.z - elevationPart };
                  if (hh < cs.m_radius) {
                     double damageFactor{ 2.0 };
                     glm::dvec3 normal{ planet->mapNormal(sPos, 3) };
                     //ss->m_rigidBody->m_vel +
                     //glm::cross(ss->m_rigidBody->getAngularVel(), debree->m_rigidBody->m_pos - ss->m_rigidBody->m_pos);
                     glm::dvec3 sSpeed{
                        spaceShip->m_rigidBody->m_vel +
                        glm::cross(spaceShip->m_rigidBody->getAngularVel(), sPos - spaceShipPos)
                     };
                     double projVel{ glm::dot(normal, sSpeed) };
                     //double damage{ glm::length2(spaceShip->m_rigidBody->m_vel) * damageFactor + 0.1 };
                     double damage{ glm::abs(projVel) * damageFactor + 0.1*0.};
                     if (damage > ssp->m_health) {
                        for (size_t nn = 0; nn < spaceShip->m_spaceShipParts.size(); nn++) {
                           damageSpaceShip(
                              (int)nn, spaceShip,
                              spaceShip->m_spaceShipParts[nn].m_health * 0.6
                           );
                        }
                     }
                     damageSpaceShip((int)kk, spaceShip, damage);
                     maxPenetration = glm::max(maxPenetration, cs.m_radius - hh);
                  }
               }
            }
         }
         if (spaceShip->m_spaceShipParts[0].m_health < 0.) {
            // Calculate intersection point if spaceship explodes under the ground.
            //glm::dvec3 groundPos = planet->findGroundIntersectionPoint(
            //   spaceShipPos - spaceShip->m_rigidBody->m_vel * 2., spaceShip->m_rigidBody->m_vel * 2., 6
            //);
            glm::dvec3 groundPos = spaceShipPos;
            groundPos -= spaceShip->m_rigidBody->m_vel*0.5;
            //
            for (size_t kk = 0; kk < 2; kk++) {
               MovingParticleEffect* mpe{ createMovingParticleEffect(
                  m_plasmaExplosionEffect,
                  groundPos
               ) };
               //mpe->m_rigidBody->m_pos.z = elevation + 0.0005 * 0.;
               glm::dvec3 normal{ planet->mapNormal(spaceShipPos, 2) };

               mpe->m_rigidBody->m_vel = spaceShip->m_rigidBody->m_vel -
                  normal * glm::dot(normal, spaceShip->m_rigidBody->m_vel);
               double size{ 1. };
               double speedScaleFactor{ 1. };
               if (kk == 1) {
                  mpe->m_particleEffectInstance->m_numParticles = 4;
                  mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(4.);
                  mpe->m_lifeTime = PhysicsUnits::ticks(18.);
                  mpe->m_rigidBody->m_vel += normal * PhysicsUnits::metersPerSecond(1920.) * speedScaleFactor;
                  mpe->airResistanceFactor = PhysicsUnits::blendHalfLife(0.054823445658004864);
                  size = 131.072;
               } else {
                  mpe->m_particleEffectInstance->m_numParticles = 1;
                  mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(12.);
                  mpe->m_lifeTime = PhysicsUnits::ticks(6.);
                  mpe->m_rigidBody->m_vel += normal * PhysicsUnits::metersPerSecond(720.) * speedScaleFactor;
                  mpe->airResistanceFactor = PhysicsUnits::blendHalfLife(0.054823445658004864);
                  size = 65.536;
               }
               mpe->m_particleEffectInstance->m_scale = { size,size,size };
               //mpe->m_rigidBody->m_vel.x = spaceShip->m_rigidBody->m_vel.x * 1.;
               //mpe->m_rigidBody->m_vel.y = spaceShip->m_rigidBody->m_vel.y * 1.;
            }
            //
         }
         if (maxPenetration > 0.) {
            glm::dvec3 normal{ planet->mapNormal(spaceShip->m_rigidBody->m_pos, 3) };
            double projVel{ glm::dot(normal, spaceShip->m_rigidBody->m_vel) };
            if (projVel < 0.) {
               double bounceFactor{ 0. };
               spaceShip->m_rigidBody->m_vel += -normal * projVel * (1. + bounceFactor);
               if (spaceShip->m_rigidBody->m_rotationVel < 0.1) {
                  spaceShip->m_rigidBody->m_rotationVel += 0.01 * glm::sign(spaceShip->m_rigidBody->m_rotationVel) *
                     Hash::pcgUnit(m_graphicsEngine.m_frameNum + spaceShip->m_id);
               }
            }
         }
      }
   }
}

void GameEngine::handleDebreeGroundCollision() {

   // Debree ground collision.
   for (size_t ii = 0; ii < m_planets.size(); ii++) {
      Planet* planet{ m_planets[ii] };
      for (size_t jj = 0; jj < m_debrees.size(); jj++) {
         Debree* debree{ m_debrees[jj] };
         double elevation{ planet->map(debree->m_rigidBody->m_pos, 3).z };
         double hh{ debree->m_rigidBody->m_pos.z - elevation };
         if (hh < 0.) {
            debree->m_health -= 0.1;
            if (debree->m_health < 0.) {
               MovingParticleEffect* mpe{ createMovingParticleEffect(
                  m_plasmaExplosionEffect,
                  debree->m_rigidBody->m_pos + glm::dvec3{0,0,-hh}
               ) };
               double size{ 32.768 };
               mpe->m_particleEffectInstance->m_scale = { size,size,size };
               mpe->m_lifeTime = PhysicsUnits::ticks(2.4);
               mpe->m_particleEffectInstance->m_numParticles = 1;
               mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(60.);
               mpe->m_rigidBody->m_vel.z = PhysicsUnits::metersPerSecond(157.2864);
               double pitch{ Hash::pcgUnit(jj, m_graphicsEngine.m_frameNum)*1. + 1.0 };
               playSoundAtLocation(
                  mpe->m_rigidBody->m_pos, m_explosionLargeSound,
                  pitch, 10000., 0.5
               );
               removeDebree(debree);
               jj--;
            } else {
               glm::dvec3 normal{ planet->mapNormal(debree->m_rigidBody->m_pos, 3) };
               double projVel{ glm::dot(normal, debree->m_rigidBody->m_vel) };
               if (projVel < 0.) {
                  double bounceFactor{ 0.3 };
                  debree->m_rigidBody->m_vel += -normal * projVel * (1. + bounceFactor);
                  debree->m_rigidBody->m_rotationVel += 0.1 * glm::sign(debree->m_rigidBody->m_rotationVel) *
                     Hash::pcgUnit(m_graphicsEngine.m_frameNum + jj);
                  //std::cout << "projVel: " << projVel << std::endl;
                  if (projVel < -2.) {
                     MovingParticleEffect* mpe{ createMovingParticleEffect(
                        m_plasmaExplosionEffect,
                        debree->m_rigidBody->m_pos + glm::dvec3{0,0,-hh}
                     ) };
                     double size{ 32.768 };
                     mpe->m_particleEffectInstance->m_scale = { size,size,size };
                     mpe->m_lifeTime = PhysicsUnits::ticks(2.4);
                     mpe->m_particleEffectInstance->m_numParticles = 1;
                     mpe->m_particleEffectInstance->m_animationPace = PhysicsUnits::perSecond(60.);
                     mpe->m_rigidBody->m_vel.z = PhysicsUnits::metersPerSecond(157.2864);
                     double pitch{ Hash::pcgUnit(jj, m_graphicsEngine.m_frameNum) * 1. + 1.0 };
                     playSoundAtLocation(
                        mpe->m_rigidBody->m_pos, m_explosionLargeSound,
                        pitch, 10000., 0.5
                     );
                  }
               }
            }
         }
      }
   }
}

void GameEngine::handleGameLogic(uint64_t frameNum) {

   //std::cout << "TTT " << m_projectiles.size() << std::endl;
   handleCloseToGroundEffect();

   handleProjectileGroundCollision();

   handleProjectileSpaceShipCollision();

   handleSpaceShipGroundCollision();

   handleDebreeGroundCollision();

   // Remove bullets after some time.
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      Projectile* projectile{ m_projectiles[ii] };
      if (projectile->m_spawnTime + projectile->m_lifeTime < frameNum) {
         removeProjectile(m_projectiles[ii]);
         ii--;
      }
   }

   // Remove movingParticleEffects.
   for (size_t ii = 0; ii < m_movingParticleEffects.size(); ii++) {
      MovingParticleEffect* mpe = m_movingParticleEffects[ii];
      if (mpe->m_spawnTime + mpe->m_lifeTime < frameNum) {
         removeMovingParticleEffect(mpe);
         ii--;
      }
   }

   // Check for if spaceShip is completely destroyed.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      SpaceShip* ss{ m_spaceShips[ii] };
      if (ss->m_spaceShipParts[0].m_health < 0.) {
         // Create explosion effect.
         MovingParticleEffect* mpe{ createMovingParticleEffect(
                        m_plasmaExplosionEffect,
                        ss->m_rigidBody->m_pos
                     ) };
         double size{ 32.768 };
         mpe->m_particleEffectInstance->m_scale = { size,size,size };
         mpe->m_lifeTime = PhysicsUnits::ticks(0.6);
         mpe->m_particleEffectInstance->m_numParticles = 1;
         mpe->m_rigidBody->m_vel = ss->m_rigidBody->m_vel;
         // Credit last damage given.
         for (size_t jj = 0; jj < m_spaceShips.size(); jj++) {
            SpaceShip* other{ m_spaceShips[jj] };
            if (other->m_id == ss->m_lastDamageId) {
               other->m_numKills++;
               break;
            }
         }
         //
         double pitch{ Hash::pcgUnit(ii, m_graphicsEngine.m_frameNum) * 0.5 + 0.5 };
         playSoundAtLocation(
            ss->m_rigidBody->m_pos, m_explosionLargeSound,
            pitch, 100000., 0.5
         );
         //
         removeSpaceShip(ss);
         ii--;
      }
   }


}

void GameEngine::handlePhysics(uint64_t /*frameNum*/) {
   // Space ship physics.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      SpaceShip* spaceShip{ m_spaceShips[ii] };
      RigidBody* rigidBody(spaceShip->m_rigidBody);

      // Game physics.
      glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
      glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
      glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };

      // Torque.
      glm::dvec3 torque{ 0,0,0 };
      // Thrust.
      spaceShip->updateCapacitor();
      double thrust{ spaceShip->m_thrust * spaceShip->m_thrustMultiplier };
      //if (m_keyLShiftDown) {
      //   thrust *= 2.;
      //}
      std::vector<SpaceShipPart>* ssp{ &m_spaceShips[ii]->m_spaceShipParts };
      double rWingHealth{ glm::max((*ssp)[1].m_health / (*ssp)[1].m_maxHealth, 0.) };
      double lWingHealth{ glm::max((*ssp)[4].m_health / (*ssp)[4].m_maxHealth, 0.) };
      bool missingREngine{ (*ssp)[2].m_health < 0. };
      bool missingLEngine{ (*ssp)[3].m_health < 0. };
      
      if (missingREngine) {
         if (missingLEngine) {
            thrust = 0.;
         } else {
            thrust *= 0.5;
            torque -= up * thrust * 0.0122;
         }
      } else {
         if (missingLEngine) {
            thrust *= 0.5;
            torque += up * thrust * 0.0122;
         }
      }
      rigidBody->m_vel += forward * thrust;

      // Lift.
      constexpr double k_liftSpeedScale{ 3794.7331922020555 };
      double liftFactor{ 2.0 };
      double forwardSpeed{
         PhysicsUnits::toMetersPerSecond(glm::dot(rigidBody->m_vel, forward)) / k_liftSpeedScale
      };
      double lift{ liftFactor * forwardSpeed * forwardSpeed };
      //m_spaceShip->m_rigidBody->m_vel += up * lift;
      //torque += glm::dvec3{0, 0, 1} * lift * 20. * right.z;

      // Air resistance.
      // Translational.
      double resistanceEffectOnAirResistance{0.5};
      double airResistanceFactor{ glm::mix(
         0.05,
         glm::min(lift, 1.), resistanceEffectOnAirResistance
      ) };
      //airResistanceFactor = 0.;
      glm::dvec3 airResistance{ // Local coordiante system of space ship.
         -glm::dot(rigidBody->m_vel, right) * PhysicsUnits::perSecond(24.), // Sideways.
         -glm::dot(rigidBody->m_vel, forward) * PhysicsUnits::perSecond(2.4), // Forward.
         -glm::dot(rigidBody->m_vel, up) * PhysicsUnits::perSecond(60.), // Up.
      };
      airResistance *= glm::pow(rWingHealth * lWingHealth, 0.5);
      // Make global coordinate system.
      double uppwardAirResistance{ airResistance.z };
      airResistance = right * airResistance.x + forward * airResistance.y + up * airResistance.z;
      rigidBody->m_vel += airResistance * airResistanceFactor;
      // Rotational.
      //double aliveFriction{ 0.98 };
      //double deadFriction{ 0.999 };
      //rigidBody->m_rotationVel *= glm::mix(deadFriction, aliveFriction, rWingHealth * lWingHealth);
      rigidBody->m_rotationVel *= PhysicsUnits::halfLife(0.2859134874293379);
      //
      double liftTurnFactor{ PhysicsUnits::radiansPerSecondSquared(0.9216) };
      torque += glm::dvec3{ 0,0,01 } * lift * right.z * liftTurnFactor;
      // Dihedral.
      torque += forward * lift * liftTurnFactor * right.z;

      // Counter torque.
      //torque += -forward * glm::dot(rigidBody->getAngularVel(), forward) * 0.01;

      // Missing wings.
      double wingFactorAirResistance{ 0.00122 };
      double wingFactorLift{ PhysicsUnits::radiansPerSecondSquared(129.6) };
      double wingFactorTurnDown{ PhysicsUnits::radiansPerSecondSquared(11.52) };
      double effectFactor{ 0. };
      if (rWingHealth < 1.) {
         effectFactor += glm::pow(1. - rWingHealth, 2.);
      }
      if (lWingHealth < 1.) {
         effectFactor -= glm::pow(1. - lWingHealth, 2.);
      }
      if (effectFactor != 0.) {
         //double reduceTimeFactor{ 1. / (1. + 0.0004 * (double)(frameNum - spaceShip->lastTimeHit)) };
         //effectFactor *= reduceTimeFactor;
         torque += forward * uppwardAirResistance * wingFactorAirResistance * effectFactor;
         torque += forward * lift * wingFactorLift * effectFactor;
         torque -= right * up.z * lift * wingFactorTurnDown *
            glm::pow(1. - rWingHealth * lWingHealth, 4.);
         //   glm::pow(1. - rWingHealth * lWingHealth, 4.) * reduceTimeFactor;
      }
      //if (rWingHealth < 1.) { // Right wing.
      //   double effectFactor{ glm::pow(1. - rWingHealth, 4.) };
      //   effectFactor *= 1./(1. + 0.001*(double)(frameNum - spaceShip->lastTimeHit));
      //   torque += forward * uppwardAirResistance * wingFactorAirResistance * effectFactor;
      //   torque += forward * lift * wingFactorLift * effectFactor;
      //   torque += -right * up.z * lift * wingFactorTurnDown * effectFactor; // Make the plane turn down.
      //}
      //if (lWingHealth < 1.) { // Left wing.
      //   double effectFactor{ glm::pow(1. - lWingHealth, 4.) };
      //   effectFactor *= 1. / (1. + 0.001 * (double)(frameNum - spaceShip->lastTimeHit));
      //   torque -= forward * uppwardAirResistance * wingFactorAirResistance * effectFactor;
      //   torque -= forward * lift * wingFactorLift * effectFactor;
      //   torque -= right * up.z * lift * wingFactorTurnDown * effectFactor;
      //}

      // TEST START.
      //for (size_t jj = 0; jj < m_planets.size(); jj++) {
      //   Planet* planet{ m_planets[jj] };
      //   double elevation{ rigidBody->m_pos.z - planet->map(rigidBody->m_pos, 3).z };
      //   double minElevation{ 80 };
      //   if (elevation < minElevation) {
      //      double offset{ minElevation - elevation };
      //      glm::dvec3 normal{ planet->mapNormal(rigidBody->m_pos, 3) };
      //      double projVel{ glm::dot(normal, rigidBody->m_vel * 0.0) };
      //      double power{ (offset - projVel) * 0.0004 };
      //      //power *= glm::max(up.z, 0.);
      //      power *= rWingHealth * lWingHealth;
      //      spaceShip->m_rigidBody->m_vel += normal * power;
      //   }
      //}
      //for (size_t jj = 0; jj < m_planets.size(); jj++) {
      //   Planet* planet{ m_planets[jj] };
      //
      //   double distance{ 10.0 };
      //   std::vector<glm::dvec3> positions = {
      //      spaceShip->m_rigidBody->m_pos + (forward * distance),
      //      spaceShip->m_rigidBody->m_pos + (right * -distance),
      //      spaceShip->m_rigidBody->m_pos + (right * distance),
      //      spaceShip->m_rigidBody->m_pos + (forward * -distance)
      //   };
      //
      //   double minElevation{ 60 };
      //
      //   for (const glm::dvec3& pos : positions) {
      //      double elevation{ pos.z - planet->map(pos, 3).z };
      //
      //      if (elevation < minElevation) {
      //         double offset{ minElevation - elevation };
      //         glm::dvec3 normal{ planet->mapNormal(pos, 3) };
      //         glm::dvec3 radius{ pos - spaceShip->m_rigidBody->m_pos };
      //
      //         double projVel{ glm::dot(normal, rigidBody->m_vel * 0.0) };
      //         double power{ (offset - projVel) * 0.0004 };
      //
      //         power *= rWingHealth * lWingHealth;
      //         spaceShip->m_rigidBody->m_vel += normal * power;
      //         spaceShip->applyTorque(glm::cross(radius, normal) * power * 0.008);
      //      }
      //   }
      //}
      // TEST END.

      //torque *= rWingHealth * lWingHealth;

      spaceShip->applyTorque(torque);
   }

   for (size_t ii = 0; ii < m_movingParticleEffects.size(); ii++) {
      MovingParticleEffect* mpe{ m_movingParticleEffects[ii] };
      mpe->m_rigidBody->m_vel *= 1. - mpe->airResistanceFactor;
   }

   // Debree physics.
   for (size_t ii = 0; ii < m_debrees.size(); ii++) {
      Debree* debree{ m_debrees[ii] };
      debree->m_rigidBody->m_vel -= 0.0005 * debree->m_rigidBody->m_vel * glm::length(debree->m_rigidBody->m_vel);
      glm::dvec3 randomAxis{ Hash::pcgUnit3(m_graphicsEngine.m_frameNum + ii) - 0.5 };
      debree->m_rigidBody->m_rotationAxis = HelperFunctions::rotateAroundVector(
         debree->m_rigidBody->m_rotationAxis, randomAxis, PhysicsUnits::radiansPerSecond(1.2)
      );
   }

   // Physics engine physics.
   m_phyisicsEngine.run();

   //
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      m_spaceShips[ii]->updateCollisionBoxes();
   }
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      m_projectiles[ii]->updateCollisionBoxes();
   }
}

void GameEngine::handleGraphics(uint64_t frameNum) {
   // Graphics.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      m_spaceShips[ii]->updateMeshPositions(frameNum, m_graphicsEngine.m_camPos, m_graphicsEngine.m_fieldOfView);
   }
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      m_projectiles[ii]->updateMeshPositions(m_graphicsEngine.m_camPos, m_graphicsEngine.m_camVel, frameNum);
   }
   for (size_t ii = 0; ii < m_movingParticleEffects.size(); ii++) {
      m_movingParticleEffects[ii]->updateMeshPositions();
   }
   for (size_t ii = 0; ii < m_debrees.size(); ii++) {
      m_debrees[ii]->updateMeshPositions();
   }
}

void GameEngine::handleSoundEffects() {
   // Wind effect.
   if (Hash::pcg(m_graphicsEngine.m_frameNum) % (144 / 4) == 0) {
      double speed{ glm::length(m_graphicsEngine.m_camVel) };
      double pitch{ 1. + speed * 0.1 };
      pitch *= Hash::pcgUnit(m_graphicsEngine.m_frameNum) * 0.5 + 0.75;
      double gain{ .0 + speed * 0.1 };
      m_soundPlayer.playSound(m_windSound, pitch, gain * m_globalVolumeMultiplier);
   }
   // Thruster effect.
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      if ((int)m_graphicsEngine.m_frameNum % (144 / 4) != 0) {
         continue;
      }
      SpaceShip* sp{ m_spaceShips[ii] };
      double pitch{ 4.0 };
      pitch *= Hash::pcgUnit(m_graphicsEngine.m_frameNum + ii) * 0.5 + 0.1;
      pitch *= sp->getEnginePitchScale();
      double gain{ 12.0 * sp->m_thrustMultiplier * sp->m_thrustMultiplier };
      playSoundAtLocation(
         sp->m_rigidBody->m_pos, m_rocketSound, pitch, gain, 0.5
      );
      //bool tt{ false };
      //if (tt) {
      //   break;
      //}
   }
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      if ((int)(m_graphicsEngine.m_frameNum + ii) % (144 / 8) != 0) {
         continue;
      }
      Projectile* pp{ m_projectiles[ii] };
      double gain{10.};
      double pitch{2. + Hash::pcgUnit(m_graphicsEngine.m_frameNum + ii) * 2.0 };
      playSoundAtLocation(
         pp->m_rigidBody->m_pos, m_flaunchSound, pitch, gain, 0.1
      );
   }
   //if ((int)m_graphicsEngine.m_frameNum % (144 / 4) == 0) {
   //   double speedSqr{ glm::length2(m_spaceShips[0]->m_rigidBody->m_vel) };
   //   double pitch{ 3.0 + speedSqr * 0.001 };
   //   pitch *= Hash::pcgUnit(m_graphicsEngine.m_frameNum) * 0.5 + 0.1;
   //   double gain{ 10.0 };
   //   playSoundAtLocation(
   //      m_spaceShips[0]->m_rigidBody->m_pos, m_rocketSound, pitch, gain
   //   );
   //}
   //if ((int)frameNum % (144 / 4) == 0 && m_playerVehicle != nullptr && m_keyBoardHandler->m_lShift.m_isDown) {
   //   double speedSqr{ glm::length2(m_playerVehicle->m_rigidBody->m_vel) };
   //   double pitch{ 3.0 + speedSqr * 0.001 };
   //   pitch *= Hash::pcgUnit(frameNum) * 0.5 + 0.1;
   //   double gain{ 10.0 };
   //   playSoundAtLocation(
   //      m_playerVehicle->m_rigidBody->m_pos, m_gameEngine->m_rocketSound, pitch, gain
   //   );
   //}
}

void GameEngine::preRenderCallback(uint64_t frameNum) {
   //
   for (size_t ii = 0; ii < m_gameEngineCallbacks.size(); ii++) {
      m_gameEngineCallbacks[ii]->preRenderCallback(frameNum);
   }
   //
   m_frameLog.emptyLog();
   //
   handleStatistics(frameNum);
   handleGameLogic(frameNum);
   handlePhysics(frameNum);
   handleGraphics(frameNum);
   handleSoundEffects();

   //std::cout << m_graphicsEngine.m_camPos.x << std::endl;
   //std::cout << m_graphicsEngine.m_camPos.y << std::endl;
   //std::cout << m_graphicsEngine.m_camPos.z << std::endl;

}

void GameEngine::framebufferSizeCallback(int /*width*/, int /*height*/) {
   
}

SpaceShip* GameEngine::createSpaceShip(glm::dvec3 position) {
   SpaceShip* spaceShip = new SpaceShip{};
   m_spaceShips.push_back(spaceShip);
   for (size_t ii = 0; ii < m_spaceShipModels.size(); ii++) {
      if (ii==3) {
         //continue;
      }
      Mesh* meshHigh = m_graphicsEngine.createMesh(m_spaceShipModels[ii]);
      Mesh* meshMedium = m_graphicsEngine.createMesh(m_spaceShipModelsBroken[ii]);
      meshMedium->m_isVisible = false;
      //meshHigh->m_isVisible = false;
      double scale{ spaceShip->m_scale };
      meshHigh->m_scale = { scale , scale, scale };
      meshMedium->m_scale = { scale , scale, scale };
      SpaceShipPart spaceShipPart{};
      spaceShipPart.m_meshCurrent = meshHigh;
      //spaceShipPart.m_meshCurrent = meshMedium;
      spaceShipPart.m_meshHealthMax = meshHigh;
      spaceShipPart.m_meshHealthMedium = meshMedium;
      spaceShipPart.m_offset = m_spaceShipModelOffsets[ii] * scale;
      //spaceShipPart.m_health *= 5.;
      if (ii == 0) {
         spaceShipPart.m_health *= 3.;
         spaceShipPart.m_maxHealth *= 3.;
      } else if (ii == 2 || ii == 3) {
         //spaceShipPart.m_health /= 2.;
         //spaceShipPart.m_maxHealth /= 2.;
      } else if (ii == 1 || ii == 4) {
         spaceShipPart.m_health *= 1.25;
         spaceShipPart.m_maxHealth *= 1.25;
      }
      spaceShip->m_spaceShipParts.push_back(spaceShipPart);
      spaceShip->updateCenterOfMass();
   }

   //
   spaceShip->m_plasmaL = m_graphicsEngine.createTransparentMesh(m_spaceShipEnginePlasmaModel);
   spaceShip->m_plasmaR = m_graphicsEngine.createTransparentMesh(m_spaceShipEnginePlasmaModel);
   double ss{ spaceShip->m_plasmaScale };
   spaceShip->m_plasmaR->m_scale = { ss,ss,ss };
   spaceShip->m_plasmaL->m_scale = { ss,ss,ss };
   spaceShip->m_plasmaR->m_doCulling = false;
   spaceShip->m_plasmaL->m_doCulling = false;

   //
   spaceShip->m_plasmaEffectR = m_graphicsEngine.createParticleEffectInstance(m_plasmaParticleEffect);
   spaceShip->m_plasmaEffectL = m_graphicsEngine.createParticleEffectInstance(m_plasmaParticleEffect);
   ss = 3.2768;
   spaceShip->m_plasmaEffectR->m_scale = glm::dvec3{ ss,ss * 2.,ss };
   spaceShip->m_plasmaEffectL->m_scale = glm::dvec3{ ss,ss * 2.,ss };
   spaceShip->m_plasmaEffectR->m_numParticles = 5;
   spaceShip->m_plasmaEffectL->m_numParticles = 5;

   //
   RigidBody* rigidBody = m_phyisicsEngine.createRigidBody(position);
   spaceShip->m_rigidBody = rigidBody;

   // Create collisionBoxes.
   spaceShip->m_size = 19.6608;
   std::vector<glm::dvec3> positions{
      {0, 1.11011,-0.018622}, {0,-0.537917,-0.018622},
      {1.49718, -0.015757,0.084645}, {4.70226,-0.216871,-0.315428},
      {3.08041, 0.224195,0.043719}, {3.08041,-1.77401,0.043719},
      {-3.08041, 0.224195,0.043719}, {-3.08041,-1.77401,0.043719},
      {-1.49718, -0.015757,0.084645}, {-4.70226,-0.216871,-0.315428},
   };
   std::vector<double> radii{
      0.959647,
      0.67,
      0.882028,
      0.882028,
      0.67,
   };
   std::vector<glm::dvec3> colors{
      { 0,0,1 },
      { 0,1,0 },
      { 0,1,1 },
      { 1,0,0 },
      { 1,0,1 },
   };
   for (size_t ii = 0; ii < radii.size(); ii++) {
      //Mesh* mesh0{ m_graphicsEngine.createMesh(m_sphereModel) };
      //Mesh* mesh1{ m_graphicsEngine.createMesh(m_sphereModel) };
      //mesh0->m_color = colors[ii];
      //mesh1->m_color = colors[ii];
      //mesh0->m_useTexture = false;
      //mesh1->m_useTexture = false;
      Mesh* mesh0{ nullptr };
      Mesh* mesh1{nullptr};
      SpaceShipPart::CollisionSphere cs{
         mesh0, mesh1,
         positions[ii*2] * 3.2768 -spaceShip->m_centerOfMass,
         positions[ii*2 + 1] * 3.2768 - spaceShip->m_centerOfMass, radii[ii] * 3.2768
      };
      spaceShip->m_spaceShipParts[ii].m_collisionSpheres.push_back(cs);
   }

   return spaceShip;
}

void GameEngine::removeSpaceShip(SpaceShip* spaceShip) {
   for (size_t ii = 0; ii < m_spaceShips.size(); ii++) {
      if (m_spaceShips[ii] == spaceShip) {
         if (ii != m_spaceShips.size() - 1) {
            m_spaceShips[ii] = m_spaceShips[m_spaceShips.size() - 1];
         }
         m_spaceShips.pop_back();
         // Remove sub components.
         for (size_t jj = 0; jj < spaceShip->m_spaceShipParts.size(); jj++) {
            m_graphicsEngine.removeMesh(spaceShip->m_spaceShipParts[jj].m_meshHealthMax);
            m_graphicsEngine.removeMesh(spaceShip->m_spaceShipParts[jj].m_meshHealthMedium);
         }
         m_graphicsEngine.removeTransparentMesh(spaceShip->m_plasmaR);
         m_graphicsEngine.removeTransparentMesh(spaceShip->m_plasmaL);
         m_graphicsEngine.removeParticleEffectInstance(spaceShip->m_plasmaEffectR);
         m_graphicsEngine.removeParticleEffectInstance(spaceShip->m_plasmaEffectL);
         m_phyisicsEngine.removeRigidBody(spaceShip->m_rigidBody);

         //
         m_frameLog.m_removedSpaceShipIds.push_back(spaceShip->m_id);
         delete spaceShip;
         return;
      }
   }
   std::cout << "ERROR: Tried removing space ship that does not exist." << std::endl;
   throw "ERROR: Tried removing space ship that does not exist.";
}

Projectile* GameEngine::createProjectile(glm::dvec3 position) {
   Projectile* projectile{ new Projectile(m_graphicsEngine.m_frameNum)};
   m_projectiles.push_back(projectile);
   RigidBody* rigidBody{ m_phyisicsEngine.createRigidBody(position) };
   projectile->m_rigidBody = rigidBody;
   Mesh* mesh{ m_graphicsEngine.createTransparentMesh(m_projectileModel) };
   //double ss{ 0.0002 };
   //mesh->m_scale = {ss, ss, ss};
   mesh->m_doCulling = false;
   projectile->m_mesh = mesh;
   //projectile->m_size = 0.0001;
   projectile->updateCollisionBoxes();
   projectile->updateCollisionBoxes(); // Call it twice to update the prev coordinates.

   return projectile;
}

void GameEngine::removeProjectile(Projectile* projectile) {
   for (size_t ii = 0; ii < m_projectiles.size(); ii++) {
      if (m_projectiles[ii] == projectile) {
         if (ii == m_projectiles.size() - 1) {
            m_projectiles.pop_back();
         } else {
            m_projectiles[ii] = m_projectiles[m_projectiles.size() - 1];
            m_projectiles.pop_back();
         }
         m_graphicsEngine.removeTransparentMesh(projectile->m_mesh);
         m_phyisicsEngine.removeRigidBody(projectile->m_rigidBody);
         
         delete projectile;
         return;
      }
   }
   std::cout << "ERROR: Tried removing projectile that does not exist." << std::endl;
   throw "ERROR: Tried removing projectile that does not exist.";
}

MovingParticleEffect* GameEngine::createMovingParticleEffect(ParticleEffect* particleEffect, glm::dvec3 position) {
   MovingParticleEffect* movingParticleEffect{ new MovingParticleEffect(m_graphicsEngine.m_frameNum) };
   m_movingParticleEffects.push_back(movingParticleEffect);
   RigidBody* rigidBody{ m_phyisicsEngine.createRigidBody(position) };
   movingParticleEffect->m_rigidBody = rigidBody;
   ParticleEffectInstance* particleEffectInstance{ m_graphicsEngine.createParticleEffectInstance(particleEffect) };
   movingParticleEffect->m_particleEffectInstance = particleEffectInstance;
   particleEffectInstance->m_numParticles = 4;

   return movingParticleEffect;
}

void GameEngine::removeMovingParticleEffect(MovingParticleEffect* movingParticleEffect) {
   for (size_t ii = 0; ii < m_movingParticleEffects.size(); ii++) {
      if (m_movingParticleEffects[ii] == movingParticleEffect) {
         if (ii != m_movingParticleEffects.size() - 1) {
            m_movingParticleEffects[ii] = m_movingParticleEffects[m_movingParticleEffects.size() - 1];
         }
         m_movingParticleEffects.pop_back();
         m_graphicsEngine.removeParticleEffectInstance(movingParticleEffect->m_particleEffectInstance);
         m_phyisicsEngine.removeRigidBody(movingParticleEffect->m_rigidBody);

         delete movingParticleEffect;
         return;
      }
   }
   std::cout << "ERROR: Tried removing movingParticleEffect that does not exist." << std::endl;
   throw "ERROR: Tried removing movingParticleEffect that does not exist.";
}

Planet* GameEngine::createPlanet() {
   Planet* planet{ new Planet() };
   Surface* surface = m_graphicsEngine.createSurface();
   planet->setSurface(surface);
   m_planets.push_back(planet);

   return planet;
}

void GameEngine::removePlanet(Planet* planet) {
   for (size_t ii = 0; ii < m_planets.size(); ii++) {
      if (m_planets[ii] == planet) {
         if (ii == m_planets.size() - 1) {
            m_planets.pop_back();
         } else {
            m_planets[ii] = m_planets[m_planets.size() - 1];
            m_planets.pop_back();
         }
         m_graphicsEngine.removeSurface(planet->m_surface);

         delete planet;
         return;
      }
   }
   std::cout << "ERROR: Tried removing planet that does not exist." << std::endl;
   throw "ERROR: Tried removing planet that does not exist.";
}

Debree* GameEngine::createDebree(glm::dvec3 position, Model* model) {
   Debree* debree{ new Debree(m_graphicsEngine.m_frameNum) };
   RigidBody* rigidBody{ m_phyisicsEngine.createRigidBody(position) };
   debree->m_rigidBody = rigidBody;
   Mesh* mesh{ m_graphicsEngine.createMesh(model) };
   debree->m_mesh = mesh;
   m_debrees.push_back(debree);

   return debree;
}

void GameEngine::removeDebree(Debree* debree) {
   for (size_t ii = 0; ii < m_debrees.size(); ii++) {
      if (m_debrees[ii] == debree) {
         if (ii == m_debrees.size() - 1) {
            m_debrees.pop_back();
         } else {
            m_debrees[ii] = m_debrees[m_debrees.size() - 1];
            m_debrees.pop_back();
         }
         m_graphicsEngine.removeMesh(debree->m_mesh);
         m_phyisicsEngine.removeRigidBody(debree->m_rigidBody);

         delete debree;
         return;
      }
   }
   std::cout << "ERROR: Tried removing debree that does not exist." << std::endl;
   throw "ERROR: Tried removing debree that does not exist.";
}
