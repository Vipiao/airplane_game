#pragma once

#include "GameEngine.h"
#include "KeyboardHandler.h"

class GameLogic : public GameEngineCallback {
protected:
public:
   enum Configuration {
      
   };
   enum CameraMode {
      FREE,
      FOLLOW
   } m_followMode{ FOLLOW };
   enum ControlScheme {
      REALISTIC,
      ARCADE,
   } m_controlScheme{ ARCADE };
   enum GameState {
      PLAY,
      PAUSE
   } m_gameState{ PLAY };
   GameEngine* m_gameEngine{ nullptr };
   KeyboardHandler* m_keyBoardHandler{ nullptr };
   Planet* m_planet{ nullptr };
   glm::dvec3 m_followPos{ 0,0,0 };
   glm::dvec3 m_followUp{ 0,0,1 };
   double m_camSpeed{ 0.01 };
   int m_playerVehicleId{ -2 };
   SpaceShip* m_playerVehicle{ nullptr };
   Model* m_enemyMarkerModel{ nullptr };
   Mesh* m_enemyMarkerMesh{ nullptr };
   Model* m_crossHair{ nullptr };
   std::vector<Mesh*> m_crossHairsMeshes{};
   std::vector<std::vector<Mesh*>> m_debugGrid{};
   int m_simulationSpeed{};
   //double m_shootSpeed{ 0.001 };
   int m_numEnemiesKilledLast{ 0 };
   int m_winTime{ -1 };
   Mesh* m_debugTargetMesh{ nullptr };
   int m_totalSpaceShipsInit{ 100 };
   int m_totalSpaceShips{ 100 };
   int m_maxSpaceships{ 15 };

   GameLogic();
   ~GameLogic();
   void createPlayerSpaceShip();
   void handleGameEvents(uint64_t frameNum);
   void handleSoundEffects();
   void joystickControlSpaceShip(SpaceShip* spaceShip, glm::dvec2 joystick);
   void targetPosition(SpaceShip* spaceShip, glm::dvec3 position);
   void handleAI(uint64_t frameNum);
   void handleUI(uint64_t frameNum);
   void shoot(SpaceShip* spaceShip);
   void handleInput(uint64_t frameNum);
   void handleDebug(uint64_t frameNum);
   void preRenderCallback(uint64_t frameNum);
   static void trajectoryInterception(
      glm::dvec3 wV1, glm::dvec3 wP1, glm::dvec3 wV2, glm::dvec3 wP2, double absV,
      glm::dvec3* pos, bool* canHit, double* time
   );
   void trajectoryInterceptionWithGravity(
      SpaceShip* ss1, SpaceShip* ss2, double scaleFactor, glm::dvec3* pos, bool* canHit, double* time
   );
};

