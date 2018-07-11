#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/Math/Vector4.hpp"


//----------------------------------------------------------------------------------------------------------------
Sampler::Sampler() {}


//----------------------------------------------------------------------------------------------------------------
Sampler::~Sampler()
{
	Destroy();
}


//----------------------------------------------------------------------------------------------------------------
bool Sampler::Create( eSamplerModes mode /* = SAMPLER_NEAREST_MIPMAP_LINEAR */ )
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
	glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
	glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

	SetSamplerMode( mode );

	return true; 
}


//----------------------------------------------------------------------------------------------------------------
void Sampler::Destroy()
{
	if (m_handle != NULL) {
		glDeleteSamplers( 1, &m_handle ); 
		m_handle = NULL; 
	}
} 


//----------------------------------------------------------------------------------------------------------------
unsigned int Sampler::GetHandle() const {
	return m_handle;
}


//----------------------------------------------------------------------------------------------------------------
void Sampler::SetSamplerMode( eSamplerModes mode ) {

	switch (mode) {
	case SAMPLER_NEAREST: 
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
		break;
	case SAMPLER_LINEAR:
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		break;
	case SAMPLER_LINEAR_MIPMAP_LINEAR:
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

		break;
	case SAMPLER_NEAREST_MIPMAP_LINEAR:
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

		break;
	case SAMPLER_SHADOW:
		glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );  
		glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );		
		glSamplerParameteri( m_handle, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );
		
		{
			Vector4 white = Vector4(1.f, 1.f, 1.f, 1.f);
			glSamplerParameterfv( m_handle, GL_TEXTURE_BORDER_COLOR, (GLfloat*) &white);
		}
		
		glSamplerParameteri( m_handle, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE );
		glSamplerParameteri( m_handle, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL );
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
		glSamplerParameteri( m_handle, GL_TEXTURE_MIN_LOD, (GLint) -1000.f );
		glSamplerParameteri( m_handle, GL_TEXTURE_MAX_LOD, (GLint) 1000.f );

		break;
	default: 
		break;
	}
}