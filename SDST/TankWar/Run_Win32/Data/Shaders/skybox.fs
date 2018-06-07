#version 420 core

layout(binding = 0) uniform samplerCube skybox;

in vec3 texCoords;

out vec4 outColor;


void main() {

	outColor = texture(skybox, texCoords);
}
