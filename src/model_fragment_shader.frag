
#version 410 core

#define RENDER_MODE_SELF_ILLUMINATION 1
#define RENDER_MODE_USE_TEXTURE 2

out vec4 FragColor;

in vec3 fragPos;
in vec3 normalFrag;
in vec2 texCoordFrag;

uniform sampler2D textureSampler;
uniform int renderMode;
uniform int time;
uniform vec3 color;
uniform vec3 camPos;

float mod289(float x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 mod289(vec4 x){return x - floor(x * (1.0 / 289.0)) * 289.0;}
vec4 perm(vec4 x){return mod289(((x * 34.0) + 1.0) * x);}

void main()
{

   vec3 light;
   vec3 normal = normalize(normalFrag);
   vec3 viewDir = normalize(fragPos - camPos);
   
   if((renderMode & RENDER_MODE_SELF_ILLUMINATION) == RENDER_MODE_SELF_ILLUMINATION){
      light = vec3(1,1,1);
   }else{
      // ambient
      float ambientStrength = 0.3;
      vec3 lightColor = vec3(1,1,1);
      vec3 ambient = ambientStrength * lightColor;
      
      // diffuse
      //vec3 lightPos = vec3(4,4,4);
      //vec3 lightDir = normalize(lightPos - fragPos);
      vec3 lightDir = normalize(vec3(-1,-1,-1));
      float diff = max(dot(normal, -lightDir), 0.0);
      vec3 diffuse = diff * lightColor;
      
      // specular
      vec3 reflectDir = reflect(lightDir, normal);
      float spec = pow(max(dot(-viewDir, reflectDir), 0.0), 8.0);
      vec3 specular = lightColor * (spec * 1.0);
      
      // Combine.
      light = (ambient + (diffuse + specular));
   }
   
   //
   vec4 objectColor;
   if((renderMode & RENDER_MODE_USE_TEXTURE) == RENDER_MODE_USE_TEXTURE){
      objectColor = texture(textureSampler, texCoordFrag);
      float alpha = objectColor.w;
      if(alpha < 0.0001) {
        discard;
      }
      objectColor.w = alpha;
   }else{
      objectColor = vec4(color, 1.);
   }
   vec4 result = vec4(light, 1.) * objectColor;
   //float noise = noise(fragPos.xyz);
   //result = vec4(noise, noise, noise, 1.);

   FragColor = vec4(result);

   //FragColor = vec4(texture(textureSampler, fragPos.xy).xyz, 1.);
}





