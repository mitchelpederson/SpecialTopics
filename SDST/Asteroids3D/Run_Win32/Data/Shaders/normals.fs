#version 420 core

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

layout(binding = 0) uniform sampler2D gTexDiffuse;

in vec2 passUV;
in vec3 passNormal;
in vec4 passColor;


out vec4 outColor;

void main(void) {

	vec3 modelNormal = (normalize(passNormal) + vec3(1)) * 0.5;
	outColor = vec4(modelNormal, 1);
}