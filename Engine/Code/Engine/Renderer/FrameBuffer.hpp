#pragma once
#include "Engine/Renderer/Texture.hpp"

class FrameBuffer
{
public:
	FrameBuffer(); 
	~FrameBuffer();

	// should just update members
	// finalize does the actual binding
	void SetColorTarget( Texture *colorTarget ); 
	void SetDepthStencilTarget( Texture *depthTarget ); 

	bool BindTargets();
	int GetHeight();
	int GetWidth();
	int GetHandle();

	// setups the the GL framebuffer - called before us. 
	// TODO: Make sure this only does work if the targets
	// have changed.
	void Finalize(); 

public:
	unsigned int m_handle; 
	Texture *m_colorTarget; 
	Texture *m_depthStencilTarget; 
};