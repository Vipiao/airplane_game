#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <vector>


class KeyboardHandler {
protected:
   class Button {
      friend class KeyboardHandler;
   protected:
      bool m_isDownPrevious{ false };
      int m_keyCode{-1};
   public:
      Button(int keyCode) {
         m_keyCode = keyCode;
      }
      bool justPressed() {
         return m_isDown && m_timeDown == 0;
      }

      bool m_isDown{ false };
      uint64_t m_timeDown{ 0 };
      uint64_t m_timeUp{ 0 };
   };
public:
   KeyboardHandler(GLFWwindow* window);

   void update();

   GLFWwindow* m_window{ nullptr };
   bool m_doRecord{ false };
   std::vector<Button*> m_buttons{};

   Button m_q{GLFW_KEY_Q};
   Button m_w{GLFW_KEY_W};
   Button m_e{GLFW_KEY_E};
   Button m_r{GLFW_KEY_R};
   Button m_t{GLFW_KEY_T};
   Button m_y{GLFW_KEY_Y};
   Button m_u{GLFW_KEY_U};
   Button m_i{GLFW_KEY_I};
   Button m_o{GLFW_KEY_O};
   Button m_p{GLFW_KEY_P};
   Button m_a{GLFW_KEY_A};
   Button m_s{GLFW_KEY_S};
   Button m_d{GLFW_KEY_D};
   Button m_f{GLFW_KEY_F};
   Button m_g{GLFW_KEY_G};
   Button m_h{GLFW_KEY_H};
   Button m_j{GLFW_KEY_J};
   Button m_k{GLFW_KEY_K};
   Button m_l{GLFW_KEY_L};
   Button m_z{GLFW_KEY_Z};
   Button m_x{GLFW_KEY_X};
   Button m_c{GLFW_KEY_C};
   Button m_v{GLFW_KEY_V};
   Button m_b{GLFW_KEY_B};
   Button m_n{GLFW_KEY_N};
   Button m_m{GLFW_KEY_M};

   Button m_lShift{ GLFW_KEY_LEFT_SHIFT };
   Button m_rShift{ GLFW_KEY_RIGHT_SHIFT};
   Button m_lCtrl{ GLFW_KEY_LEFT_CONTROL };
   Button m_rCtrl{ GLFW_KEY_RIGHT_CONTROL };
   Button m_space{ GLFW_KEY_SPACE };

   Button m_right{ GLFW_KEY_RIGHT };
   Button m_left{ GLFW_KEY_LEFT };
   Button m_up{ GLFW_KEY_UP };
   Button m_down{ GLFW_KEY_DOWN };
};

