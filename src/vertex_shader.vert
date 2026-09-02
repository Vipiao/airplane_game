#version 410 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in float aRadius;
layout (location = 2) in float aTotalRadius;
layout (location = 3) in float aLevels;

out vec3 vertPos;
out float vertRadius;
out float vertTotalRadius;
out float vertLevels;

void main() {
  vertRadius = aRadius;
  vertTotalRadius = aTotalRadius;
  vertLevels = aLevels;
  vertPos = aPos;
}