#version 420 core

#define MAX_LIGHTS 8

uniform mat4 MODEL;
uniform mat4 VIEW;
uniform mat4 PROJECTION;
uniform vec3 UP;

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

in vec3 POSITION;
in vec4 COLOR;
in vec2 UV;
in vec3 NORMAL;
in vec3 TANGENT;

out vec2 passUV;
out vec3 passNormal;
out vec3 passTangent;
out vec3 passBitangent;
out vec4 passColor;
out vec4 passWorldPos;
out mat4 passView;


void main (void) {

	vec4 localPosition = vec4(POSITION, 1);

	passUV = UV;
	passColor = COLOR;
	passNormal = NORMAL;
	passTangent = TANGENT;
	passBitangent = cross(TANGENT, NORMAL);
	passWorldPos = MODEL * localPosition;
	passView = VIEW;

	gl_Position = PROJECTION * VIEW * MODEL * localPosition;

}