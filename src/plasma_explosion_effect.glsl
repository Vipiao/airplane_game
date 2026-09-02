
#define PI 3.1415926535897932384626433832795
//#define GET_TIME mod(float(time-startTime) * 0.5 * animationPace, 40.)
#define GET_TIME float(time-startTime) * 0.5 * animationPace

// Motion.
#ifdef VERTEX_SHADER
vec3 particleMap(out vec2 scale){
   float tt = GET_TIME;
   //

   //
   float ss = 0.5 + tt * 0.2;
   scale = vec2(ss);
   vec3 direction = random;
   direction = mod(direction, 1.);
   direction = 0.5 - direction;
   return direction * tt * 0.5;
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
   float tt = GET_TIME;

   vec3 camRight = vec3(view[0][0],view[1][0],view[2][0]);
   vec3 camUp = vec3(view[0][1],view[1][1],view[2][1]);
   vec3 camForward = vec3(view[0][2],view[1][2],view[2][2]);

   // Calculate the roll angle from the view matrix
   float rollAngle = atan(camUp.z, camRight.z);

   float angle = float(instance_id) - rollAngle;
   vec2 rotatedUV = rotate(uv - vec2(0.5,0.5), angle);
   rotatedUV += vec2(0.5,0.5);
   rotatedUV = clamp(rotatedUV, vec2(0,0), vec2(1,1));
   vec4 color = texture(textureSampler, rotatedUV);
   color = mix(color, texture(textureSampler, 1.-rotatedUV), sin(tt*1.0)*0.5+0.5);

   float factor = abs(camForward.z);
   color.rgb = mix(color.rgb, vec3(170,178,254)/255., factor);

   //
   float ss = 0.5 + tt * 0.2;
   color.a *= 1. / (ss * ss);

   float ff = 1. - 1./(tt*tt*0.003 + 1.);
   color.rgb = mix(color.rgb, color.bgr, ff*3.);
   color.rgb = mix(color.rgb, 1.-color.rgb, ff*ff);

   return color;
}
#endif // FRAGMENT_SHADER

