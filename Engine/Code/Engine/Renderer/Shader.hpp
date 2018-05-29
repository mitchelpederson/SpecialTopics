#pragma once
//#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/ThirdParty/tinyxml2/tinyxml2.h"

class RenderState;
enum BlendOp;
enum BlendFactor;
enum DepthCompare;
enum CullMode;
enum FillMode;
enum WindOrder;


class Shader {

public:
	Shader();
	Shader( ShaderProgram* program );
	Shader( const tinyxml2::XMLElement& xml );
	~Shader();

	void SetProgram( ShaderProgram* program );
	void EnableBlending( BlendOp op, BlendFactor src, BlendFactor dst );
	void DisasbleBlending();
	void EnableDepth( DepthCompare compare, bool write );
	void DisableDepth();
	void SetCullMode( CullMode mode );
	void SetFillMode( FillMode mode );
	void SetFrontFace( WindOrder order );

	const std::string& GetName() const;
	ShaderProgram* GetProgram() const;
	RenderState* GetRenderState() const;
	unsigned int GetQueue() const;

private:
	ShaderProgram* m_program = nullptr;
	RenderState* m_state = nullptr;;
	std::string m_name;
	unsigned int m_queue = 0;
};