#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/RenderSceneGraph.hpp"

struct DrawCall {

	Matrix44 m_model;
	Mesh* m_mesh;
	Material* m_material;

	unsigned int m_lightCount;
	unsigned int m_lightIndices[MAX_LIGHTS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	int m_layer;
	int m_queue;

};


class ForwardRenderPath {

public:

	ForwardRenderPath( Renderer* r ) : renderer( r ) {};

	void Render( RenderSceneGraph* scene );
	void RenderSceneForCamera( Camera* camera, RenderSceneGraph* scene );
	void ClearBasedOnCameraOptions( Camera* camera );
	 
private:
	void ComputeMostContributingLights( unsigned int* m_lightCount, unsigned int m_lightIndices[MAX_LIGHTS], const Vector3& position, RenderSceneGraph* scene );
	void SortDrawCalls( std::vector<DrawCall>& drawCalls, Camera* camera );
	void EnableLightsForDrawCall( const DrawCall& drawCall, RenderSceneGraph* scene );
	void ApplyCameraEffects( Camera* camera );

	Renderer* renderer;

	Texture* m_effectScratchTarget = nullptr;
	Texture* m_effectCurrentTarget = nullptr;
	Camera* m_effectCamera = nullptr;

};