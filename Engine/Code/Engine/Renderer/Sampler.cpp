#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/glbindings.h"

Sampler::Sampler() {}

Sampler::~Sampler()
{
	Destroy();
}

bool Sampler::Create()
{
	// create the sampler handle if needed; 
	if (m_handle == NULL) {
		glGenSamplers( 1, &m_handle ); 
		if (m_handle == NULL) {
			return false; 
		}
	}

	// setup wrapping
	glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_S, GL_REPEAT );  
	glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_T, GL_REPEAT );  
	glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_R, GL_REPEAT );  

	// filtering; 
	glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
	glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	return true; 
}

void Sampler::Destroy()
{
	if (m_handle != NULL) {
		glDeleteSamplers( 1, &m_handle ); 
		m_handle = NULL; 
	}
} 


unsigned int Sampler::GetHandle() const {
	return m_handle;
}