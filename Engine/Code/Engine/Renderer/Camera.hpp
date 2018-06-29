#pragma once


#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Core/Transform.hpp"

class Camera {

public:

	Camera();
	~Camera();

	virtual void Update();

	// will be implemented later
	void SetColorTarget( Texture* colorTarget );
	void SetDepthStencilTarget( Texture* depthTarget );
	void SetFrameBuffer( FrameBuffer* frameBuffer );

	void SetSkybox( CubeMap* skyboxTexture );

	// model setters
	void LookAt( const Vector3& position, const Vector3& target, const Vector3& up = Vector3::UP ); 
	void Translate( const Vector3& position );

	// projection settings
	void SetProjection( Matrix44 proj ); 
	void SetProjectionOrtho( float size, float near, float far ); 

	Vector3 GetForward() const;
	Vector3 GetRight() const;
	Vector3 GetUp() const;
	Matrix44 GetViewProjection() const;

	void Finalize();

public:
	// default all to identiy
	Matrix44 m_cameraMatrix;  // where is the camera?
	Matrix44 m_viewMatrix;    // inverse of camera (used for shader)
	Matrix44 m_projMatrix;    // projection

	Transform transform;
	FrameBuffer* m_frameBuffer = nullptr;

	Renderable* skybox = nullptr;

	std::vector<Material*> cameraEffects;
};