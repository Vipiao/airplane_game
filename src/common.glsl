//precision highp float;

#define PI 3.1415926535897932384626433832795

uniform sampler2D textureSampler;
uniform sampler2D textureSampler2;

vec3 debug = vec3(0);

float smoothReLU(float xx, float hh){
  return (sqrt(xx * xx + 4 * hh) + xx) * 0.5 - 0.01;
}

float getDetailLevel(float distance, float minimum, float maximum){
  float ss = 0.23;
  float detailLevelScaled = -log2(distance) / log2(64.) - log2(ss);
  return clamp(detailLevelScaled, minimum, maximum);
}

//int intMod(ivec2 aa, int bb){
//  aa = aa - aa / bb * bb;
//  if (aa.x < 0) aa.x += bb;
//  if (aa.y < 0) aa.y += bb;
//  return aa;
//}

float noise(vec2 pos, vec2 offset){
  //debug += mod(pos.x, 1.);
  float textureSize = float(textureSize(textureSampler2, 0).x);
  pos = pos + mod(offset * float(offsetScale), textureSize);
  //pos = pos + offset * float(offsetScale);
  vec2 ff = pos + 0.5;
  pos = round(pos - 0.5) + 0.5;
  ff -= pos;

  vec4 texel = texture(textureSampler, (pos.xy)/textureSize);
  //vec4 texel = textureLod(textureSampler, (pos.xy)/textureSize, 1);
  float result = mix(
    mix(texel.x, texel.y, ff.x),
    mix(texel.z, texel.w, ff.x),
  ff.y);

  return result;
}

vec2 noisePartialDerivative(vec2 pos, vec2 offset, float lodOffset){
  float textureSize = float(textureSize(textureSampler2, 0).x);
  vec2 samplePos = (pos.xy + mod(offset.xy*float(offsetScale), textureSize))/textureSize + 1./textureSize/2.;
  //vec2 samplePos = (pos.xy + offset.xy*float(offsetScale))/textureSize + 1./textureSize/2.;
  
  #ifdef FRAGMENT_SHADER
  float lod = textureQueryLod(textureSampler2, samplePos).x;
  vec3 nn = textureLod(textureSampler2, samplePos.xy, lod - lodOffset).xyz;
  #else
  vec3 nn = texture(textureSampler2, samplePos.xy).xyz;
  #endif // FRAGMENT_SHADER

  //vec3 nn = vec3(sin(pos.x));
  nn = (nn - 0.5) * 2.;
  nn *= 0.001 / nn.z;
  //nn.z = 1.;
  return -nn.xy;
}

float layeredNoise(vec2 pos, vec2 offset, int numLayers){
  float ff = 1.;
  float result = 0.;
  for (int ii = 0; ii < numLayers; ii++) {
    result += noise(pos.xy * ff + ff * 0.5, offset * ff) / ff;
    ff *= 64.;
  }
  return result;
}

vec2 layeredNoisePartialDerivative(vec2 pos, vec2 offset, int numLayers){
  float ff = 1.;
  vec2 result = vec2(0.);
  for (int ii = 0; ii < numLayers; ii++) {
    float lodOffset = -0.;
    result += noisePartialDerivative(pos.xy * ff + ff * 0.5, offset * ff, lodOffset);
    ff *= 64.;
  }
  return result;
}

//float FF = 100.;
//float SS = 0.3;
vec3 map(vec3 pos, vec3 offset, int detailLevel){
  float elevation = layeredNoise(pos.xy*0.0122, offset.xy*0.0122, detailLevel);
  elevation = mix(
    sin((elevation - 0.5) * 32.),
    elevation * 16.,
    0.25
  ) * 800.;
  elevation += (
   sin(pos.x * 0.0000305 + mod(offset.x * float(offsetScale) * 0.0000305, 2.*PI)) +
   sin(pos.y * 0.0000305 + mod(offset.y * float(offsetScale) * 0.0000305, 2.*PI))
  ) * 3276.8;
  if(elevation < 0.){
   elevation = 0.0;
  }

  vec3 result = vec3(pos.xy, elevation);

  return result;
}

vec3 mapNormal(in vec3 pos, in vec3 offset, int detailLevel, float dx){
  
  vec2 nn = layeredNoisePartialDerivative(pos.xy*0.0122, offset.xy*0.0122, detailLevel) * 0.0122;
  float elevation = layeredNoise(pos.xy*0.0122, offset.xy*0.0122, detailLevel);
  nn = mix(
    cos((elevation - 0.5) * 32.) * nn * 32.,
    nn * 16.,
    0.25
  ) * 800.;
  nn += cos(pos.xy * 0.0000305 + mod(offset.xy * float(offsetScale) * 0.0000305, 2.*PI)) * 0.0000305 * 3276.8;

  return normalize(vec3(-nn, 1.));
}

//vec3 mapNormal(in vec3 pos, in vec3 offset, int detailLevel, float dx){
//  dx = length(camPos - pos) * 0.01;
//  vec2 e = vec2(dx, 0.);
//  float mapPos = map(pos, offset, detailLevel).z;
//  return normalize(vec3(
//      -(map(pos + e.xyy, offset, detailLevel).z - mapPos) / dx,
//      -(map(pos + e.yxy, offset, detailLevel).z - mapPos) / dx,
//      1.
//  ));
//}

//vec3 mapNormal(in vec3 pos, in vec3 offset, int detailLevel){
//  return mapNormal(pos, offset, detailLevel, 0.0001);
//}

