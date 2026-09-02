
#define PI 3.1415926535897932384626433832795
//#define GET_TIME mod(float(time-startTime) * 0.5 * animationPace, 40.)
#define GET_TIME float(time-startTime) * 0.5 * animationPace

// Motion.
#ifdef VERTEX_SHADER
vec3 particleMap(out vec2 scale){
   float tt = GET_TIME;
   //

   //
   float ss = 0.5 + tt * 0.5;
   scale = vec2(ss);
   vec3 direction = random;
   direction = mod(direction, 1.);
   direction = 0.5 - direction;
   return direction * tt * 0.1;
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
   //
   vec4 color = texture(textureSampler, uv);

   //
   //color = color.bgra;

   float ss = 0.5 + tt * 0.5;
   color.a *= 1.0 / (ss * ss);

   //float ff = 1. - 1./(tt*tt*0.003 + 1.);
   //color.rgb = mix(color.rgb, color.bgr, ff*3.);
   //color.rgb = mix(color.rgb, 1.-color.rgb, ff*ff);
   color.xyz = vec3(color.r + color.g + color.b) / 3.;

   return color;
}
#endif // FRAGMENT_SHADER

