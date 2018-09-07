#version 420 core

#define MAX_LIGHTS 8

uniform mat4 MODEL;
uniform mat4 VIEW;

uniform vec3 EYE_POSITION;

uniform vec4 AMBIENT_COLOR;
uniform float AMBIENT_INTENSITY;

uniform vec3 LIGHT_POSITION[MAX_LIGHTS];
uniform vec4 LIGHT_COLOR[MAX_LIGHTS];
uniform float LIGHT_INTENSITY[MAX_LIGHTS];
uniform float LIGHT_ATTENUATION[MAX_LIGHTS];

uniform float SPECULAR_POWER;
uniform float SPECULAR_AMOUNT;

layout(binding = 0) uniform sampler2D gTexDiffuse;
layout(binding = 1) uniform sampler2D gTexNormal;

in vec2 passUV;
in vec3 passNormal;
in vec3 passTangent;
in vec4 passColor;
in vec4 passWorldPos;


vec3 CalculateDot3Lighting( vec3 lightPos, vec3 lightColor, float lightIntensity, float lightAttenuation, vec4 worldPos, vec3 surfaceNormal) {

	float lightDistance = length(lightPos - worldPos.xyz);
	vec3 lightDirection = normalize( lightPos - worldPos.xyz );
	float dot3 = dot( lightDirection, surfaceNormal );
	vec3 lightContribution = max(dot3, 0.0f) * lightColor * lightIntensity * (1.f / (1.f + lightAttenuation * lightDistance));

	return lightContribution;
}


vec3 CalculateSpecularLighting( vec3 lightPos, vec3 lightColor, float lightIntensity, float lightAttenuation, vec3 eyePos, vec4 worldPos, vec3 surfaceNormal ) {

	vec3 eyeDirection = normalize( EYE_POSITION - worldPos.xyz );
	vec3 lightDirection = normalize( lightPos - worldPos.xyz );
	float lightDistance = length(lightPos - worldPos.xyz);

	// Get the direction of the reflected light and how well it aligns 
	// with the direction of the camera
	vec3 reflectedLightDirection = reflect( -lightDirection, normalize(surfaceNormal) );
	float specularFactor = max( 0, dot( eyeDirection, reflectedLightDirection ) );

	// apply the specular factor to the specular model equation
	specularFactor = SPECULAR_AMOUNT * pow( specularFactor, SPECULAR_POWER );
	vec3 reflectedLight = lightColor.rgb * specularFactor * lightIntensity * (1.f / (1.f + lightAttenuation * lightDistance));
	return reflectedLight;
}


out vec4 outColor;

void main(void) {

	// Get the normal and the diffuse color
	vec3 surfaceNormal = vec4( texture(gTexNormal, passUV).xyz, 0 ).xyz;
	vec4 surfaceColor = texture(gTexDiffuse, passUV);
	vec3 bitangent = cross(passTangent, passNormal);
	outColor = vec4((bitangent + vec3(1.f)) * 0.5f, 1);//vec4((passTangent + vec3(1.f)) * 0.5f, 1);
	return;


	// derived from a reference to my class notes and this forum post:
	// https://www.opengl.org/discussion_boards/showthread.php/162857-Computing-the-tangent-space-in-the-fragment-shader
	vec2 verticalDeltaUV = dFdy(passUV);
	vec2 horizontalDeltaUV = dFdx(passUV);
	vec3 horizontalDeltaPos = dFdx(passWorldPos.xyz);
	vec3 verticalDeltaPos = dFdy(passWorldPos.xyz);

	vec3 surfaceTangent = normalize( horizontalDeltaPos * verticalDeltaUV.t - verticalDeltaPos * horizontalDeltaUV.t );
	outColor = vec4(surfaceTangent, 1);
	return;
	
	vec3 surfaceBitangent = normalize( -horizontalDeltaPos * verticalDeltaUV.s + verticalDeltaPos * horizontalDeltaUV.s);
	vec3 newNormal = normalize(cross(surfaceBitangent, surfaceTangent));

	mat4 tbn = mat4(vec4(surfaceTangent, 0), vec4(surfaceBitangent, 0), vec4(newNormal, 0), vec4(0,0,0,1));
	surfaceNormal = normalize((tbn * vec4(surfaceNormal, 0)).xyz);

	// calculate ambient light
	vec3 surfaceLight = AMBIENT_COLOR.rgb * AMBIENT_INTENSITY;

	// Get how aligned to the light the surface is and 
	// add it to the total surface light
	// then clamp it so the color doesn't blow out the surface
	//float dot3 = dot(lightDirection, surfaceNormal);
	//vec3 lightColor = max(dot3, 0.0f) * LIGHT_COLOR[0].rgb;
	//surfaceLight += lightColor;

	vec3 reflectedLight = vec3(0);

	for (int i = 0; i < MAX_LIGHTS; i++) {
		surfaceLight += CalculateDot3Lighting(LIGHT_POSITION[i], LIGHT_COLOR[i].rgb, LIGHT_INTENSITY[i], LIGHT_ATTENUATION[i], passWorldPos, surfaceNormal);

		reflectedLight += CalculateSpecularLighting( LIGHT_POSITION[i], LIGHT_COLOR[i].rgb, LIGHT_INTENSITY[i], LIGHT_ATTENUATION[i], EYE_POSITION, passWorldPos, surfaceNormal );
	}
	surfaceLight = clamp(surfaceLight, vec3(0), vec3(1));
	
	vec4 finalColor = vec4(surfaceLight, 1) * surfaceColor + vec4(reflectedLight, 0);

	//vec3 colorRgb = diffuse.rgb * passColor.rgb * AMBIENT_COLOR.rgb * AMBIENT_INTENSITY;
	outColor = finalColor;
}