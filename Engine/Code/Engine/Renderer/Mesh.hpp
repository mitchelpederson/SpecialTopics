#pragma once
#include "Engine/Renderer/RenderBuffer.hpp"

struct Vertex3D_PCU;
class VertexLayout;
class MeshBuilder;
enum DrawPrimitive;
 
struct DrawInstructions {
	DrawPrimitive type;
	unsigned int startIndex;
	unsigned int vertexCount;
	bool useIndices;
	unsigned int indexCount;
};



class Mesh {
public:
	Mesh();
	Mesh( unsigned int count, Vertex3D_PCU* vertices );
	Mesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_PCU* vertices, unsigned int* indices );
	Mesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_Lit* vertices, unsigned int* indices );

	~Mesh();

	void SetMesh( unsigned int count, Vertex3D_PCU* vertices );
	void SetMesh( unsigned int vertCount, unsigned int indexCount, Vertex3D_PCU* vertices, unsigned int* indices );
	void SetDrawPrimitive( DrawPrimitive type );

	unsigned int GetVertexBufferHandle();
	unsigned int GetIndexBufferHandle();
	const DrawInstructions& GetDrawInstructions();
	const VertexLayout* GetVertexLayout() const;

	void SetIndices( unsigned int count, const unsigned int* data );

	template <typename VERTEXTYPE>
	void SetVertices( unsigned int count, VERTEXTYPE* vertices ) {

		m_layout = &VERTEXTYPE::LAYOUT;
		m_instructions.startIndex = 0;
		m_instructions.vertexCount = count;
		m_instructions.useIndices = 0;
		m_vbo.SetVertices( sizeof(VERTEXTYPE), count, vertices);
	}

	template <typename VERTEX_TYPE>
	void FromBuilderAsType( MeshBuilder* mb ) {

		const VertexMaster* verts = mb->GetVertices().data();
		const unsigned int* indices = mb->GetIndices().data();

		VERTEX_TYPE* tempVerts = new VERTEX_TYPE[mb->GetVertexCount()];

 		for (unsigned int i = 0; i < mb->GetVertexCount(); i++) {
 			tempVerts[i] = VERTEX_TYPE(verts[i]);
 		}

		SetVertices<VERTEX_TYPE>(mb->GetVertexCount(), tempVerts);

		m_instructions = mb->GetDrawInstructions();

		if (m_instructions.useIndices) {
			SetIndices( mb->GetIndexCount(), indices);
		}

		delete[] tempVerts;
	}

private:
	VertexBuffer m_vbo;
	IndexBuffer m_ibo;
	DrawInstructions m_instructions;
	const VertexLayout* m_layout;
};