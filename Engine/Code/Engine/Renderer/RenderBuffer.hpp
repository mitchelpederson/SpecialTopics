#pragma once
#include "Engine/Core/Vertex.hpp"

class RenderBuffer {

public:
	RenderBuffer();
	~RenderBuffer();

	bool CopyToGPU( size_t const byte_count, const void* data );
	unsigned int GetHandle() const;

protected:
	size_t buffer_size = 0;
	unsigned int handle = 0;
};


class VertexBuffer : public RenderBuffer {

public:
	void SetVertices( unsigned int stride, unsigned int count, const void* data );
	//void SetVertices( unsigned int count, Vertex3D_PCU* vertices );

	unsigned int GetVertexCount();

private:
	unsigned int m_vertexStride = 4;
	unsigned int m_vertexCount = 0;

};


class IndexBuffer : public RenderBuffer {

public:
	void SetIndices( unsigned int count, const void* data );
	void SetIndices( unsigned int count, unsigned int* data );
	unsigned int GetIndexCount();

private:
	unsigned int m_indexCount = 0;
};