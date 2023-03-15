#version 400

// INPUT VARIABLES
uniform vec4 color;

// OUTPUT VARIABLES
out vec4 outputColor;

// Pixel Shader
void main(void)
{
  outputColor = color;
}