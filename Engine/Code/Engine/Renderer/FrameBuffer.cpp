#include "Engine/Renderer/FrameBuffer.hpp"
#include "Engine/Renderer/glbindings.h"

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &m_handle);
}

FrameBuffer::~FrameBuffer() {
	if (m_handle != NULL) {
		glDeleteFramebuffers( 1, &m_handle ); 
		m_handle = NULL; 
	}
}


void FrameBuffer::SetColorTarget( Texture *colorTarget ) {
	m_colorTarget = colorTarget;
}


void FrameBuffer::SetDepthStencilTarget( Texture *depthTarget ) {
	m_depthStencilTarget = depthTarget;
}


bool FrameBuffer::BindTargets() {
	glBindFramebuffer( GL_FRAMEBUFFER, m_handle ); 

	// keep track of which outputs go to which attachments; 
	GLenum targets[1]; 

	// Bind a color target to an attachment point
	// and keep track of which locations to to which attachments. 
	glFramebufferTexture( GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0 + 0, 
		m_colorTarget->GetTextureID(), 
		0 ); 
	// 0 to to attachment 0
	targets[0] = GL_COLOR_ATTACHMENT0 + 0;

	// Update target bindings
	glDrawBuffers( 1, targets ); 

	// Bind depth if available;
	if (m_depthStencilTarget == nullptr) {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			NULL, 
			0 ); 
	} else {
		glFramebufferTexture( GL_FRAMEBUFFER, 
			GL_DEPTH_STENCIL_ATTACHMENT, 
			m_depthStencilTarget->GetTextureID(), 
			0 ); 
	}

	// Error Check - recommend only doing in debug
#if defined(_DEBUG)
	GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		//LogErrorf( "Failed to create framebuffer:  %u", status );
		return false;
	}
#endif

	return true;
}


int FrameBuffer::GetHeight() {
	return m_colorTarget->GetDimensions().y;
}

int FrameBuffer::GetWidth() {
	return m_colorTarget->GetDimensions().x;
}


int FrameBuffer::GetHandle() {
	return m_handle;
}