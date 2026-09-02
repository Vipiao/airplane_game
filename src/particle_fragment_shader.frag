
#version 410 core

#define FRAGMENT_SHADER

out vec4 FragColor;

in vec3 fragPos;
in vec2 texCoordVert;
flat in int instance_id;

//uniform sampler2D textureSampler;
uniform int time;
uniform vec3 camPos;
uniform int numParticles;
uniform int startTime;
uniform float animationPace;
uniform mat4 view;

// --- Color chooser Function will be replaced FROM here ---
vec4 colorChooser(vec3 p, vec2 uv){
   return vec4(1,1,1,1);
}
// --- Color chooser Function function will be replaced TO here ---

void main()
{

   vec3 light = vec3(1);
   //vec3 viewDir = normalize(fragPos - camPos);
   
   //
   vec4 objectColor = colorChooser(fragPos, texCoordVert);
   //vec4 objectColor = texture(textureSampler, texCoordVert);
   float alpha = objectColor.w;
   if(alpha < 0.001) {
      discard;
   }
   vec4 result = vec4(light, 1.) * objectColor;

   FragColor = vec4(result);
}





