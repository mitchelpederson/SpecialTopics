#version 420 core

uniform mat4 VIEW;
uniform float MAX_FOG_DISTANCE;
uniform float FOG_FACTOR;
uniform vec4 FOG_COLOR;

layout(binding = 0) uniform samplerCube skybox;

in vec3 texCoords;
in vec4 passWorldPos;

out vec4 outColor;


void main() {
	vec4 viewPosition = VIEW * passWorldPos;
	float fog = smoothstep( 0.0, 0.5, viewPosition.z);
	float fogFactor = (FOG_FACTOR * 0.5) * fog;
	vec4 color = texture(skybox, texCoords);

	outColor = mix(color, FOG_COLOR, fogFactor);
}
