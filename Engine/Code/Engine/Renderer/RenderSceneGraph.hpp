#pragma once
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"
#include <vector>


class RenderSceneGraph {
	friend class ForwardRenderPath;

public:
	RenderSceneGraph() {};

	void AddRenderable( Renderable* r);
	void AddCamera( Camera* c );
	void AddLight( Light* l );
	void AddParticleEmitter( ParticleEmitter* p );
	void RemoveRenderable( Renderable* r);
	void RemoveCamera( Camera* c );
	void RemoveLight( Light* l );
	void RemoveParticleEmitter( ParticleEmitter* p );

	unsigned int GetRenderableCount() const;
	unsigned int GetLightCount() const;
	unsigned int GetCameraCount() const;

	void SortCameras();

private:
	std::vector<Renderable*> m_renderables;
	std::vector<Light*> m_lights;
	std::vector<Camera*> m_cameras;
	std::vector<ParticleEmitter*> m_particleEmitters;
};