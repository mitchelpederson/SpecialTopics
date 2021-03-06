#pragma once 
#pragma comment( lib, "opengl32" )	// Link in the OpenGL32.lib static library

#define MAX_LIGHTS 8

#include "Engine/Math/Vector2.hpp"
#include "Engine/Math/Vector4.hpp"
#include "Engine/Core/Rgba.hpp"
#include "Engine/Core/Vertex.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Sprites/Sprite.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/CubeMap.hpp"
#include <string.h>
#include <map>

struct DrawCall; 


enum DrawPrimitive {
	LINES,
	TRIANGLES,
	QUADS
};

enum TextDrawMode {
	TEXT_DRAW_SHRINK_TO_FIT,
	TEXT_DRAW_WORD_WRAP,
	TEXT_DRAW_OVERRUN
};

enum DepthCompare
{
	COMPARE_NEVER,       // GL_NEVER
	COMPARE_LESS,        // GL_LESS
	COMPARE_LEQUAL,      // GL_LEQUAL
	COMPARE_GREATER,     // GL_GREATER
	COMPARE_GEQUAL,      // GL_GEQUAL
	COMPARE_EQUAL,       // GL_EQUAL
	COMPARE_NOT_EQUAL,   // GL_NOTEQUAL
	COMPARE_ALWAYS,      // GL_ALWAYS
};


enum CullMode {
	CULLMODE_BACK,
	CULLMODE_FRONT,
	CULLMODE_NONE
};


enum FillMode {
	FILLMODE_SOLID,
	FILLMODE_WIRE,
	FILLMODE_POINT
};

enum WindOrder {
	WIND_CLOCKWISE,
	WIND_COUNTER_CLOCKWISE
};

enum BlendOp {
	COMPARE_ADD,
	COMPARE_SUBTRACT,
	COMPARE_REVERSE_SUBTRACT,
	COMPARE_MIN,
	COMPARE_MAX
};

enum BlendFactor {
	BLEND_ONE,
	BLEND_ZERO,
	BLEND_ONE_MINUS_SRC_ALPHA,
	BLEND_SRC_ALPHA,
	BLEND_DST_ALPHA
};

enum RenderDataType {
	RENDER_DATA_FLOAT,
	RENDER_DATA_UNSIGNED_INT,
	RENDER_DATA_UNSIGNED_BYTE
};


class RenderState {
public:

	CullMode cullMode = CULLMODE_BACK;
	FillMode fillMode = FILLMODE_SOLID;
	WindOrder windOrder = WIND_COUNTER_CLOCKWISE;

	DepthCompare compareMode = COMPARE_LESS;
	bool depthWrite = true;

	BlendOp colorBlendOp = COMPARE_ADD;
	BlendFactor colorSrcFactor = BLEND_ONE;
	BlendFactor colorDstFactor = BLEND_ONE_MINUS_SRC_ALPHA;

	BlendOp alphaBlendOp = COMPARE_ADD;
	BlendFactor alphaSrcFactor = BLEND_SRC_ALPHA;
	BlendFactor alphaDstFactor = BLEND_ONE_MINUS_SRC_ALPHA;
	bool blend = false;
};

class Renderer {

public:

	//----------------------------------------------------------------------------------------------------------------
	// Infrastructure
	Renderer();
	void Initialize();
	void BeginFrame();
	void EndFrame();	
	bool RenderStartup( void* hwnd );

	//----------------------------------------------------------------------------------------------------------------
	// Generic draw calls
	void Draw( DrawCall& drawCall );
	void DrawRenderable( Renderable* renderable );
	void DrawMesh( Mesh* mesh );
	void DrawMeshImmediate( Vertex3D_PCU* verts, int numVerts, DrawPrimitive drawPrimitive );
	void DrawMeshImmediate( Vertex3D_Lit* verts, int numVerts, unsigned int* indices, int numIndices, DrawPrimitive drawPrimitive );
	void DrawMeshImmediate( MeshBuilder* builder );

	//----------------------------------------------------------------------------------------------------------------
	// Shape draw calls
	void DrawRegularPolygon(const Vector2& center, float radius, float degreesToRotate, int sides, Rgba color = Rgba(255, 255, 255, 255));
	void DrawRegularPolygonLocal(const Vector2& center, float radius, int sides);
	void DrawLine(const Vector2& start, const Vector2& end, const Rgba& color);
	void DrawLine(const Vector2& start, const Vector2& end);
	void DrawAABB(const AABB2& bounds, const Rgba& color);
	void DrawQuad(const Matrix44& transform, const Vector2& dimensions, const Rgba& color);
	void DrawQuad(const Vector3& topLeft, const Vector3& bottomLeft, const Vector3& topRight, const Vector3& bottomRight, const AABB2& topUVs = AABB2::ZERO_TO_ONE, const Rgba& color = Rgba());
	void DrawSprite( const Vector3& position, Sprite* sprite, const Vector3& up, const Vector3& right, const Vector2& scale = Vector2(1.f, 1.f), const Rgba& tint = Rgba(), bool isLit = false); 
	void DrawTexturedAABB( const AABB2& bounds, const Texture& texture, const Vector2& texCoordsAtMins, 
							const Vector2& texCoordsAtMaxs, const Rgba& tint ); 
	void DrawCube( const Vector3& center, const Vector3& size, const Rgba& color = Rgba(255, 255, 255, 255), const AABB2& topUVs = AABB2::ZERO_TO_ONE, const AABB2& sideUVs = AABB2::ZERO_TO_ONE, const AABB2& bottomUVs = AABB2::ZERO_TO_ONE);
	void DrawText2D(const Vector2& drawMins,
		const std::string& asciiText,
		float cellHeight,
		const Rgba& tint,
		float aspectScale, // multiplied by the font�s inherent m_baseAspect
		const BitmapFont* font);
	void DrawText(const Vector3& position
		, const std::string& asciiText
		, float cellHeight
		, const Rgba& tint
		, float aspectScale
		, BitmapFont* font
		, const Vector3& up = Vector3::UP
		, const Vector3& right = Vector3::RIGHT);
	void DrawTextInBox2D(const AABB2& drawBox
		, const Vector2& alignment
		, const std::string& asciiText
		, float cellHeight
		, const Rgba& tint
		, float aspectScale
		, const BitmapFont* font
		, TextDrawMode mode);
	void DrawRegularPolygonDotted(const Vector2& center, float radius, float degreesToRotate, int sides, const Rgba& color);
	void DrawVertexArray(const Vector2* vertices, int numberOfVertices, const Vector2& position, float radius, float degreesToRotate, const Rgba& color, bool isPolygon);
	void DrawAABB3(const Vector3& center, const Vector3& halfSize, const Rgba& color);
	void DrawLine( const Vector3& start, const Vector3& end, const Rgba& startColor, const Rgba& endColor );

	//----------------------------------------------------------------------------------------------------------------
	// Camera functions
	void SetClearScreenColor( const Rgba& color );
	void ClearScreen(const Rgba& color) const;
	void SetOrtho(float left, float right, float bottom, float top, float nearVal, float farVal);
	void SetProjection(const Matrix44& proj);
	void SetCamera(Camera* cam);
	void SetCameraToDefault();
	void SetCameraToUI();
	void ActivateScreenShake(float intensity, float duration);

	//----------------------------------------------------------------------------------------------------------------
	// GL 1 wrappers
	void SetLineWidth(float width) const;
	void PushMatrix() const;
	void PopMatrix() const;
	void TranslateMatrix(float x, float y, float z) const;
	void RotateMatrix(float x, float y, float z, float w) const;
	void ScaleMatrix(float x, float y, float z) const;
	void SetDrawColor(const Rgba& color) const;

	//----------------------------------------------------------------------------------------------------------------
	// Shaders and Materials
	void SetAdditiveBlending();
	void SetAlphaBlending();
	void SetLineWidth(float width);
	void UseShaderProgram(ShaderProgram* shader);
	void SetShader( Shader* shader );
	void BindRenderState();
	void BindMaterial( Material const* material );
	void UseCubemap( unsigned int slot, const CubeMap& cubeMap, Sampler* sampler = nullptr );
	void EnableDepth( DepthCompare compare, bool shouldWrite );
	void DisableDepth();
	void ClearDepth( float depth = 1.f);

	//----------------------------------------------------------------------------------------------------------------
	// Textures and samplers
	Sampler* GetSamplerForMode( eSamplerModes mode = SAMPLER_NEAREST );
	void DisableTexture2D();
	void UseTexture( unsigned int slot, const Texture& tex, Sampler* sampler = nullptr );

	//----------------------------------------------------------------------------------------------------------------
	// Asset management functions
	Texture* CreateOrGetTexture( const std::string& path );
	CubeMap* CreateCubeMap( const std::string& path );
	BitmapFont* CreateOrGetBitmapFont( const char* bitmapFontName );
	ShaderProgram* CreateOrGetShaderProgram( const char* shaderName );
	Shader* GetShader( const std::string& name );
	Texture* CreateRenderTarget( int width, int height, eTextureFormat fmt = TEXTURE_FORMAT_RGBA8 );
	Mesh* CreateOrGetMesh( const std::string& path );
	Material* GetMaterial( const std::string& name );
	void ReloadAllShaders();

	//----------------------------------------------------------------------------------------------------------------
	// Framebuffer management
	bool CopyFrameBuffer( FrameBuffer* dst, FrameBuffer* src );
	Texture* GetDefaultColorTarget();
	Texture* GetDefaultBloomColorTarget();
	Texture* GetDefaultDepthTarget();
	FrameBuffer* GetDefaultFrameBuffer();

	//----------------------------------------------------------------------------------------------------------------
	// Enum conversions
	static GLenum ToGLCompare( DepthCompare compare );
	static GLenum ToGLCullMode( CullMode mode );
	static GLenum ToGLWindOrder( WindOrder order );
	static GLenum ToGLPolygonMode( FillMode mode );
	static GLenum ToGLBlendOp( BlendOp op );
	static GLenum ToGLBlendFactor( BlendFactor factor );
	static GLenum ToGLDataType( RenderDataType type );

	//----------------------------------------------------------------------------------------------------------------
	// Uniform functions
	void BindStandardUniforms();
	void BindLayoutToProgram(VertexLayout const *layout);
	void SetUniform( const std::string& name, const Rgba* color, unsigned int size = 1 ) const;
	void SetUniform( const std::string& name, const float* param, unsigned int size = 1 ) const;
	void SetUniform( const std::string& name, const Vector3* vec3, unsigned int size = 1 ) const;
	void SetUniform( const std::string& name, const Vector4* vec4, unsigned int size = 1 ) const;

	//----------------------------------------------------------------------------------------------------------------
	// Lighting functions
	void SetAmbientLight( float intensity, const Rgba& color );
	void SetPointLight( const Vector3& position, const Rgba& color, float intensity = 1.f, float attenuation = 0.f );
	void SetPointLight( unsigned int index, const Vector3& position, const Rgba& color, float intensity = 1.f, float attenuation = 0.4f );
	void SetDirectionalLight( unsigned int index, const Vector3& position, const Vector3& direction, const Rgba& color, float intensity = 1.f, float attenuation = 0.f);
	void SetSpotLight( unsigned int index, const Vector3& position, const Vector3& direction, float innerAngle, float outerAngle, const Rgba& color, float intensity = 1.f, float attenuation = 0.f);
	void SetLight( unsigned int index, const Light& light );
	void SetSpecular( float power, float amount );
	void DisableAllLights();
	void BindLightState();
	void SetFog( float fogFactor, float maxFogDistance, Rgba const& fogColor );

	void SetModelMatrix( const Matrix44& modelMatrix );
	void SetViewport( int x, int y, int width, int height );
	void SaveScreenshot();
	Matrix44 GetClipToScreenSpace( Camera* worldCam );

private:
	void LoadBuiltInShaders();
	void LoadShaders();
	void LoadMaterials();

	float m_timeScreenShakeStarts;
	float m_screenShakeLength;
	float m_screenShakeIntensity;

	Rgba m_screenClearColor = Rgba(0,0,0,255);

	std::map< const std::string, Texture* > m_textures;
	std::map< std::string, BitmapFont* > m_loadedFonts;
	std::map< std::string, ShaderProgram* > m_loadedShaders;
	std::map< std::string, Shader* > m_shaders;
	std::map< std::string, Mesh* > m_loadedMeshes;
	std::map< std::string, Material* > m_materials;

	AABB2 m_orthoBounds;

	bool m_isScreenShaking;

	BitmapFont* m_defaultFont = nullptr;

	unsigned int default_vao;
	//ShaderProgram* m_currentShaderProgram = nullptr;
	//ShaderProgram* m_defaultShaderProgram = nullptr;
	Shader* m_defaultShader = nullptr;
	Shader* m_currentShader = nullptr;

	Sampler* m_nearestSampler = nullptr;
	Sampler* m_linearSampler = nullptr;
	Sampler* m_defaultSampler = nullptr;
	Sampler* m_linearMipmapSampler = nullptr;
	Sampler* m_shadowSampler = nullptr;
	Texture* m_defaultColorTarget = nullptr;
	Texture* m_defaultBloomColorTarget = nullptr;
	Texture* m_defaultDepthTarget = nullptr;
	FrameBuffer* m_defaultFrameBuffer = nullptr;
	int m_currentTextureID;
	Matrix44 m_modelMatrix = Matrix44();

	Camera* m_defaultCamera = nullptr;
	Camera* m_defaultUICamera = nullptr;
	Camera* m_currentCamera = nullptr;


	// Lighting state
	Rgba m_ambientLightColor;
	float m_ambientLightIntensity = 0.0f;

	Vector3 m_lightPositions[MAX_LIGHTS];
	Vector3 m_lightDirections[MAX_LIGHTS];
	float m_lightInnerAngles[MAX_LIGHTS];
	float m_lightOuterAngles[MAX_LIGHTS];
	Rgba m_lightColors[MAX_LIGHTS];
	float m_lightIntensities[MAX_LIGHTS];
	float m_lightAttenuation[MAX_LIGHTS];
	float m_isPointLight[MAX_LIGHTS];

	float m_isShadowcasting[MAX_LIGHTS];
	Matrix44 m_lightVP[MAX_LIGHTS];
	Matrix44 m_inverseLightVP[MAX_LIGHTS];
	float m_specularAmount = 0.3f;
	float m_specularPower = 16.f;
	float m_fogMaxDistance = 100.f;
	float m_fogFactor = 1.f;
	Rgba m_fogColor = Rgba(200, 200, 200, 255);

};