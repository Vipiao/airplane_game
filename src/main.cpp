

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <AL/al.h>
#include <AudioFile/AudioFile.h>

#include "ShaderProgram.h"
#include "Surface.h"
#include "GraphicsEngine.h"
#include "GameLogic.h"
#include "GlobalConstants.h"
#include "DebugTimer.h"
//#include "SoundPlayer.h"

#include <iostream>
#include <vector>


int main(int /*argc*/, char* /*argv[]*/) {

   GameLogic* gameLogic = new GameLogic();
   delete gameLogic;


#ifdef  DO_TIMING
   std::cout << "Timer result:\n" << DebugTimer::getResultsString() << std::endl;
#endif //  DO_TIMING

   return 0;
}



