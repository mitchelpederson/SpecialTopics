#include "Engine/Renderer/RenderSceneGraph.hpp"


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::AddRenderable( Renderable* r ) {
	m_renderables.push_back(r);
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::AddCamera( Camera* c ) {
	m_cameras.push_back(c);
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::AddLight( Light* l ) {
	m_lights.push_back(l);
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::AddParticleEmitter( ParticleEmitter* p ) {
	m_particleEmitters.push_back(p);
	m_renderables.push_back(p->renderable);
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::RemoveRenderable( Renderable* r) {
	std::vector<Renderable*>::iterator searchResult = std::find(m_renderables.begin(), m_renderables.end(), r);
	if (searchResult != m_renderables.end()) {
		unsigned int index = (unsigned int) (searchResult - m_renderables.begin());
		m_renderables[index] = m_renderables[ m_renderables.size() - 1 ];
		m_renderables.pop_back();
	}
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::RemoveCamera( Camera* c ) {
	std::vector<Camera*>::iterator searchResult = std::find(m_cameras.begin(), m_cameras.end(), c);
	if (searchResult != m_cameras.end()) {
		unsigned int index = (unsigned int) (searchResult - m_cameras.begin());
		m_cameras[index] = m_cameras[ m_cameras.size() - 1 ];
		m_cameras.pop_back();
	}
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::RemoveLight( Light* l ) {
	std::vector<Light*>::iterator searchResult = std::find(m_lights.begin(), m_lights.end(), l);
	if (searchResult != m_lights.end()) {
		unsigned int index = (unsigned int) (searchResult - m_lights.begin());
		m_lights[index] = m_lights[ m_lights.size() - 1 ];
		m_lights.pop_back();
	}
}


//----------------------------------------------------------------------------------------------------------------
// Nuance: remove the particle emitter's renderable too
void RenderSceneGraph::RemoveParticleEmitter( ParticleEmitter* l ) {
	std::vector<ParticleEmitter*>::iterator searchResult = std::find(m_particleEmitters.begin(), m_particleEmitters.end(), l);
	if (searchResult != m_particleEmitters.end()) {
		unsigned int index = (unsigned int) (searchResult - m_particleEmitters.begin());
		RemoveRenderable(m_particleEmitters[index]->renderable);
		m_particleEmitters[index] = m_particleEmitters[ m_particleEmitters.size() - 1 ];
		m_particleEmitters.pop_back();
	}
}


//----------------------------------------------------------------------------------------------------------------
void RenderSceneGraph::SortCameras() {

}


//----------------------------------------------------------------------------------------------------------------
unsigned int RenderSceneGraph::GetRenderableCount() const {
	return (unsigned int) m_renderables.size();
}


//----------------------------------------------------------------------------------------------------------------
unsigned int RenderSceneGraph::GetLightCount() const {
	return (unsigned int) m_lights.size();
}


//----------------------------------------------------------------------------------------------------------------
unsigned int RenderSceneGraph::GetCameraCount() const {
	return (unsigned int) m_cameras.size();
}