#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Renderer.hpp"

Mesh::Mesh() {
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = true;
}

Mesh::Mesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_PCU* vertices, unsigned int* indices ) {
	m_vbo.SetVertices(sizeof(Vertex3D_PCU), vertCount, vertices);
	m_ibo.SetIndices(indexCount, indices);
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = true;
	m_instructions.indexCount = m_ibo.GetIndexCount();
	m_layout = &Vertex3D_PCU::LAYOUT;
}


Mesh::Mesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_Lit* vertices, unsigned int* indices ) {
	m_vbo.SetVertices(sizeof(Vertex3D_Lit), vertCount, vertices);
	m_ibo.SetIndices(indexCount, indices);
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = true;
	m_instructions.indexCount = m_ibo.GetIndexCount();
	m_layout = &Vertex3D_Lit::LAYOUT;
}


Mesh::Mesh( unsigned int count,  Vertex3D_PCU* vertices ) {
	m_vbo.SetVertices( sizeof(Vertex3D_PCU), count, vertices );
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = false;
	m_layout = &Vertex3D_PCU::LAYOUT;
}


Mesh::~Mesh() {

}


void Mesh::SetMesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_PCU* vertices, unsigned int* indices ) {
	m_vbo.SetVertices(sizeof(Vertex3D_PCU), vertCount, vertices);
	m_ibo.SetIndices(indexCount, indices);
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = true;
	m_instructions.indexCount = m_ibo.GetIndexCount();
	m_layout = &Vertex3D_PCU::LAYOUT;
}

void Mesh::SetMesh( unsigned int count, Vertex3D_PCU* vertices ) {
	m_vbo.SetVertices( sizeof(Vertex3D_PCU), count, vertices );
	m_instructions.startIndex = 0;
	m_instructions.type = TRIANGLES;
	m_instructions.vertexCount = m_vbo.GetVertexCount();
	m_instructions.useIndices = false;
	m_layout = &Vertex3D_PCU::LAYOUT;
}


void Mesh::SetIndices( unsigned int count, const unsigned int* data )  {
	m_ibo.SetIndices(count, data);
}


unsigned int Mesh::GetIndexBufferHandle() {
	return m_ibo.GetHandle();
}

unsigned int Mesh::GetVertexBufferHandle() {
	return m_vbo.GetHandle();
}


const DrawInstructions& Mesh::GetDrawInstructions() {
	return m_instructions;
}


void Mesh::SetDrawPrimitive(DrawPrimitive type) {
	m_instructions.type = type;
}


const VertexLayout* Mesh::GetVertexLayout() const {
	return m_layout;
}