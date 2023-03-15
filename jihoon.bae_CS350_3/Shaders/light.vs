#version 400

//Input variables
in vec3 inputPosition;
in vec3 inputColor;
in vec3 inputNormal;
in vec3 inputTangent;
in vec3 inputBinormal;
in vec2 inputCoords;

// Output variables
out vec3 color;
out vec2 texCoords;
out vec3 normalW;
out vec3 tangentW;
out vec3 binormalW;
out vec3 posW;

// UNIFORM VARIABLES 
uniform mat4 worldMatrix;
uniform mat4 viewMatrix;
uniform mat4 projectionMatrix;
uniform mat4 worldInvTranspose;

// Vertex Shader
void main(void)
{
	// Calculate the position of the vertex against the world, view, and projection matrices.
	gl_Position = worldMatrix * vec4(inputPosition, 1.0f);
	posW = gl_Position.xyz;
	gl_Position = viewMatrix * gl_Position;
    gl_Position = projectionMatrix * gl_Position;

	// Store the input color for the pixel shader to use.
	color = inputColor;

	// Store pos to calculate u,v
	texCoords = inputCoords;

	// Transform normal
	normalW = mat3(worldInvTranspose) * inputNormal;
	normalW = normalize(normalW);
	normalW = normalW * 0.5f + 0.5f;

	// Transform tangent
	tangentW = mat3(worldInvTranspose) * inputTangent;
	tangentW = normalize(tangentW);
	tangentW = tangentW * 0.5f + 0.5f;

	// Transform binormal
	binormalW = mat3(worldInvTranspose) * inputBinormal;
	binormalW = normalize(binormalW);
	binormalW = binormalW * 0.5f + 0.5f;
}