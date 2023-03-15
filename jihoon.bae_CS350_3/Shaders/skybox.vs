#version 400

//Input variables
in vec3 inputPosition;

// Output
out vec3 texCoords;

// UNIFORM VARIABLES 
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;

// Vertex Shader
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	gl_Position = viewMatrix * gl_Position;
    gl_Position = projectionMatrix * gl_Position;
	gl_Position = gl_Position.xyww;

	texCoords = inputPosition;
}