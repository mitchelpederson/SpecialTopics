#version 420 core

uniform mat4 VIEW;
uniform mat4 PROJECTION;

in vec3 POSITION;

out vec3 texCoords;

void main() {

	mat3 noTranslationView = mat3(VIEW);
	texCoords = POSITION;
	gl_Position = PROJECTION * vec4( noTranslationView * POSITION, 1);
}