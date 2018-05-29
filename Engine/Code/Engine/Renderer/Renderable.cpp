#include "Engine/Renderer/Renderable.h"

#define SAFE_DELETE(p) if (nullptr == (p)) { delete (p); (p) = nullptr; } 

Renderable::Renderable(Mesh* mesh /* = nullptr */, Material* mat /* = nullptr */) {
	m_mesh = mesh;
	m_sharedMaterial = mat;
	m_instanceMaterial = nullptr;
}

Renderable::~Renderable() {
	SAFE_DELETE( m_instanceMaterial );
}


void Renderable::SetModelMatrix( const Matrix44& model ) {
	m_modelMatrix = model;
}


void Renderable::SetMesh( Mesh* mesh ) {
	m_mesh = mesh;
}


void Renderable::SetMaterial( Material* material ) {
	m_sharedMaterial = material;
	SAFE_DELETE( m_instanceMaterial );
}


Material* Renderable::GetMaterial() {
	if ( nullptr == m_instanceMaterial ) {
		return m_sharedMaterial;
	}
	else {
		return m_instanceMaterial;
	}
}


Material* Renderable::GetEditableMaterial() {
	if ( nullptr == m_instanceMaterial ) {
		m_instanceMaterial = new Material(m_sharedMaterial);
	}
	return m_instanceMaterial;
}


Mesh* Renderable::GetMesh() {
	return m_mesh;
}

const Matrix44& Renderable::GetModelMatrix() {
	return m_modelMatrix;
}


Vector3 Renderable::GetPosition() {
	return m_modelMatrix.GetTranslation();
}