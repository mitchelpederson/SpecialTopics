#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/glbindings.h"


RenderBuffer::RenderBuffer() {}

RenderBuffer::~RenderBuffer() {
	// cleanup for a buffer; 
	if (handle != NULL) {
		glDeleteBuffers( 1, &handle ); 
		handle = NULL; 
	}

}

bool RenderBuffer::CopyToGPU( size_t const byte_count, const void* data ) 
{
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (handle == 0) {
		glGenBuffers( 1, &handle ); 
	}

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer( GL_ARRAY_BUFFER, handle ); 
	glBufferData( GL_ARRAY_BUFFER, byte_count, data, GL_DYNAMIC_DRAW ); 

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	buffer_size = byte_count; 
	return true; 
}

unsigned int RenderBuffer::GetHandle() const {
	return handle;
}


/*void VertexBuffer::SetVertices( unsigned int count, Vertex3D_PCU* vertices ) {
	int stride = sizeof(Vertex3D_PCU);

	SetVertices(stride, count, (void*) vertices);
}*/

void VertexBuffer::SetVertices( unsigned int stride, unsigned int count, const void* data ) {
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (handle == 0) {
		glGenBuffers( 1, &handle ); 
	}
	m_vertexStride = stride;
	m_vertexCount = count;
	size_t byte_count = stride * count;

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer( GL_ARRAY_BUFFER, handle ); 
	glBufferData( GL_ARRAY_BUFFER, byte_count, data, GL_DYNAMIC_DRAW ); 

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	buffer_size = byte_count; 
	m_vertexStride = stride;
	m_vertexCount = count;
}


void IndexBuffer::SetIndices( unsigned int count, unsigned int* data ) {
	// handle is a GLuint member - used by OpenGL to identify this buffer
	// if we don't have one, make one when we first need it [lazy instantiation]
	if (handle == 0) {
		glGenBuffers( 1, &handle ); 
	}

	size_t byte_count = count * sizeof(unsigned int);

	// Bind the buffer to a slot, and copy memory
	// GL_DYNAMIC_DRAW means the memory is likely going to change a lot (we'll get
	// during the second project)
	glBindBuffer( GL_ARRAY_BUFFER, handle ); 
	glBufferData( GL_ARRAY_BUFFER, byte_count, data, GL_DYNAMIC_DRAW ); 

	// buffer_size is a size_t member variable I keep around for 
	// convenience
	buffer_size = byte_count; 
	m_indexCount = count;
}


void IndexBuffer::SetIndices( unsigned int count, const void* data ) {
	SetIndices(count, (unsigned int*) data);
}


unsigned int VertexBuffer::GetVertexCount() {
	return m_vertexCount;
}

unsigned int IndexBuffer::GetIndexCount() {
	return m_indexCount;
}