#version 410 core
//#extension GL_ARB_gpu_shader_fp64 : require

#define FRAGMENT_SHADER

out vec4 FragColor;

in vec3 tePos;
in vec3 teDebug;

vec3 fragDebug;

uniform mat4 view;
uniform mat4 proj;
uniform vec3 camPos;
uniform vec3 camPosOffset;
uniform float fov;
uniform uint time;
uniform uint offsetScale;

// --- Procedural generation map function will be replaced FROM here ---

// --- Procedural generation map function will be replaced TO here ---

void main() {
  fragDebug = teDebug;
  vec3 mm = map(tePos, camPosOffset, 3);
  
  //
  vec3 viewDir = normalize(vec3(camPos) - tePos);
  
  ////
  vec3 lightDir = normalize(vec3(1,1,1));
  vec3 lightColor = vec3(1,1,1);
  vec3 normal = mapNormal(tePos, camPosOffset, 3, 0.0001*0.1);
  vec3 normalSmooth = mapNormal(tePos, camPosOffset, 1, 0.0001*0.1);
  
  float ambientFactor = 0.2;
  float diffuseFactor = 0.8;
  float specularFactor = 0.05;
  float specularShininess = 16.;

  // Water.
  bool isWater = false;
  float waterLevel = mix(mm.z, tePos.z, pow(1. - abs(viewDir.z), 8.));
  if(waterLevel < 0.032768){
    isWater = true;
    specularFactor = 1.;
    specularShininess = 8.;
    diffuseFactor = 0.2;
    ambientFactor = 0.8;
    
    normal = vec3(0,0,1);
    const int size = 5;
    vec3 vecs[size] = vec3[](
        vec3(-20.,8., 8.0),
        vec3(40.,8., 4.0),
        vec3(16.,80., 2.0),
        vec3(-160.,-160.+40., 1.0),
        vec3(320.-120.,-320., 0.5)
    );
    float waveHeight = 0.;
    float tt = float(time) * 0.0001;
    float ss = PI * 2. / float(offsetScale);
    for(int i=0; i<size; i++) {
      // The speed of waves on water is independent on amplitude or wavelength.
      //waveHeight += sin(pos.x*vecs[i].x+pos.z*vecs[i].y+time*length(vecs[i].xy))*vecs[i].z;
      float freq = length(vecs[i].xy);
      normal.xy += cos(
          tePos.x*vecs[i].x * ss +
          tePos.y*vecs[i].y * ss +
          tt * freq +
          normalSmooth.z * 1. * freq
        ) * vecs[i].xy * ss * vecs[i].z *
        1./(1. + dot(tePos - camPos, tePos - camPos) * 0.0001 / vecs[i].z);
    }
    normal = normalize(vec3(-normal.xy, 1.));
  }

  // Ambient.
  vec3 ambient = ambientFactor * lightColor;

  // Diffuse.
  vec3 diff = diffuseFactor * max(dot(lightDir, normal), 0.) * lightColor;
  if(!isWater){
    diff *= pow(max(dot(lightDir, normalSmooth), 0.), 0.25);
  }

  // Specular.
  //vec3 halfwayDir = normalize(lightDir + viewDir);
  //float spec = pow(max(dot(normal, halfwayDir), 0.0), specularShininess);
  vec3 reflectDir = reflect(-lightDir, normal);
  float spec = pow(max(dot(viewDir, reflectDir), 0.0), specularShininess);
  //vec3 specular = lightColor * (spec * 1.0);
  vec3 specular = specularFactor * spec * lightColor;
  
  //
  vec3 color = vec3(0.3);

  // Grass.
  vec3 grassColor = vec3(100,140,40)/255.;
  vec3 grassColorDark = vec3(95,100,30)/255.;
  grassColor = mix(
   grassColor,
   grassColorDark,
   (sin(tePos.x * PI * 2. / float(offsetScale)) + sin(tePos.y * PI * 2. / float(offsetScale)) + 2.) * 0.25
  );
  color = mix(color, grassColor, min(1., pow(normal.z+0.2, 8.)));

  // Sand.
  vec3 sandColor = vec3(194,178,128)/255.;
  float sandFactor = min(pow(mm.z * 0.0078125, 2.), 1.);
  color = mix(color, sandColor,
    min(1., pow(normal.z+0.3-(sandFactor*0.9)*0.5, 32.))
  );

  // Snow.
  float snowFactor = min(pow(mm.z * 0.00030517578, 16.), 1.);
  color = mix(color, vec3(0.8),
    min(1., pow(normal.z+0.3-(1.-snowFactor*0.9)*0.5, 32.))
  );

  // Water.
  if(isWater){
    //color = vec3(70,70,170)/255.;
    color = vec3(70,70,140)/255.;
    //diff = lightColor;
  }

  color = color * (ambient + diff + specular);
  
  // Fog.
  float fogFactor = 1. - pow(0.9999, dot(vec3(camPos) - tePos, viewDir));
  color = mix(color, vec3(0.6, 0.7, 0.8), fogFactor);

  //color = vec3(0);
  //color = vec3(normal.b);
  //color.x += fragDebug.x;
  //color.x = debug.x;

  FragColor = vec4(color.xyz, 1.0f);
}


