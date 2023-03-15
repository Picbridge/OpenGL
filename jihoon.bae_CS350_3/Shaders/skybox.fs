#version 400

// INPUT VARIABLES
in vec3 texCoords;

// OUTPUT VARIABLES
out vec4 outputColor;

// UNIFORM
uniform samplerCube cubemapTexture;

// Pixel Shader
void main(void)
{
  vec3 tex = texCoords;
  tex.x *= -1;
  outputColor = texture(cubemapTexture, tex);
}