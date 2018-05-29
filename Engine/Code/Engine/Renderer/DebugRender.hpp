#pragma once
#include "Engine/Core/Rgba.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include <vector>

enum DebugRenderMode {
	DEBUG_RENDER_IGNORE_DEPTH,
	DEBUG_RENDER_USE_DEPTH,
	DEBUG_RENDER_HIDDEN,
	DEBUG_RENDER_XRAY
};


struct DebugRenderObject {

	DebugRenderObject() 
		: startColor(Rgba())
		, endColor(Rgba())
		, lifetime(0.f)
		, mode(DEBUG_RENDER_USE_DEPTH)
		, mesh(nullptr) {}

	DebugRenderObject( const Rgba& startColor, const Rgba& endColor, float lifetime, float spawnTime, DebugRenderMode mode, Mesh* mesh) 
		: startColor(startColor)
		, endColor(endColor)
		, lifetime(lifetime)
		, spawnTime(spawnTime)
		, mode(mode)
		, mesh(mesh) {}

	Rgba startColor;
	Rgba endColor;
	float lifetime;
	float spawnTime;
	DebugRenderMode mode;
	Mesh* mesh;
};

class DebugRenderState {

public:
	static Renderer* currentRenderer;
	static Camera* currentCamera;
	static std::vector<DebugRenderObject>* objects;
	static bool isActive;
};

void DebugRenderStartup( Renderer* renderer );
void DebugRenderShutdown();
void DebugRenderAndUpdate();
void DebugRenderSet3DCamera( Camera *camera ); 
void DebugRenderClear();
void DebugRenderToggle();

void DebugRenderPoint( float lifetime, 
	const Vector3& position, 
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH); 

void DebugRenderLineSegment( float lifetime, 
	const Vector3& p0, const Rgba& p0_color, 
	const Vector3& p1, const Rgba& p1_color, 
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH); 

void DebugRenderBasis( float lifetime, 
	const Matrix44& basis, 
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH);  

void DebugRenderWireSphere( float lifetime, 
	const Vector3& pos, 
	float  radius, 
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH); 
// May also want a DebugRenderWireSphere

void DebugRenderWireCube( float lifetime, 
	const Vector3& position, 
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH); 

void DebugRenderQuad( float lifetime, 
	const Vector3& pos, 
	const Vector3& right, float const x_min, float const x_max, 
	const Vector3& up, float const y_min, float const y_max, 
	Texture* texture,        // default to a white texture if nullptr
	const Rgba& start_color = Rgba(0, 255, 0, 255), 
	const Rgba& end_color = Rgba(255, 0, 0, 255),
	DebugRenderMode mode = DEBUG_RENDER_USE_DEPTH); 