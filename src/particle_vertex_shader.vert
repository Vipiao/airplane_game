#version 410 core

#define VERTEX_SHADER

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 texCoord;
layout (location = 2) in vec3 random;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 camPos;
uniform int time;
uniform int numParticles;
uniform int startTime;
uniform float animationPace;

out vec3 fragPos;
out vec2 texCoordVert;
flat out int instance_id;

// --- Particle Motion Function function will be replaced FROM here ---
vec3 particleMap(out vec2 scale){
   scale = vec2(1,1);
   return vec3(0,0,0);
}
// --- Particle Motion Function function will be replaced TO here ---

void main()
{
   startTime;animationPace;
   instance_id = gl_InstanceID;
   vec2 scale;
   vec3 pm = particleMap(scale);
   //scale = scale * vec2(model[0][0], model[1][1]);
   vec3 scaledPos = aPos * vec3(scale,1);
   mat3 inverseRotation = inverse(mat3(view) * mat3(model));
   inverseRotation[0] = normalize(inverseRotation[0]);
   inverseRotation[1] = normalize(inverseRotation[1]);
   inverseRotation[2] = normalize(inverseRotation[2]);
   vec3 rotatedPosToCamera = inverseRotation * scaledPos;
   fragPos = vec3(model * vec4(rotatedPosToCamera, 1.0));
   fragPos += mat3(model) * pm;
   gl_Position = projection * view * vec4(fragPos, 1.0);
   texCoordVert = texCoord;

}


