#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vector>
#include <unordered_map>
#include <cstdint>
#include <string>

class MouseHandler {
protected:
   static void cursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
   static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
   static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);

   uint64_t m_frameNum{ 0 };
   static std::unordered_map<std::uintptr_t, MouseHandler*> m_windowsToMouseHandlers;
   bool m_mouseIsLocked{ false };

   glm::dvec2 m_mousePosPrev{};
   bool m_prevLeftMouseDown{ false };
   bool m_prevRightMouseDown{ false };
   double m_scrollPositionPrev{ 0. };
public:
   MouseHandler(GLFWwindow* window);
   ~MouseHandler();

   void update();
   void setMouseLock(bool lockMouse);
   bool getMouseLock();
   bool leftClick();
   bool rightClick();
   void startRecording();
   void stopRecording();
   std::string stateToString();
   void stateFromString(std::string state);

   GLFWwindow* m_window{ nullptr };
   // Mouse.
   glm::dvec2 m_mousePos{};
   glm::dvec2 m_mouseMovement{};
   bool m_leftMouseDown{ false };
   bool m_rightMouseDown{ false };
   uint64_t m_timeLeftDown{ 0 };
   uint64_t m_timeLeftUp{ 0 };
   uint64_t m_timeRightDown{ 0 };
   uint64_t m_timeRightUp{ 0 };
   double m_scrollPosition{ 0. };
   double m_scrollMovement{};
   bool m_doRecord{ false };

   std::vector<std::string> m_recordings{};
};
