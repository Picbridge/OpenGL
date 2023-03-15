#version 400

uniform LightCount
 {
   int Dir;
   int Point;
   int Spot;
   int Total;
 } lightcount;

struct LightStruct
{
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;

    vec3 position;
    vec3 direction;
    vec3 attenuation;

    float innerangle;
    float outerangle;
    float spotfalloff;
};
 
uniform Lights
{
    LightStruct light[8];
} lights;

// INPUT VARIABLES
in vec3 color;
in vec2 texCoords;
in vec3 normalW;
in vec3 tangentW;
in vec3 binormalW;
in vec3 posW;

// OUTPUT VARIABLES
out vec4 outputColor;

// UNIFORM VARIABLES
uniform vec4 globalambient;
uniform vec4 fogcolor;
uniform float camNear;
uniform float camFar;
uniform float width;
uniform float height;
uniform vec3 camPos;
uniform int option;
uniform float normalMapScalar;

uniform sampler2D diffuseMap;
uniform sampler2D normalMap;

uniform int enableCubeMap;
uniform float reflectivity;
uniform float refractindex;
uniform sampler2D cubemap_top;
uniform sampler2D cubemap_bot;
uniform sampler2D cubemap_left;
uniform sampler2D cubemap_right;
uniform sampler2D cubemap_front;
uniform sampler2D cubemap_back;

vec4 ComputeDirectionalLight(LightStruct L, vec4 matDiff, vec4 matSpec, vec3 norm, vec3 toEye)
{
  // Initialize outputs.
  vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 spec    = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  // The light vector aims opposite the direction the light rays travel.
  vec3 lightVec = -L.direction;
  lightVec = normalize(lightVec);

  // Ambient
  ambient = L.ambient;

  // Diffuse  
  float diffuseFactor = dot(lightVec, norm);

  if(diffuseFactor > 0.0f)
  {
    diffuse = diffuseFactor * matDiff * L.diffuse;

    vec3 reflectVec = 2 * diffuseFactor * norm - lightVec;
    // NORMALIZE REFLECTION VECTOR
    reflectVec = normalize(reflectVec);
    float specFactor = pow(max(dot(reflectVec, toEye), 0.0f), matSpec.r);   
    //spec = specFactor * matSpec * L.specular;
  }

  return (ambient + diffuse + spec);
}

vec4 ComputePointLight(LightStruct L, vec4 matDiff, vec4 matSpec, vec3 pos, vec3 norm, vec3 toEye)
{
  // Initialize outputs.
  vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 spec    = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  // Light vector
  vec3 lightVec = L.position - pos;
    
  // The distance from surface to light.
  float d = length(lightVec);
    
  // Normalize the light vector.
  lightVec /= d; 
  
  // Ambient
  ambient = L.ambient;
  
  // Diffuse
  float diffuseFactor = dot(lightVec, norm);
    
  // Specular
  if(diffuseFactor > 0.0f)
  {
    diffuse = diffuseFactor * matDiff * L.diffuse;

    vec3 reflectVec = 2 * diffuseFactor * norm - lightVec;
    // NORMALIZE REFLECTION VECTOR
    reflectVec = normalize(reflectVec);
    float specFactor = pow(max(dot(reflectVec, toEye), 0.0f), matSpec.r);   
    spec = specFactor * matSpec * L.specular;
  }
  
  // Attenuate
  float att = 1.0f / dot(L.attenuation, vec3(1.0f, d, d*d));
  att = min(att, 1.f);
  
  diffuse *= att;
  spec    *= att;
  
  return (ambient + diffuse + spec);
}

vec4 ComputeSpotLight(LightStruct L, vec4 matDiff, vec4 matSpec, vec3 pos, vec3 norm, vec3 toEye)
{
  // Initialize outputs.
  vec4 ambient = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 diffuse = vec4(0.0f, 0.0f, 0.0f, 0.0f);
  vec4 spec    = vec4(0.0f, 0.0f, 0.0f, 0.0f);

  // Light vector
  vec3 lightVec = L.position - pos;

  // The distance from surface to light.
  float d = length(lightVec);

  // Normalize the light vector.
  lightVec /= d; 

  // Direction vector
  vec3 dirVec = normalize(L.direction);

  float outAng = cos(radians(L.outerangle));
  float inAng = cos(radians(L.innerangle));
  float spot = (dot(-lightVec,dirVec) - outAng) / (inAng - outAng);
  spot = pow(max(spot, 0.f), L.spotfalloff);

  // Ambient
  ambient = L.ambient;
  
  // Diffuse
  float diffuseFactor = dot(lightVec, norm);
    
  // Specular
  if(diffuseFactor > 0.0f)
  {
    diffuse = diffuseFactor * matDiff * L.diffuse;

    vec3 reflectVec = 2 * diffuseFactor * norm - lightVec;
    // NORMALIZE REFLECTION VECTOR
    reflectVec = normalize(reflectVec);
    float specFactor = pow(max(dot(reflectVec, toEye), 0.0f), matSpec.r);   
    spec = specFactor * matSpec * L.specular;
  }
  
  // Attenuate
  float att = 1.0f / dot(L.attenuation, vec3(1.0f, d, d*d));
  att = min(att, 1.f);
  att *= spot;
  
  diffuse *= att;
  spec    *= att;

  return (ambient + diffuse + spec);
}

vec4 my_reflection(vec3 toEye, vec3 norm)
{
  vec4 reflection;
  vec2 tex;
  vec3 r = 2 * dot(toEye, norm) * norm - toEye;

  // X - Major axis
  if( abs(r.x) > abs(r.y) && abs(r.x) > abs(r.z) )
  {
    if(r.x > 0)
    {
      tex.x = (r.z / abs(r.x) + 1) * .5;
      tex.y = (-r.y / abs(r.x) + 1) * .5;
      reflection = texture(cubemap_left, tex);
    }
    else
    {
      tex.x = (-r.z / abs(r.x) + 1) * .5;
      tex.y = (-r.y / abs(r.x) + 1) * .5;
      reflection = texture(cubemap_right, tex);
    }
  }
  
  // Y - Major axis
  if( abs(r.y) > abs(r.x) && abs(r.y) > abs(r.z) )
  {
    if(r.y > 0)
    {
      tex.x = (r.x / abs(r.y) + 1) * .5;
      tex.y = (-r.z / abs(r.y) + 1) * .5;
      reflection = texture(cubemap_bot, tex);
    }
    else
    {
      tex.x = (-r.x / abs(r.y) + 1) * .5;
      tex.y = (-r.z / abs(r.y) + 1) * .5;
      reflection = texture(cubemap_top, tex);
    }
  }
  
  // Z - Major axis
  if( abs(r.z) > abs(r.y) && abs(r.z) > abs(r.x) )
  {
    if(r.z > 0)
    {
      tex.x = (-r.x / abs(r.z) + 1) * .5;
      tex.y = (-r.y / abs(r.z) + 1) * .5;
      reflection = texture(cubemap_front, tex);
    }
    else
    {
      tex.x = (r.x / abs(r.z) + 1) * .5;
      tex.y = (-r.y / abs(r.z) + 1) * .5;
      reflection = texture(cubemap_back, tex);
    }
  }

  return reflection;
}

vec4 my_refraction(vec3 toEye, vec3 norm, float eta)
{
  vec4 refraction;
  vec3 r;
  vec2 tex;

  float cosI = dot(toEye, norm);
  float cosT2 = 1.0f - eta * eta * (1.0f - cosI * cosI);
  if(cosT2 > 0)
    r = eta * toEye - ((eta * cosI + sqrt(cosT2)) * norm);
  else
    r = vec3(0,0,0);
 
  // X - Major axis
  if( abs(r.x) > abs(r.y) && abs(r.x) > abs(r.z) )
  {
    if(r.x > 0)
    {
      tex.x = (r.z / abs(r.x) + 1) * .5;
      tex.y = (r.y / abs(r.x) + 1) * .5;
      refraction = texture(cubemap_right, tex);
    }
    else
    {
      tex.x = (-r.z / abs(r.x) + 1) * .5;
      tex.y = (r.y / abs(r.x) + 1) * .5;
      refraction = texture(cubemap_left, tex);
    }
  }

  // Y - Major axis
  if( abs(r.y) > abs(r.x) && abs(r.y) > abs(r.z) )
  {
    if(r.y > 0)
    {
      tex.x = (r.x / abs(r.y) + 1) * .5;
      tex.y = (r.z / abs(r.y) + 1) * .5;
      refraction = texture(cubemap_top, tex);
    }
    else
    {
      tex.x = (-r.x / abs(r.y) + 1) * .5;
      tex.y = (r.z / abs(r.y) + 1) * .5;
      refraction = texture(cubemap_bot, tex);
    }
  }

  // Z - Major axis
  if( abs(r.z) > abs(r.y) && abs(r.z) > abs(r.x) )
  {
    if(r.z > 0)
    {
      tex.x = (-r.x / abs(r.z) + 1) * .5;
      tex.y = (r.y / abs(r.z) + 1) * .5;
      refraction = texture(cubemap_back, tex);
    }
    else
    {
      tex.x = (r.x / abs(r.z) + 1) * .5;
      tex.y = (r.y / abs(r.z) + 1) * .5;
      refraction = texture(cubemap_front, tex);
    }
  }

  return refraction;
}

// Pixel Shader
void main(void)
{
  // Render vert normals
  if(option == 1)
  {
    outputColor = vec4(normalW,1);
    return;
  }
  // Render tangent
  if(option == 3)
  {
    outputColor = vec4(tangentW,1);
    return;
  }
  // Render binormal
  if(option == 4)
  {
    outputColor = vec4(binormalW,1);
    return;
  }

  if(option == 6)
  {
    outputColor = vec4(color, 1.0f);
    return;
  }

  vec4 finalColor = vec4(0,0,0,0);
  
  vec4 texDiffColor, texSpecColor, reflect, refract;
  vec3 normal, bumpNormal, tangent, binormal;
  vec3 sVec = vec3(1,0,0);
  vec3 tVec = vec3(0,1,0);

  sVec.z = texture(normalMap, vec2(texCoords.x + 1/width, texCoords.y)).r 
           - texture(normalMap, vec2(texCoords.x - 1/width, texCoords.y)).r;
  tVec.z = texture(normalMap, vec2(texCoords.x, texCoords.y + 1/height)).r 
           - texture(normalMap, vec2(texCoords.x, texCoords.y - 1/height)).r;
  sVec.z *= normalMapScalar;
  sVec.z *= normalMapScalar;
  bumpNormal = normalize(cross(sVec, tVec));

  // Render normal map
  if(option == 2)
  {
    bumpNormal = bumpNormal *.5 + .5;
    outputColor = vec4(bumpNormal,1);
    return;
  }
  
  // Change normal range back to  -1 to 1
  normal = (normalW - 0.5f) * 2.0f;
  tangent = (tangentW - 0.5f) * 2.0f;
  binormal = (binormalW - 0.5f) * 2.0f;

  bumpNormal = (bumpNormal.x * tangent) + (bumpNormal.y * binormal) + (bumpNormal.z * normal);
  normal = normalize(bumpNormal);

  // Off normal map
  //if(option == 5)
  //{
    normal = (normalW - 0.5f) * 2.0f;
  //}

  // View vector
  vec3 toEyeW = camPos - posW;
  float z = length(toEyeW);
  toEyeW = normalize(toEyeW);

  texDiffColor = texture(diffuseMap, texCoords);
  texSpecColor = texture(normalMap, texCoords);

  //if(enableCubeMap == 0)
  //{
  //  outputColor = (globalambient + texDiffColor * texSpecColor);
  //  return;
  //}
  
  //reflect = my_reflection(-toEyeW, normal);
  //refract = my_refraction(-toEyeW, normal, 1 / refractindex);

  // Calculate directional lights
  for(int i = 0; i < lightcount.Dir; ++i)
  {
    finalColor += ComputeDirectionalLight(lights.light[i], texDiffColor, texSpecColor, normal, toEyeW);
  }
  // Calculate point lights
  for(int j = lightcount.Dir; j < lightcount.Dir + lightcount.Point; ++j)
  {
    finalColor += ComputePointLight(lights.light[j], texDiffColor, texSpecColor, posW, normal, toEyeW);
  }
  // Calculate spot lights
  for(int k = lightcount.Dir + lightcount.Point; k < lightcount.Total; ++k)
  {
    finalColor += ComputeSpotLight(lights.light[k], texDiffColor, texSpecColor, posW, normal, toEyeW);
  }

  // Total intensity
  finalColor = (globalambient + finalColor);// * vec4(color, 1.0f);

  //if(enableCubeMap == 1)
  //  finalColor += reflect * reflectivity + refract * (1 - reflectivity);

  // Atmospheric attenuation
  z = max(z, camNear);
  float s = (camFar - z) / (camFar - camNear);
  outputColor = finalColor * s + (1 - s) * fogcolor;
}