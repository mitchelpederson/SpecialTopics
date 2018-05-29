#pragma once


#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Math/Matrix44.hpp"


class Renderable {

public:
	Renderable(Mesh* mesh = nullptr, Material* mat = nullptr);
	~Renderable();

	void SetModelMatrix( const Matrix44& model );
	void SetMesh( Mesh* mesh );
	void SetMaterial( Material* material );

	Material* GetEditableMaterial();
	Material* GetMaterial();
	Mesh* GetMesh();
	const Matrix44& GetModelMatrix();
	Vector3 GetPosition();

private:
	Matrix44 m_modelMatrix;
	Mesh* m_mesh;
	Material* m_sharedMaterial;
	Material* m_instanceMaterial = nullptr;
};