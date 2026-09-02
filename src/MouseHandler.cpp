#include "MouseHandler.h"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <numeric>

std::unordered_map<std::uintptr_t, MouseHandler*> MouseHandler::m_windowsToMouseHandlers{};

void MouseHandler::cursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
   size_t key{ reinterpret_cast<std::uintptr_t>(window) };
   if (MouseHandler::m_windowsToMouseHandlers.find(key) !=
      MouseHandler::m_windowsToMouseHandlers.end()) {
      MouseHandler* mouseHandler{ MouseHandler::m_windowsToMouseHandlers[key] };
      mouseHandler->m_mousePos.x = xpos;
      mouseHandler->m_mousePos.y = ypos;
      //std::cout << "xpos: " << xpos << std::endl << "ypos: " << ypos << std::endl << std::endl;
   }
}

void MouseHandler::mouseButtonCallback(GLFWwindow* window, int button, int action, int /*mods*/) {
   size_t key{ reinterpret_cast<std::uintptr_t>(window) };
   if (MouseHandler::m_windowsToMouseHandlers.find(key) !=
      MouseHandler::m_windowsToMouseHandlers.end()) {
      MouseHandler* mouseHandler{ MouseHandler::m_windowsToMouseHandlers[key] };
      if (button == GLFW_MOUSE_BUTTON_LEFT) {
         mouseHandler->m_leftMouseDown = action == GLFW_PRESS;
      } else if (button == GLFW_MOUSE_BUTTON_RIGHT) {
         mouseHandler->m_rightMouseDown = action == GLFW_PRESS;
      }
   }
}

void MouseHandler::scrollCallback(GLFWwindow* window, double /*xoffset*/, double yoffset) {
   size_t key{ reinterpret_cast<std::uintptr_t>(window) };
   if (MouseHandler::m_windowsToMouseHandlers.find(key) !=
      MouseHandler::m_windowsToMouseHandlers.end()) {
      MouseHandler* mouseHandler{ MouseHandler::m_windowsToMouseHandlers[key] };
      mouseHandler->m_scrollPosition += yoffset;
   }
}

MouseHandler::MouseHandler(GLFWwindow* window) {
   m_window = window;
   size_t key{ reinterpret_cast<std::uintptr_t>(window) };
   if (MouseHandler::m_windowsToMouseHandlers.find(key) !=
      MouseHandler::m_windowsToMouseHandlers.end()) {
      std::cout << "Error: Cannot assign window to two mouse handlers." << std::endl;
      throw "Error: Cannot assign window to two mouse handlers.";
   }
   MouseHandler::m_windowsToMouseHandlers[key] = this;
   //
   glfwSetCursorPosCallback(m_window, cursorPositionCallback);
   glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
   glfwSetScrollCallback(m_window, scrollCallback);
   glfwSetCursorPos(m_window, 0, 0); // Prevents jumping of mouse when first entering the window.
}

MouseHandler::~MouseHandler() {
   size_t key{ reinterpret_cast<std::uintptr_t>(m_window) };
   MouseHandler::m_windowsToMouseHandlers.erase(key);
}

void MouseHandler::update() {
   // Record/playback.
   if (m_doRecord) {
      //MouseHandler mh{ *this };

      //m_recordings.push_back(*this);
   }

   // Movement.
   if (m_frameNum == 0) {

   } else {
      m_mouseMovement = m_mousePos - m_mousePosPrev;
      m_scrollMovement = m_scrollPosition - m_scrollPositionPrev;
   }
   m_mousePosPrev = m_mousePos;
   m_scrollPositionPrev = m_scrollPosition;

   // Mouse click.
   if (m_prevLeftMouseDown) {
      if (m_leftMouseDown) {
         m_timeLeftDown++;
      } else {
         m_timeLeftUp = 0;
      }
   } else {
      if (m_leftMouseDown) {
         m_timeLeftDown = 0;
      } else {
         m_timeLeftUp++;
      }
   }
   m_prevLeftMouseDown = m_leftMouseDown;

   if (m_prevRightMouseDown) {
      if (m_rightMouseDown) {
         m_timeRightDown++;
      } else {
         m_timeRightUp = 0;
      }
   } else {
      if (m_rightMouseDown) {
         m_timeRightDown = 0;
      } else {
         m_timeRightUp++;
      }
   }
   m_prevRightMouseDown = m_rightMouseDown;

   m_frameNum++;
}

void MouseHandler::setMouseLock(bool lockMouse) {
   if (lockMouse) {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   } else {
      glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
   }
   m_mouseIsLocked = lockMouse;
}

bool MouseHandler::getMouseLock() {
   return m_mouseIsLocked;
}

bool MouseHandler::leftClick() {
   return m_leftMouseDown && m_timeLeftDown == 0;
}

bool MouseHandler::rightClick() {
   return m_rightMouseDown && m_timeRightDown == 0;
}

void MouseHandler::startRecording() {
   m_doRecord = true;
   m_recordings = {};
}

void MouseHandler::stopRecording() {
   m_doRecord = false;
   m_recordings = {};
}

std::string MouseHandler::stateToString() {
   std::vector<std::string> result{
      std::to_string(m_mousePos.x), std::to_string(m_mousePos.y),
      std::to_string(m_leftMouseDown),
      std::to_string(m_rightMouseDown),
      std::to_string(m_scrollPosition),
   };
   return {};
   //return std::accumulate(result.begin(), result.end(), ",");
}

void MouseHandler::stateFromString(std::string state) {
   std::vector<char> word{};
   std::vector<std::string> results{};
   for (size_t ii = 0; ii < state.size(); ii++) {
      char nextChar{ state[ii] };
      if (nextChar == ',') {
         results.push_back(std::accumulate(word.begin(), word.end(), std::string{}));
         word = {};
      } else {
         word.push_back(nextChar);
      }
   }
   results.push_back(std::accumulate(word.begin(), word.end(), std::string{}));
   m_mousePos.x = std::stod(results[0]);
   m_mousePos.y = std::stod(results[1]);
   m_leftMouseDown = results[2] == "1";
   m_rightMouseDown = results[3] == "1";
   m_scrollPosition = std::stod(results[4]);
}
