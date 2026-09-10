

#include "GameLogic.h"

#include <iostream>

#include "Hash.h"
#include "GlobalConstants.h"
#include "PhysicsUnits.h"


GameLogic::GameLogic() {
   m_gameEngine = new GameEngine();
   m_gameEngine->m_gameEngineCallbacks.push_back(this);

   m_camSpeed = PhysicsUnits::metersPerSecond(1.2);

   m_keyBoardHandler = new KeyboardHandler(m_gameEngine->m_graphicsEngine.m_window);

   //
   m_gameEngine->m_graphicsEngine.m_mouseHandler->setMouseLock(true);

   //
   m_planet = m_gameEngine->createPlanet();

   //
   m_gameEngine->m_graphicsEngine.m_camPos = glm::dvec3{ 0., 0., 0.1 };
   
   //
   createPlayerSpaceShip();
   
   SpaceShip* tt{ m_gameEngine->createSpaceShip({ 0,328.,4000. }) };
   tt->updateMeshPositions(
      m_gameEngine->m_graphicsEngine.m_frameNum,
      m_gameEngine->m_graphicsEngine.m_camPos,
      m_gameEngine->m_graphicsEngine.m_fieldOfView
   );
   //m_gameEngine->removeSpaceShip(tt);

   //
   m_enemyMarkerModel = m_gameEngine->m_graphicsEngine.createModel(
      "../media/models/crossHair/cross_hair.obj",
      "../media/models/crossHair/cross_hair.png"
   );
   m_enemyMarkerMesh = m_gameEngine->m_graphicsEngine.createMesh(m_enemyMarkerModel);
   m_enemyMarkerModel->m_selfIllumination = true;
    
   //
   m_crossHair = m_gameEngine->m_graphicsEngine.createModel(
      "../media/models/crossHair/cross_hair.obj",
      "../media/models/crossHair/cross_hair_5.png"
   );
   m_crossHair->m_selfIllumination = true;
   //double factor{ 1. };
   for (size_t ii = 0; ii < 3; ii++) {
      Mesh* crossHairMesh{ m_gameEngine->m_graphicsEngine.createTransparentMesh(m_crossHair) };
      //double ss{ factor * 0.0001 };
      //crossHairMesh->m_scale = { ss,ss,ss };
      crossHairMesh->m_doCulling = false;
      m_crossHairsMeshes.push_back(crossHairMesh);
      //factor *= 2.;
   }

   //
   //int density{ 20 };
   //for (size_t yy = 0; yy < density; yy++) {
   //   std::vector<Mesh*> row{};
   //   for (size_t xx = 0; xx < density; xx++) {
   //      Mesh* mesh = m_gameEngine->m_graphicsEngine.createMesh(m_crossHair);
   //      double ss{ 1. };
   //      mesh->m_scale = glm::dvec3{ ss,ss,ss };
   //      mesh->m_doCulling = false;
   //      mesh->m_orientation = glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 });
   //      mesh->m_useTexture = false;
   //      mesh->m_position.z += 0.00000;
   //      mesh->m_color = glm::dvec3{1,0,0};
   //
   //      row.push_back(mesh);
   //   }
   //   m_debugGrid.push_back(row);
   //}

   //m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_scifiSound, 1., 100.);

   m_debugTargetMesh = m_gameEngine->m_graphicsEngine.createMesh(m_gameEngine->m_sphereModel);
   double ss{ 0.01 };
   m_debugTargetMesh->m_scale = { ss,ss,ss };
   m_debugTargetMesh->m_isVisible = false;
   

   //
   m_gameEngine->startRenderLoop();
}

GameLogic::~GameLogic() {
   delete m_gameEngine;
   delete m_keyBoardHandler;
}

void GameLogic::createPlayerSpaceShip() {
   m_playerVehicle = m_gameEngine->createSpaceShip({ 0,0,4000 });
   m_playerVehicle->m_rigidBody->m_pos.x = 0.0001;
   //m_playerVehicle->m_rigidBody->m_vel.z += 0.001;
   //m_playerVehicle->m_rigidBody->m_vel.x += 0.0001;
   //m_playerVehicle->m_rigidBody->m_vel.y += -0.0001;
   m_playerVehicleId = m_playerVehicle->m_id;
   m_playerVehicle->m_shootingSpeed *= 4.;
   m_playerVehicle->m_projectileDamage *= 1.0;
   //m_playerVehicle->m_shotIntervalTime /= 1;
   m_playerVehicle->m_shootSpread *= 0.5;
   m_playerVehicle->m_doAimAssist = true;
}

void GameLogic::handleGameEvents(uint64_t frameNum) {
   //m_gameEngine->m_frameLog;
   for (size_t ii = 0; ii < m_gameEngine->m_frameLog.m_removedSpaceShipIds.size(); ii++) {
      int id{ m_gameEngine->m_frameLog.m_removedSpaceShipIds[ii] };
      if (id == m_playerVehicleId) {
         m_playerVehicle = nullptr;
         //GraphicsEngine* ge{ &m_gameEngine->m_graphicsEngine };
         m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_lossSound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
         //glm::dvec3 targetPos{ ge->m_camPos };
         //ge->m_camPos += glm::dvec3{0,0,1} * 0.01;
         //glm::dquat newOrientation{ glm::quatLookAt(
         //   targetPos - ge->m_camPos,
         //   ge->m_camOri * glm::dvec3{0,0,1}
         //) };
         //newOrientation = newOrientation * glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 });
         //ge->m_camOri = newOrientation;
         //ge->m_camOri = glm::slerp(ge->m_camOri, glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 }), 0.25);
      }
      m_totalSpaceShips--;
      std::cout << "Num space ships left:" << std::endl << m_totalSpaceShips << std::endl << std::endl;
      //if (m_totalSpaceShips == m_totalSpaceShipsInit * 1 / 6) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_victorySound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_totalSpaceShips == m_totalSpaceShipsInit * 2 / 6) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_4Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_totalSpaceShips == m_totalSpaceShipsInit * 3 / 6) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_3Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_totalSpaceShips == m_totalSpaceShipsInit * 4 / 6) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_2Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_totalSpaceShips == m_totalSpaceShipsInit * 5 / 6) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_1Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //}
   }
   // Count kills.
   if (m_playerVehicle != nullptr && m_numEnemiesKilledLast < m_playerVehicle->m_numKills) {
      //m_numEnemiesKilledLast = m_playerVehicle->m_numKills;
      //if (m_playerVehicle->m_numKills == 5) {
      //   // WIN!
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_victorySound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //   m_winTime = (int)frameNum;
      //   for (size_t ii = 0; ii < m_gameEngine->m_spaceShips.size(); ii++) {
      //      SpaceShip* ss{ m_gameEngine->m_spaceShips[ii] };
      //      if (ss->m_id != m_playerVehicleId) {
      //         //m_gameEngine->damageSpaceShip(0, ss, 999.);
      //      }
      //   }
      //} else if (m_playerVehicle->m_numKills == 4) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_4Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_playerVehicle->m_numKills == 3) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_3Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_playerVehicle->m_numKills == 2) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_2Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //} else if (m_playerVehicle->m_numKills == 1) {
      //   m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_1Sound, 1., 1. * m_gameEngine->m_globalVolumeMultiplier);
      //}
   }

   if (m_playerVehicle != nullptr) {
      if (m_keyBoardHandler->m_lShift.m_isDown) {
         //double thrust{ 0.0000004 };
         //m_playerVehicle->m_rigidBody->m_vel += m_playerVehicle->m_rigidBody->m_ori * glm::dvec3{ 0,1,0 } * thrust;
         m_playerVehicle->m_thrustMultiplier = 2.;
         //double current{ m_playerVehicle->m_plasmaEffectR->m_scale.y };
         //double target{ m_playerVehicle->m_plasmaEffectL->m_scale.x * 4. };
         //double newL{ glm::mix(current, target, 0.01) };
         //m_playerVehicle->m_plasmaEffectR->m_scale.y = newL;
         //m_playerVehicle->m_plasmaEffectL->m_scale.y = newL;
      } else {
         m_playerVehicle->m_thrustMultiplier = 1.;
         //double current{ m_playerVehicle->m_plasmaEffectR->m_scale.y };
         //double target{ m_playerVehicle->m_plasmaEffectL->m_scale.x };
         //double newL{ glm::mix(current, target, 0.01) };
         //m_playerVehicle->m_plasmaEffectR->m_scale.y = newL;
         //m_playerVehicle->m_plasmaEffectL->m_scale.y = newL;
      }
      if (m_keyBoardHandler->m_lCtrl.m_isDown) {
         m_playerVehicle->m_thrustMultiplier = -0.5;
         m_playerVehicle->m_pitchUpMax = m_playerVehicle->m_pitchUpMaxBase * 1.25;
      } else {
         m_playerVehicle->m_pitchUpMax = m_playerVehicle->m_pitchUpMaxBase;
      }

      // Zoom.
      double zoomScale{ 2. };
      MouseHandler* mh{ m_gameEngine->m_graphicsEngine.m_mouseHandler };
      if (mh->m_rightMouseDown && mh->m_timeRightDown == 0) {
         m_gameEngine->m_graphicsEngine.m_fieldOfView /= zoomScale;
      } else if (!mh->m_rightMouseDown && mh->m_timeRightUp == 0) {
         m_gameEngine->m_graphicsEngine.m_fieldOfView *= zoomScale;
      }
   }

   // Spawn enemies.
   if (m_gameState == PLAY) {
      double prob{ Hash::pcgUnit(frameNum) };
      if (
         prob <= PhysicsUnits::perSecond(0.6) && m_playerVehicle != nullptr &&
         m_gameEngine->m_spaceShips.size() < m_maxSpaceships &&
         m_totalSpaceShips > 0
         ) {
         glm::dvec3 spawnPos{ m_playerVehicle->m_rigidBody->m_pos };
         glm::dvec3 forward{ m_playerVehicle->m_rigidBody->m_ori * glm::dvec3{0,1.,0} };
         glm::dvec3 shift{ forward };
         shift.z = 0.;
         double sL{ glm::length(shift) };
         double spawnDistance{ 10000. };
         if (sL > 0.) {
            spawnPos += shift / sL * spawnDistance;
         } else {
            spawnPos += glm::dvec3{ 0, 0, spawnDistance };
         }
         spawnPos.z = m_playerVehicle->m_rigidBody->m_pos.z + 10000.;
         //spawnPos = glm::mix(m_playerVehicle->m_rigidBody->m_pos, spawnPos, 0.2);
         SpaceShip* tt{ m_gameEngine->createSpaceShip(spawnPos) };
         tt;
         tt->m_rigidBody->m_vel +=
            (Hash::pcgUnit3(frameNum) * 0.5 - 0.5) * PhysicsUnits::metersPerSecond(0.12);
      }
   }

   // Heal player.
   //double minHealth{ 1. };
   if (m_playerVehicle != nullptr) {
      for (size_t ii = 0; ii < m_playerVehicle->m_spaceShipParts.size(); ii++) {
         SpaceShipPart* sp{ &m_playerVehicle->m_spaceShipParts[ii] };
         if (sp->m_health <= 0.) {
            continue;
         }
         double prevHealth{ sp->m_health / sp->m_maxHealth };
         double healRate{ 0.0004 };
         if (prevHealth < 0.5) {
            healRate *= 0.1;
         }
         sp->m_health = glm::mix(
            sp->m_health, sp->m_maxHealth, PhysicsUnits::blendHalfLife(28.87824431379865)
         );
         double nextHealth{ sp->m_health / sp->m_maxHealth };
         if (prevHealth < 0.5 && nextHealth >= 0.5) {
            sp->m_meshCurrent->m_isVisible = false;
            sp->m_meshCurrent = sp->m_meshHealthMax;
            sp->m_meshCurrent->m_isVisible = true;
         }
         //if (sp->m_health > 0.5 * sp->m_maxHealth) {
         //   sp->m_health = glm::mix(sp->m_health, sp->m_maxHealth, 0.0002);
         //}
         //double fraction{ sp->m_health / sp->m_maxHealth };
         //if (fraction < minHealth) {
         //   minHealth = fraction;
         //}
      }
   }
   //if (frameNum % 100 == 0) {
   //   std::cout << "minHealth: " << minHealth << std::endl;
   //}
}

void GameLogic::handleSoundEffects() {

}

void GameLogic::joystickControlSpaceShip(SpaceShip* spaceShip, glm::dvec2 joystick) {
   RigidBody* rigidBody{ spaceShip->m_rigidBody };

   glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
   glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
   glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };

   // Control space ship.
   glm::dvec3 torqueLocal{ 0,0,0 };
   // Mouse.
   double rotationFactorMouse{ PhysicsUnits::radiansPerSecondSquared(14.4) };
   torqueLocal.x = joystick.y * rotationFactorMouse;
   torqueLocal.z = -joystick.x * rotationFactorMouse * 5.0;
   torqueLocal.y = joystick.x * rotationFactorMouse * 5.;

   // Apply.
   //rigidBody->applyTorque(torque);
   spaceShip->applyTorqueCapped(torqueLocal);
   //glm::dvec3 angVel{ rigidBody->m_rotationAxis };
   //double axisLength{ glm::length(angVel) };
   //if (axisLength > 0.) {
   //   angVel = angVel / axisLength * rigidBody->m_rotationVel;
   //   angVel += torque;
   //} else {
   //   angVel = torque;
   //}
   //rigidBody->m_rotationAxis = angVel;
   //rigidBody->m_rotationVel = glm::length(angVel);
}

void GameLogic::targetPosition(SpaceShip* spaceShip, glm::dvec3 position) {
   RigidBody* rigidBody{ spaceShip->m_rigidBody };

   glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
   glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
   glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };

   glm::dvec2 joystick{ 0 };
   glm::dvec3 dir{ position - rigidBody->m_pos };
   double dirL{ glm::length(dir) };
   if (dirL == 0.) {
      return;
   }
   dir = dir / dirL;
   glm::dvec3 angVel{ spaceShip->m_rigidBody->getAngularVel() };
   //joystick.y = glm::dot(up, dir);
   ////joystick.y = abs(joystick.y);
   //if (joystick.y < -0.2) {
   //   joystick.y = 0.;
   //}
   //joystick.y = joystick.y * 4. - glm::dot(angVel, right) * 0.;
   //joystick.x = glm::dot(right, dir) * 4. - glm::dot(angVel, forward) * 0.;
   //joystick.x += right.z * 0.01;
   double rr{ glm::dot(dir, right) };
   double ff{ glm::dot(dir, forward) };
   joystick.x = glm::atan(rr, ff);
   double uu{ glm::dot(dir, up) };
   joystick.y = glm::atan(uu, ff);
   
   if (ff > 0.5) {
      //joystick.y *= glm::pow(1. - glm::abs(joystick.x) / glm::pi<double>(), 2.);
   }

   if (joystick.x > 0.) {
      joystick.x = glm::sqrt(joystick.x);
   } else {
      joystick.x = -glm::sqrt(-joystick.x);
   }
   joystick.x += -glm::dot(angVel, forward) * PhysicsUnits::seconds(2. / 15.);

   if (joystick.y > 0.) {
      joystick.y = glm::sqrt(joystick.y);
   } else {
      joystick.y = -glm::sqrt(-joystick.y);
   }
   joystick.y += -glm::dot(angVel, right) * PhysicsUnits::seconds(2. / 15.);

   joystick *= 4096.*4096.;

   joystickControlSpaceShip(spaceShip, joystick);
}

void GameLogic::handleAI(uint64_t frameNum) {
   //return;
   if (m_gameState == PLAY && m_playerVehicle != nullptr) {
      for (size_t ii = 0; ii < m_gameEngine->m_spaceShips.size(); ii++) {
         SpaceShip* sp{ m_gameEngine->m_spaceShips[ii] };
         
         glm::dvec3 up{ sp->m_rigidBody->m_ori * glm::dvec3{0,0,1} };
         glm::dvec3 forward{ sp->m_rigidBody->m_ori * glm::dvec3{0,1,0} };

         SpaceShip* tSpaceShip{ m_playerVehicle };
         if (sp == m_playerVehicle) {
//#define DEBUG_AI
#ifdef DEBUG_AI
            if (m_gameEngine->m_spaceShips.size() > 1) {
               tSpaceShip = m_gameEngine->m_spaceShips[1];
            } else {
               continue;
            }
#else
            continue;
#endif
         }
         double prob{ Hash::pcgUnit(ii, frameNum) };
         if (sp->m_aiState == SpaceShip::CHACE) {
            if (prob <= PhysicsUnits::perSecond(1. / 12.)) {
               sp->m_aiState = SpaceShip::ESCAPE;
            } else if (prob <= PhysicsUnits::perSecond(5. / 12.)) {
               sp->m_aiState = SpaceShip::SHOOT;
            }
         } else if (sp->m_aiState == SpaceShip::SHOOT) {
#ifdef DEBUG_AI
            if (ii == 0) {
               goto breakIF;
            } else {
               sp->m_aiState = SpaceShip::CHACE;
            }
#endif // DEBUG_AI
            if (prob <= PhysicsUnits::perSecond(1. / 12.)) {
               sp->m_aiState = SpaceShip::ESCAPE;
            } else if (prob <= PhysicsUnits::perSecond(5. / 6.)) {
               sp->m_aiState = SpaceShip::CHACE;
            }
         } else if (sp->m_aiState == SpaceShip::ESCAPE) {
            double distance{ glm::length(sp->m_rigidBody->m_pos - tSpaceShip->m_rigidBody->m_pos) };
            if (prob <= PhysicsUnits::perSecond(1. / 12.) || distance > 20000.) {
               sp->m_aiState = SpaceShip::CHACE;
            }
         }
#ifdef DEBUG_AI
         breakIF:;
#endif // DEBUG_AI
         glm::dvec3 targetPos{}; bool canHit{}; double time{};
         //double reduceFactor{ 0.9 };// Makes spaceShips shoot a bit ahead due to enemy ship...
         // ...usually accelerating.
         double scaleFactor{
            1. + Hash::pcgUnit(frameNum / (uint64_t)PhysicsUnits::ticks(2.4)) * 0.2
         };
         GameLogic::trajectoryInterceptionWithGravity(
            sp, tSpaceShip, scaleFactor, &targetPos, &canHit, &time
         );
         //GameLogic::trajectoryInterception(
         //   sp->m_rigidBody->m_vel, sp->m_rigidBody->m_pos,
         //   tSpaceShip->m_rigidBody->m_vel, tSpaceShip->m_rigidBody->m_pos, sp->m_shootingSpeed * reduceFactor,
         //   &targetPos, &canHit, &time
         //);
         //double upDistance{ m_gameEngine->m_phyisicsEngine.m_gravity * time * time * 0.5 };
         if (!canHit) {
            targetPos = tSpaceShip->m_rigidBody->m_pos;
         }
         //targetPos = { 2948.58, 27468.3, 1378.36 };
         if (sp->m_aiState == SpaceShip::ESCAPE) {
            glm::dvec3 dir{ tSpaceShip->m_rigidBody->m_pos - sp->m_rigidBody->m_pos };
            double dirL{ glm::length(dir) };
            if (dirL > 0.) {
               targetPos = sp->m_rigidBody->m_pos - dir / dirL + glm::dvec3{ 0,0,-1 };
            } else {
               targetPos = sp->m_rigidBody->m_pos + glm::dvec3{ 0,0,1 };
            }
            targetPos.z = tSpaceShip->m_rigidBody->m_pos.z;
         }

         double minHeight{ 999999999. };
         int resolution{ 10 };
         double maxDistance{ PhysicsUnits::seconds(20. / 3.) * (0.5 + glm::abs(forward.z) * 0.5)};
         glm::dvec3 furthestPoint{};
         double skipHeight{ maxDistance * glm::length(sp->m_rigidBody->m_vel) };
         for (size_t jj = 0; jj < resolution+1; jj++) {
            glm::dvec3 pp{
               sp->m_rigidBody->m_pos +
               sp->m_rigidBody->m_vel * (double)jj / (double)resolution * maxDistance
            };
            furthestPoint = pp;
            double height{ pp.z - m_planet->map(pp, 3).z };
            if (height > skipHeight) {
               break;
            }
            if (minHeight > height) {
               minHeight = height;
            }
         }
         bool doAvoid{false};
         double limit{ 100. };
         if (up.z < 0.) {
            limit *= 4.;
         }
         if (minHeight < limit) {
            targetPos = sp->m_rigidBody->m_pos + glm::dvec3{ 0,0,1 } + up * 0.;
            doAvoid = true;
         }
         //if (sp == m_playerVehicle) {
         //   if (doAvoid) {
         //      std::cout << "!!" << std::endl;
         //   } else {
         //      std::cout << ":)" << std::endl;
         //   }
         //}
         //targetPos = sp->m_rigidBody->m_pos + forward;
         targetPosition(sp, targetPos);
#ifdef DEBUG_AI
         if (ii == 0) {
            m_debugTargetMesh->m_position = targetPos;
            //m_debugTargetMesh->m_position = furthestPoint;
            m_debugTargetMesh->m_isVisible = true;
            double ss{ 4. };
            m_debugTargetMesh->m_scale = { ss,ss,ss };
         }
#endif //DEBUG_AI
         glm::dvec3 targetDir{ targetPos - sp->m_rigidBody->m_pos };
         double targetDirL{ glm::length(targetDir) };
         if (targetDirL > 0.) {
            targetDir = targetDir / targetDirL;
         } else {
            targetDir = glm::dvec3{ 1,0,0 };
         }
         if (sp->m_aiState == SpaceShip::SHOOT && !doAvoid && glm::dot(targetDir, forward) > 0.999) {
            shoot(sp);
         }
         //
         if (targetDir.z > 0.5) {
            sp->m_thrustMultiplier = 2.;
         } else {
            sp->m_thrustMultiplier = 1.;
         }
      }


      //glm::dvec3 target{ 0.2,0.2,0.2 };
      //targetPosition(m_playerVehicle, target);
      //if (frameNum == 0) {
      //   Mesh* mm = m_gameEngine->m_graphicsEngine.createMesh(m_gameEngine->m_sphereModel);
      //   mm->m_position = target;
      //   double ss{ 0.01 };
      //   mm->m_scale = { ss,ss,ss };
      //}
   }
}

void GameLogic::handleUI(uint64_t /*frameNum*/) {


   if (m_followMode == FOLLOW && m_playerVehicle != nullptr) {

      RigidBody* rigidBody = m_playerVehicle->m_rigidBody;
      //int ind{ 3 };
      //if (m_gameEngine->m_spaceShips.size() > ind + 1) {
      //   rigidBody = m_gameEngine->m_spaceShips[ind]->m_rigidBody;
      //}
      GraphicsEngine* graphicsEngine{ &m_gameEngine->m_graphicsEngine };

      //
      glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
      glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
      glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };

      //
      glm::dvec3 angVel{ rigidBody->getAngularVel() };

      // Control camera.
      double offsetFactor{ 1. / (glm::pow(glm::abs(glm::dot(forward, angVel)), 2.) * 2048. + 1.) };
      m_followPos = glm::mix(
         m_followPos, rigidBody->m_pos + up * 5. * offsetFactor,
         PhysicsUnits::blendHalfLife(0.093352546530176397)
      );
      //double shipSpeed{ glm::dot(m_spaceShip->m_rigidBody->m_vel, forward) };
      //((s-t)*f-d)*f+t
      double distanceToShip{ 100. };
      glm::dvec3 targetPos = (glm::dot(
         rigidBody->m_pos - m_followPos,
         forward
      ) - distanceToShip) * forward + m_followPos;

      graphicsEngine->m_camPos = targetPos;
      glm::dvec3 lookAtPos{ rigidBody->m_pos };
      lookAtPos += forward * 512.;
      glm::dvec3 direction{ lookAtPos - graphicsEngine->m_camPos };
      direction = direction / glm::length(direction);
      m_followUp = glm::normalize(
         glm::mix(m_followUp, up, PhysicsUnits::blendHalfLife(0.18963810885644522))
      );
      graphicsEngine->m_camOri =
         glm::quatLookAt(direction, glm::dvec3{ 0,0,1 } + m_followUp * 4.0);
      const glm::dquat offset{ glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 }) };
      graphicsEngine->m_camOri = graphicsEngine->m_camOri * offset;
      //graphicsEngine->m_camOri = glm::conjugate(graphicsEngine->m_camOri);
      graphicsEngine->m_camOri = glm::normalize(graphicsEngine->m_camOri);
   }

   // UI.
   if (m_playerVehicle == nullptr) {
      for (size_t ii = 0; ii < m_crossHairsMeshes.size(); ii++) {
         Mesh* mesh{ m_crossHairsMeshes[ii] };
         mesh->m_isVisible = false;
      }
      m_enemyMarkerMesh->m_isVisible = false;
   } else {
      // Cross hairs.
      glm::dvec3 forward{ m_playerVehicle->m_rigidBody->m_ori * glm::dvec3{ 0,1,0 } };
      glm::dvec3 up{ m_playerVehicle->m_rigidBody->m_ori * glm::dvec3{ 0,0,1 } };
      for (size_t ii = 0; ii < m_crossHairsMeshes.size(); ii++) {
         Mesh* mesh{ m_crossHairsMeshes[ii] };
         mesh->m_isVisible = true;
         glm::dvec3 targetPos{ m_playerVehicle->m_rigidBody->m_pos };
         targetPos +=
            forward * 328. * glm::pow(3., (double)(ii));
         //targetPos -= glm::dot(targetPos - m_gameEngine->m_graphicsEngine.m_camPos, forward) * forward * 0.8;
         targetPos = glm::mix(targetPos, m_gameEngine->m_graphicsEngine.m_camPos, 0.99);
         //targetPos += up * 0.0002;
         mesh->m_position = targetPos;
         glm::dquat orientation{ glm::quatLookAt(m_playerVehicle->m_rigidBody->m_ori * glm::dvec3{0,1,0}, glm::dvec3{0,0,1}) };
         const glm::dquat offset{ glm::angleAxis(glm::radians(-90.), glm::dvec3{ 1,0,0 }) };
         mesh->m_orientation = orientation * offset;

         double scale{ glm::length(mesh->m_position - m_gameEngine->m_graphicsEngine.m_camPos) };
         scale *= 0.03 * glm::pow(0.7, (double)(ii));
         mesh->m_scale = { scale, scale, scale };
      }

      // Enemy marker.
      double largestCos{ -2 };
      SpaceShip* targetSpaceShip{ nullptr };
      for (size_t ii = 0; ii < m_gameEngine->m_spaceShips.size(); ii++) {
         SpaceShip* ss{ m_gameEngine->m_spaceShips[ii] };
         if (ss == m_playerVehicle) {
            continue;
         }
         glm::dvec3 diff{ ss->m_rigidBody->m_pos - m_playerVehicle->m_rigidBody->m_pos };
         double diffL{ glm::length(diff) };
         if (diffL == 0.) {
            continue;
         }
         diff = diff / diffL;
         double dot{ glm::dot(diff, forward) };
         if (dot > largestCos) {
            largestCos = dot;
            targetSpaceShip = ss;
         }
      }
      if (targetSpaceShip == nullptr) {
         m_enemyMarkerMesh->m_isVisible = false;
      } else {
         m_enemyMarkerMesh->m_isVisible = false;
         //m_enemyMarkerMesh->m_isVisible = true;
         //GraphicsEngine* ge{ &m_gameEngine->m_graphicsEngine };
         //double scale{0.1 * glm::length(
         //   ge->m_camPos - targetSpaceShip->m_rigidBody->m_pos
         //)};
         //scale = 0.2;
         //m_enemyMarkerMesh->m_scale = { scale, scale, scale };
         //m_enemyMarkerMesh->m_orientation = ge->m_camOri;
         //
         //glm::dvec3 diff{ targetSpaceShip->m_rigidBody->m_pos - ge->m_camPos };
         //glm::dvec3 posLocal{ glm::conjugate(ge->m_camOri) * diff };
         //double maxFOVH{ ge->m_fieldOfView };
         //double maxFOVV{ ge->m_fieldOfView * (double)ge->m_screen_height / (double)ge->m_screen_width };
         //double maxLengthH{ glm::tan(maxFOVH * 0.5) };
         //maxLengthH *= 0.85;
         //double maxLengthV{glm::tan(maxFOVV * 0.5)};
         //maxLengthH *= posLocal.y;
         //maxLengthV *= posLocal.y;
         //glm::dvec2 sideways{ posLocal.x, posLocal.z };
         //if (sideways.x > maxLengthH) {
         //   sideways.x = maxLengthH;
         //} else if (sideways.x < -maxLengthH) {
         //   sideways.x = -maxLengthH;
         //}
         //if (sideways.y > maxLengthV) {
         //   sideways.y = maxLengthV;
         //} else if (sideways.y < -maxLengthV) {
         //   sideways.y = -maxLengthV;
         //}
         //posLocal = { sideways.x, posLocal.y, sideways.y };
         //double posLocalLength{ glm::length(posLocal) };
         //if (posLocalLength > 0.) {
         //   posLocal = posLocal / posLocalLength * 10.;
         //}
         //
         //m_enemyMarkerMesh->m_position = ge->m_camPos + ge->m_camOri * posLocal;
      }
   }
}

void GameLogic::shoot(SpaceShip* spaceShip) {
   RigidBody* rigidBody{ spaceShip->m_rigidBody };
   if (
      spaceShip->m_nextTimeToShoot <= m_gameEngine->m_graphicsEngine.m_frameNum &&
      (uint64_t)spaceShip->m_nextTimeCoolDown <= m_gameEngine->m_graphicsEngine.m_frameNum
   ) {
      spaceShip->m_nextTimeToShoot = m_gameEngine->m_graphicsEngine.m_frameNum + spaceShip->m_shotIntervalTime;
      spaceShip->m_nextTimeCoolDown = std::max(
         spaceShip->m_nextTimeCoolDown + spaceShip->m_heatPerBullet,
         (int64_t)m_gameEngine->m_graphicsEngine.m_frameNum - spaceShip->m_turretMaxHeat
      );
      
      glm::dvec3 shootPos{ spaceShip->getShootingPosition() };
      spaceShip->nextShootingPosition();
      Projectile* projectile{ m_gameEngine->createProjectile(shootPos) };
      projectile->m_owner = spaceShip->m_id;
      
      glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{ 0, 1, 0 } };
      glm::dvec3 shootingDir{ forward };

      //
      if (spaceShip->m_doAimAssist) {
         double largestCos{ -2. };
         SpaceShip* targetSpaceShip{ nullptr };

         for (size_t ii = 0; ii < m_gameEngine->m_spaceShips.size(); ii++) {
            SpaceShip* other{ m_gameEngine->m_spaceShips[ii]};
            glm::dvec3 direction{ other->m_rigidBody->m_pos - spaceShip->m_rigidBody->m_pos };
            double dirLength{ glm::length(direction) };
            if (dirLength == 0.) {
               continue;
            }
            direction = direction / dirLength;
            double dot{ glm::dot(direction,forward) };
            if (dot > largestCos) {
               largestCos = dot;
               targetSpaceShip = other;
            }
         }
         if (largestCos > 0.) {
            glm::dvec3 targetPos{}; bool canHit{}; double time{};
            GameLogic::trajectoryInterceptionWithGravity(
               spaceShip, targetSpaceShip, 1., &targetPos, &canHit, &time
            );
            if (canHit) {
               glm::dvec3 aheadDir{ targetPos - spaceShip->m_rigidBody->m_pos };
               double aheadLength{ glm::length(aheadDir) };
               if (aheadLength > 0.) {
                  aheadDir = aheadDir / aheadLength;
                  double shiftFactor{glm::pow(
                        glm::max(glm::dot(forward, aheadDir), 0.),
                        64.
                  ) * 1.0};
                  shootingDir = glm::mix(shootingDir, aheadDir, shiftFactor);
                  double shootingDirLength{ glm::length(shootingDir) };
                  if (shootingDirLength == 0.) {
                     shootingDir = forward;
                  } else {
                     shootingDir = shootingDir / shootingDirLength;
                  }
               }
            }
         }
      }

      //
      glm::dvec3 shootingVelocity{ shootingDir * spaceShip->m_shootingSpeed };
      
      // Add noise.
      shootingVelocity +=
         (Hash::pcgUnit3(m_gameEngine->m_graphicsEngine.m_frameNum) - 0.5) *
         spaceShip->m_shootSpread * spaceShip->m_shootingSpeed;
      projectile->m_rigidBody->m_vel = rigidBody->m_vel + shootingVelocity;
      projectile->m_damage = spaceShip->m_projectileDamage;
      //m_gameEngine->m_soundPlayer.playSound(m_gameEngine->m_flaunchSound, 1., 1.);
      
      // Add sound.
      double pitch{Hash::pcgUnit(
         m_gameEngine->m_graphicsEngine.m_frameNum + spaceShip->m_id
      ) + 0.5 };
      //pitch = 1.;
      double fraction{glm::clamp(
         (double)((int64_t)m_gameEngine->m_graphicsEngine.m_frameNum - spaceShip->m_nextTimeCoolDown) /
         (double)spaceShip->m_turretMaxHeat,
         0., 1.
      )};
      pitch *= 1./(fraction + 0.2);
      //pitch *= -glm::pow(fraction * 3. - 2.2, 3.) + 1.;
      m_gameEngine->playSoundAtLocation(
         spaceShip->m_rigidBody->m_pos,
         m_gameEngine->m_flaunchSound, pitch, 1000., 0.1
      );
   }
}

void GameLogic::handleInput(uint64_t /*frameNum*/) {
   m_keyBoardHandler->update();

   GraphicsEngine* graphicsEngine{ &m_gameEngine->m_graphicsEngine };
   MouseHandler* mouseHandler{ graphicsEngine->m_mouseHandler };
   GLFWwindow* window{ graphicsEngine->m_window };

   // Camera controls.
   //double height{ graphicsEngine->m_camPos.z - 500 };
   //std::cout << height << std::endl;
   //double height{
   //   m_gameEngine->m_graphicsEngine.m_camPos.z -
   //   m_planet->map(m_gameEngine->m_graphicsEngine.m_camPos, 3).z
   //};
   //int level = (int)glm::log2(glm::max(height * 0.005, 1.0)) + 13;
   //std::cout << level << std::endl;
   int level = 16;
   m_planet->m_surface->setLevel(level);

   // Quit.
   if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
      glfwSetWindowShouldClose(window, true);

   // Control.
   if (m_followMode == FREE) {

      // Translate.
      if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ 0,1,0 } * m_camSpeed;
      if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ 0,-1,0 } * m_camSpeed;
      if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ 1,0,0 } * m_camSpeed;
      if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ -1,0,0 } * m_camSpeed;
      if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ 0,0,1 } * m_camSpeed;
      if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
         graphicsEngine->m_camPos += graphicsEngine->m_camOri * glm::dvec3{ 0,0,-1 } * m_camSpeed;

      // Accelerate.
      double camSpeedHalfLife{ PhysicsUnits::halfLife(0.11838915902408702) };
      if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS)
         m_camSpeed /= camSpeedHalfLife;
      if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS)
         m_camSpeed *= camSpeedHalfLife;

      // Rotate.
      double rotSpeed{ PhysicsUnits::radiansPerSecond(1.2) * graphicsEngine->m_fieldOfView };
      glm::dvec3 rotation{};
      // Keyboard.
      if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
         rotation += glm::dvec3{ rotSpeed,0,0 };
      if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
         rotation += glm::dvec3{ -rotSpeed,0,0 };
      if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
         rotation += glm::dvec3{ 0,0,-rotSpeed };
      if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
         rotation += glm::dvec3{ 0,0,rotSpeed };
      if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
         rotation += glm::dvec3{ 0,-PhysicsUnits::radiansPerSecond(1.2),0 };
      if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
         rotation += glm::dvec3{ 0,PhysicsUnits::radiansPerSecond(1.2),0 };
      // Mouse.
      if (graphicsEngine->m_mouseHandler->getMouseLock()) {
         double mouseSensitivity{0.002};
         rotation += glm::dvec3{
            -mouseHandler->m_mouseMovement.y,
            0.,
            -mouseHandler->m_mouseMovement.x,
         } * mouseSensitivity;
      }
      //
      double rotationLength{ glm::length(rotation) };
      if (rotationLength > 0.) {
         graphicsEngine->m_camOri = glm::rotate(
            graphicsEngine->m_camOri, glm::length(rotation), rotation
         );
      }
   } else if (m_followMode == FOLLOW && m_playerVehicle != nullptr) {

      RigidBody* rigidBody = m_playerVehicle->m_rigidBody;

      // Shoot.
      //bool tt{ true };
      if (mouseHandler->m_leftMouseDown || m_keyBoardHandler->m_space.m_isDown) {
         shoot(m_playerVehicle);
      }
      //
      glm::dvec3 forward{ rigidBody->m_ori * glm::dvec3{0,1,0} };
      glm::dvec3 right{ rigidBody->m_ori * glm::dvec3{1,0,0} };
      glm::dvec3 up{ rigidBody->m_ori * glm::dvec3{0,0,1} };

      // Control space ship.
      glm::dvec3 torqueLocal{ 0,0,0 };
      // Rotation.
      if (m_controlScheme == ARCADE) {
         // -Keyboard.
         double arrowKeyReduceFactor{0.5};
         if (m_keyBoardHandler->m_w.m_isDown)
            torqueLocal.x += m_playerVehicle->m_pitchUpMax;
         if (m_keyBoardHandler->m_up.m_isDown)
            torqueLocal.x += m_playerVehicle->m_pitchUpMax * arrowKeyReduceFactor;
         if (m_keyBoardHandler->m_s.m_isDown)
            torqueLocal.x -= m_playerVehicle->m_pitchDownMax;
         if (m_keyBoardHandler->m_down.m_isDown)
            torqueLocal.x -= m_playerVehicle->m_pitchDownMax * arrowKeyReduceFactor;
         if (m_keyBoardHandler->m_right.m_isDown)
            torqueLocal.z -= m_playerVehicle->m_yawMax;
         if (m_keyBoardHandler->m_left.m_isDown)
            torqueLocal.z += m_playerVehicle->m_yawMax;
         if (m_keyBoardHandler->m_a.m_isDown)
            torqueLocal.y -= m_playerVehicle->m_rollMax;
         if (m_keyBoardHandler->m_d.m_isDown)
            torqueLocal.y += m_playerVehicle->m_rollMax;
         if (torqueLocal.y == 0.) {
            torqueLocal.y +=
               -glm::dot(rigidBody->getAngularVel(), forward) * PhysicsUnits::perSecond(1.2);
         }
         // -Mouse.
         double rotationFactorMouse{
            PhysicsUnits::radiansPerSecond(0.006) *
            graphicsEngine->m_fieldOfView / 1.5707963267948966
         };
         glm::dvec2 joyStick{ mouseHandler->m_mouseMovement * rotationFactorMouse };
         if (
            m_keyBoardHandler->m_w.m_isDown && joyStick.y > 0. ||
            m_keyBoardHandler->m_s.m_isDown && joyStick.y < 0.
            ) {
            joyStick.y = 0.;
         }
         //if (m_gameEngine->m_graphicsEngine.m_frameNum % 10 == 0) {
         //   std::cout << joyStick.y << std::endl;
         //}
         torqueLocal.x += -joyStick.y;
         torqueLocal.z += -joyStick.x;

      } else if (m_controlScheme == REALISTIC) {
         // -Keyboard.
         if (m_keyBoardHandler->m_up.m_isDown || m_keyBoardHandler->m_w.m_isDown)
            torqueLocal.x += m_playerVehicle->m_pitchUpMax;
         if (m_keyBoardHandler->m_down.m_isDown || m_keyBoardHandler->m_s.m_isDown)
            torqueLocal.x -= m_playerVehicle->m_pitchDownMax;
         if (m_keyBoardHandler->m_right.m_isDown)
            torqueLocal.y -= m_playerVehicle->m_rollMax;
         if (m_keyBoardHandler->m_left.m_isDown)
            torqueLocal.y += m_playerVehicle->m_rollMax;
         // -Mouse.
         double rotationFactorMouse{ 0.00003 * graphicsEngine->m_fieldOfView / 1.5707963267948966 };
         glm::dvec2 joyStick{ mouseHandler->m_mouseMovement * rotationFactorMouse };
         torqueLocal.x += joyStick.y * rotationFactorMouse;
         torqueLocal.y += joyStick.x * rotationFactorMouse;
         torqueLocal.z += joyStick.x * -0.05 * rotationFactorMouse;
      } else {
         std::cout << "ERROR: controlScheme not implemented." << std::endl;
         throw "ERROR: controlScheme not implemented.";
      }

      // Apply.
      m_playerVehicle->applyTorqueCapped(torqueLocal);
      //glm::dvec3 angVel{ rigidBody->m_rotationAxis };
      //double axisLength{ glm::length(angVel) };
      //if (axisLength > 0.) {
      //   angVel = angVel / axisLength * rigidBody->m_rotationVel;
      //   angVel += torque;
      //} else {
      //   angVel = torque;
      //}
      //rigidBody->m_rotationAxis = angVel;
      //rigidBody->m_rotationVel = glm::length(angVel);
   }

   // Zoom level.
   double fovZoomFactor{ PhysicsUnits::halfLife(0.5747296994710671) };
   if (m_keyBoardHandler->m_f.m_isDown)
      graphicsEngine->m_fieldOfView *= fovZoomFactor;
   if (m_keyBoardHandler->m_g.m_isDown)
      graphicsEngine->m_fieldOfView /= fovZoomFactor;

   // Triangle mode.
   if (m_keyBoardHandler->m_t.justPressed())
      graphicsEngine->setTriangleRenderMode(!graphicsEngine->getTriangleRenderMode());

   // Toggle fullscreen with F11 key
   if (m_keyBoardHandler->m_f11.justPressed())
      graphicsEngine->toggleFullscreen();

   // Refresh shaders.
   if (m_keyBoardHandler->m_l.justPressed())
      m_planet->refreshShaders();

   // Follow mode.
   if (m_keyBoardHandler->m_o.justPressed()) {
      if (m_followMode == FREE) {
         m_followMode = FOLLOW;
      } else {
         m_followMode = FREE;
      }
   }

   // Lock mouse.
   if (m_keyBoardHandler->m_m.justPressed()) {
      bool doLock{ !mouseHandler->getMouseLock() };
      mouseHandler->setMouseLock(doLock);
   }

   // Pause.
   if (m_keyBoardHandler->m_p.justPressed()) {
      //m_gameEngine->m_pause = !m_gameEngine->m_pause;
      m_gameEngine->m_graphicsEngine.setSwapInterval(64);
   }

   // Speed.
   if (m_keyBoardHandler->m_n.justPressed()) {
      m_simulationSpeed = (m_simulationSpeed + 1) % 3;
      switch (m_simulationSpeed) {
      case 0:
         m_gameEngine->m_graphicsEngine.setSwapInterval(1);
         break;
      case 1:
         m_gameEngine->m_graphicsEngine.setSwapInterval(4);
         break;
      case 2:
         m_gameEngine->m_graphicsEngine.setSwapInterval(16);
         break;
      default:
         break;
      }
   }

   // Restart game.
   if (m_keyBoardHandler->m_r.justPressed()) {
      while (m_gameEngine->m_spaceShips.size() > 0) {
         m_gameEngine->removeSpaceShip(m_gameEngine->m_spaceShips[0]);
      }
      while (m_gameEngine->m_projectiles.size() > 0) {
         m_gameEngine->removeProjectile(m_gameEngine->m_projectiles[0]);
      }
      m_numEnemiesKilledLast = 0;
      m_winTime = -1;
      m_totalSpaceShips = m_totalSpaceShipsInit;
      createPlayerSpaceShip();
   }

   //
   //std::cout << "Campos.x: " << m_gameEngine->m_graphicsEngine.m_camPos.x << std::endl;

}

void GameLogic::handleDebug(uint64_t /*frameNum*/) {
   //for (size_t yy = 0; yy < m_debugGrid.size(); yy++) {
   //   for (size_t xx = 0; xx < m_debugGrid[0].size(); xx++) {
   //      glm::dvec3 position{ m_gameEngine->m_graphicsEngine.m_camPos };
   //      double ss{ 10. };
   //      position.x += (xx - (m_debugGrid[0].size() - 1) * 0.5) * ss;
   //      position.y += (yy - (m_debugGrid.size() - 1) * 0.5) * ss;
   //      position.z = m_planet->map(position, 3).z + 0.00001;
   //      m_debugGrid[yy][xx]->m_position = position;
   //   }
   //}
   //if (m_gameEngine->m_graphicsEngine.m_frameNum > 50 && m_gameEngine->m_graphicsEngine.m_frameNum < 72) {
   //   shoot(m_playerVehicle);
   //}
   //if (m_gameEngine->m_graphicsEngine.m_frameNum == 50) {
   //   shoot(m_playerVehicle);
   //}
   
   //if (frameNum == 144) {
   //   MovingParticleEffect* mpe{ m_gameEngine->createMovingParticleEffect(m_gameEngine->m_plasmaExplosionEffect) };
   //   mpe->m_rigidBody->m_pos = { 0,0,0.01 };
   //   double ss{ 0.01 };
   //   mpe->m_particleEffectInstance->m_scale = { ss,ss,ss };
   //   mpe->m_lifeTime = 99999;
   //   //mpe->m_rigidBody->m_vel.z = 0.001;
   //}
   //if(frameNum >= 144) m_gameEngine->m_movingParticleEffects[0]->m_rigidBody->m_vel.z = 0.;

   //if (m_gameEngine->m_graphicsEngine.m_frameNum % 60 == 0) {
   //   std::cout << "Campos:" << std::endl <<
   //      "x:" << m_gameEngine->m_graphicsEngine.m_camPos.x << std::endl <<
   //      "y:" << m_gameEngine->m_graphicsEngine.m_camPos.y << std::endl <<
   //      "z:" << m_gameEngine->m_graphicsEngine.m_camPos.z << std::endl << std::endl;
   //}
   //if (m_gameEngine->m_graphicsEngine.m_frameNum == 144*10) {
   //   //m_gameEngine->damageSpaceShip(1, m_playerVehicle, 100.);
   //}
   //if (m_gameEngine->m_graphicsEngine.m_frameNum % 20 == 0 && m_playerVehicle != nullptr) {
   //   std::cout << "pos" << std::endl << m_playerVehicle->m_rigidBody->m_pos.x << std::endl <<
   //      m_playerVehicle->m_rigidBody->m_pos.y << std::endl <<
   //      m_playerVehicle->m_rigidBody->m_pos.z << std::endl;
   //   std::cout << "ori" << std::endl << m_playerVehicle->m_rigidBody->m_ori.x << std::endl <<
   //      m_playerVehicle->m_rigidBody->m_ori.y << std::endl <<
   //      m_playerVehicle->m_rigidBody->m_ori.z << std::endl <<
   //      m_playerVehicle->m_rigidBody->m_ori.w << std::endl;
   //}
   //if (m_gameEngine->m_graphicsEngine.m_frameNum % 100 == 0) {
   //   std::cout
   //      << "x: " << m_gameEngine->m_graphicsEngine.m_camPos.x << std::endl
   //      << "y: " << m_gameEngine->m_graphicsEngine.m_camPos.y << std::endl
   //      << "z: " << m_gameEngine->m_graphicsEngine.m_camPos.z << std::endl;
   //}
}

void GameLogic::preRenderCallback(uint64_t frameNum) {
   handleGameEvents(frameNum);
   handleSoundEffects();
   handleInput(frameNum);
   handleAI(frameNum);
   handleUI(frameNum);
   handleDebug(frameNum);
}

void GameLogic::trajectoryInterception(
   glm::dvec3 wV1, glm::dvec3 wP1, glm::dvec3 wV2, glm::dvec3 wP2, double absV,
   glm::dvec3* pos, bool* canHit, double* time
) {
   // takes two points two velocities and a speed. Gives the point
   // in space where a particle would hit point 2 with the initial
   // speed absV relative to point1's position and velocityvar t;
   glm::dvec3 v2{ wV2 - wV1 };
   glm::dvec3 p2{wP2 - wP1};
   double dotP2V2{ glm::dot(p2, v2) };
   double dotV2V2{ glm::dot(v2, v2) };
   double dotP2P2{glm::dot(p2, p2)};
   double discr = dotP2V2 * dotP2V2 - (dotV2V2 - absV * absV) * dotP2P2;
   if (discr < 0) {
      *time = 0.;
      *pos = glm::dvec3{ 0, 0, 0 };
      *canHit = false;
      return;
   }
   double divisor{ absV * absV - dotV2V2 };
   if (divisor == 0) {
      *time = 0.;
      *pos = glm::dvec3{};
      *canHit = false;
      return;
   }
   *time = (glm::sqrt(discr) + dotP2V2) / divisor;
   if (*time < 0) {
      *time = 0;
      *pos = glm::dvec3{ 0,0,0 };
      *canHit = false;
      return;
   }
   //pos = p2.Add(v2.Mul(t), wP1);
   *pos = wP2 + wV2 * (*time);
   *canHit = true;
   return;
}

void GameLogic::trajectoryInterceptionWithGravity(
   SpaceShip* ss1, SpaceShip* ss2, double scaleFactor, glm::dvec3* pos, bool* canHit, double* time
) {
   GameLogic::trajectoryInterception(
      ss1->m_rigidBody->m_vel, ss1->m_rigidBody->m_pos,
      ss2->m_rigidBody->m_vel, ss2->m_rigidBody->m_pos, ss1->m_shootingSpeed / scaleFactor,
      pos, canHit, time
   );
   double upDistance{ m_gameEngine->m_phyisicsEngine.m_gravity * (*time) * (*time) * 0.5};
   pos->z += upDistance;
}
