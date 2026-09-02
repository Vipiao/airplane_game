
#define PI 3.1415926535897932384626433832795

void getLifeData(
   out float lifeTime, out int lifeNum, out float maxLifeTime, out float randomOffset
){
   maxLifeTime = 20.;
   randomOffset = float(instance_id) * 123.4567;
   //lifeTime = mod(float(time) + randomOffset, maxLifeTime);
   float offset = float(time) + float(instance_id) / float(numParticles) * maxLifeTime;
   lifeNum = int(floor(offset/maxLifeTime));
   lifeTime = mod(offset, maxLifeTime);
}

// Motion.
#ifdef VERTEX_SHADER
vec3 particleMap(out vec2 scale){
   //
   float lifeTime, maxLifeTime, randomOffset;
   int lifeNum;
   getLifeData(lifeTime, lifeNum, maxLifeTime, randomOffset);

   //
   float tt = lifeTime / maxLifeTime;
   float ss = 1. - tt * tt;
   scale = vec2(ss,ss);
   //float tt = mod(float(time) + float(instance_id) / float(numParticles) * maxLifeTime, maxLifeTime);
   return vec3(0,lifeTime*0.1,0);
}
#endif // VERTEX_SHADER

// Color.
#ifdef FRAGMENT_SHADER
uniform sampler2D textureSampler;

vec2 rotate(vec2 pos, float angle){
   float modAngle = mod(angle, 2.*PI);
   float cc = cos(modAngle);
   float ss = sin(modAngle);
   return vec2(
      pos.x * cc - pos.y * ss,
      pos.y * cc + pos.x * ss
   );
}

vec4 colorChooser(vec3 p, vec2 uv){
   //
   float lifeTime, maxLifeTime, randomOffset;
   int lifeNum;
   getLifeData(lifeTime, lifeNum, maxLifeTime, randomOffset);

   // Get texture.
   //randomOffset += floor(time / maxLifeTime) * maxLifeTime * 12.3456;
   float angle = randomOffset + float(lifeNum);
   vec2 rotatedUV = rotate(uv - vec2(0.5,0.5), angle);
   rotatedUV += vec2(0.5,0.5);
   rotatedUV = clamp(rotatedUV, vec2(0,0), vec2(1,1));
   vec4 color = texture(textureSampler, rotatedUV);

   // Re-color.
   float tt = lifeTime / maxLifeTime;
   color = mix(color, color.bgra, tt);
   //color.rgb = mix(color.rgb, 1.-color.rgb, tt*tt);
   color.a = mix(color.a, 0., tt);
   color.a = mix(color.a, 0., pow(1.-tt, 8.));
   //color.a *= 2.;

   //color.w *= 0.125;
   //color = color.bgba;

   //color.xyz = vec3(lifeTime / maxLifeTime);
   return color;
}
#endif // FRAGMENT_SHADER

