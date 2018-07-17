#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/XmlUtilities.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/glbindings.h"
#include "Engine/Renderer/RenderBuffer.hpp"
#include "Engine/Renderer/ShaderProgram.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/Sampler.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Mesh.hpp"
#include "Engine/Renderer/ForwardRenderPath.hpp"
#include "Engine/Renderer/Light.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "Engine/ThirdParty/stb/stb_image.h"
#include "Engine/ThirdParty/stb/stb_image_write.h"

#include <map>
#include <vector>
#include <string.h>
#include <iostream>
#include <ctime>


HDC g_displayDeviceContext;
HGLRC g_openGLRenderingContext;
HMODULE g_GLLibrary = nullptr;


GLenum Renderer::ToGLCompare( DepthCompare compare ) 
{
	switch(compare) {
	case COMPARE_NEVER:			return GL_NEVER;
	case COMPARE_LESS:			return GL_LESS;
	case COMPARE_LEQUAL:		return GL_LEQUAL;
	case COMPARE_GREATER:		return GL_GREATER;
	case COMPARE_GEQUAL:		return GL_GEQUAL;
	case COMPARE_EQUAL:			return GL_EQUAL;
	case COMPARE_NOT_EQUAL:		return GL_NOTEQUAL;
	case COMPARE_ALWAYS:		return GL_ALWAYS;
	default:					return 0;
	}
}


GLenum Renderer::ToGLCullMode( CullMode mode ) {
	switch (mode) {
	case CULLMODE_BACK:		return GL_BACK;
	case CULLMODE_FRONT:	return GL_FRONT;
	default:				return 0;
	}
}

GLenum Renderer::ToGLPolygonMode( FillMode mode ) {
	switch (mode) {
	case FILLMODE_SOLID:	return GL_FILL;
	case FILLMODE_WIRE:		return GL_LINE;
	case FILLMODE_POINT:	return GL_POINT;
	default:				return 0;
	}
}

GLenum Renderer::ToGLWindOrder( WindOrder order ) {
	switch (order) {
	case WIND_CLOCKWISE:			return GL_CW;
	case WIND_COUNTER_CLOCKWISE:	return GL_CCW;
	default:						return 0;
	}
}

GLenum Renderer::ToGLBlendOp(BlendOp op) {
	switch (op) {
	case COMPARE_ADD:				return GL_FUNC_ADD;
	case COMPARE_SUBTRACT:			return GL_FUNC_SUBTRACT;
	case COMPARE_REVERSE_SUBTRACT:	return GL_FUNC_REVERSE_SUBTRACT;
	case COMPARE_MIN:				return GL_MIN;
	case COMPARE_MAX:				return GL_MAX;
	default:						return 0;

	}
}

GLenum Renderer::ToGLBlendFactor( BlendFactor factor )  {
	switch (factor) {
	case BLEND_ONE:					return GL_ONE;
	case BLEND_ZERO:				return GL_ZERO;
	case BLEND_ONE_MINUS_SRC_ALPHA: return GL_ONE_MINUS_SRC_ALPHA;
	case BLEND_SRC_ALPHA:			return GL_SRC_ALPHA;
	case BLEND_DST_ALPHA:			return GL_DST_ALPHA;
	default:						return GL_ONE;
	}
}


GLenum Renderer::ToGLDataType( RenderDataType type ) {
	switch (type) {
	case RENDER_DATA_FLOAT:			return GL_FLOAT;
	case RENDER_DATA_UNSIGNED_INT:	return GL_UNSIGNED_INT;
	case RENDER_DATA_UNSIGNED_BYTE: return GL_UNSIGNED_BYTE;
	default:						return 0;
	}
}


//------------------------------------------------------------------------
static HGLRC CreateOldRenderContext( HDC hdc ) 
{
	// Setup the output to be able to render how we want
	// (in our case, an RGBA (4 bytes per channel) output that supports OpenGL
	// and is double buffered
	PIXELFORMATDESCRIPTOR pfd;
	memset( &pfd, 0, sizeof(pfd) ); 
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 0; // 24; Depth/Stencil handled by FBO
	pfd.cStencilBits = 0; // 8; DepthStencil handled by FBO
	pfd.iLayerType = PFD_MAIN_PLANE; // ignored now according to MSDN

									 // Find a pixel format that matches our search criteria above. 
	int pixel_format = ::ChoosePixelFormat( hdc, &pfd );
	if ( pixel_format == NULL ) {
		return NULL; 
	}

	// Set our HDC to have this output. 
	if (!::SetPixelFormat( hdc, pixel_format, &pfd )) {
		return NULL; 
	}

	// Create the context for the HDC
	HGLRC context = wglCreateContext( hdc );
	if (context == NULL) {
		return NULL; 
	}

	// return the context; 
	return context; 
}


//------------------------------------------------------------------------
// Creates a real context as a specific version (major.minor)
static HGLRC CreateRealRenderContext( HDC hdc, int major, int minor ) 
{
	// So similar to creating the temp one - we want to define 
	// the style of surface we want to draw to.  But now, to support
	// extensions, it takes key_value pairs
	int const format_attribs[] = {
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,    // The rc will be used to draw to a window
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,    // ...can be drawn to by GL
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,     // ...is double buffered
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB, // ...uses a RGBA texture
		WGL_COLOR_BITS_ARB, 24,             // 24 bits for color (8 bits per channel)
											// WGL_DEPTH_BITS_ARB, 24,          // if you wanted depth a default depth buffer...
											// WGL_STENCIL_BITS_ARB, 8,         // ...you could set these to get a 24/8 Depth/Stencil.
											NULL, NULL,                         // Tell it we're done.
	};

	// Given the above criteria, we're going to search for formats
	// our device supports that give us it.  I'm allowing 128 max returns (which is overkill)
	size_t const MAX_PIXEL_FORMATS = 128;
	int formats[MAX_PIXEL_FORMATS];
	int pixel_format = 0;
	UINT format_count = 0;

	BOOL succeeded = wglChoosePixelFormatARB( hdc, 
		format_attribs, 
		nullptr, 
		MAX_PIXEL_FORMATS, 
		formats, 
		(UINT*)&format_count );

	if (!succeeded) {
		return NULL; 
	}

	// Loop through returned formats, till we find one that works
	for (UINT i = 0; i < format_count; ++i) {
		pixel_format = formats[i];
		succeeded = SetPixelFormat( hdc, pixel_format, NULL ); // same as the temp context; 
		if (succeeded) {
			break;
		} else {
			//DWORD error = GetLastError();
			//Logf( "Failed to set the format: %u", error ); 
		}
	}

	if (!succeeded) {
		return NULL; 
	}

	// Okay, HDC is setup to the rihgt format, now create our GL context

	// First, options for creating a debug context (potentially slower, but 
	// driver may report more useful errors). 
	int context_flags = 0; 
#if defined(_DEBUG)
	context_flags |= WGL_CONTEXT_DEBUG_BIT_ARB; 
#endif

	// describe the context
	int const attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, major,                             // Major GL Version
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,                             // Minor GL Version
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,   // Restrict to core (no compatibility)
		WGL_CONTEXT_FLAGS_ARB, context_flags,                             // Misc flags (used for debug above)
		0, 0
	};

	// Try to create context
	HGLRC context = wglCreateContextAttribsARB( hdc, NULL, attribs );
	if (context == NULL) {
		return NULL; 
	}

	return context;
}


// Rendering startup - called after we have created our window
// error checking has been removed for brevity, but you should check
// the return values at each step.
bool Renderer::RenderStartup( void* hwnd ) 
{
	// load and get a handle to the opengl dll (dynamic link library)
	g_GLLibrary = ::LoadLibraryA( "opengl32.dll" ); 

	// Get the Device Context (DC) - how windows handles the interace to rendering devices
	// This "acquires" the resource - to cleanup, you must have a ReleaseDC(hwnd, hdc) call. 
	HDC hdc = ::GetDC( (HWND) hwnd );       

	// use the DC to create a rendering context (handle for all OpenGL state - like a pointer)
	// This should be very simiilar to SD1
	HGLRC temp_context = CreateOldRenderContext( hdc ); 

	::wglMakeCurrent( hdc, temp_context ); 
	BindGLFunctions();  // find the functions we'll need to create the real context; 

						// create the real context, using opengl version 4.2
	HGLRC real_context = CreateRealRenderContext( hdc, 4, 2 ); 

	// Set and cleanup
	::wglMakeCurrent( hdc, real_context ); 
	::wglDeleteContext( temp_context ); 

	// Bind all our OpenGL functions we'll be using.
	BindGLFunctions(); 

	// set the globals
	g_displayDeviceContext = hdc; 
	g_openGLRenderingContext = real_context; 

	return true; 
}



Renderer::Renderer() : m_textures() {
	m_isScreenShaking = false;

	//m_textures["ERROR"] = new Texture("ERROR");
}


void Renderer::Initialize() {
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glEnable( GL_DEPTH_TEST );

	// default_vao is a GLuint member variable
	glGenVertexArrays( 1, &default_vao ); 
	glBindVertexArray( default_vao ); 
	m_defaultShader = new Shader();
	m_currentShader = m_defaultShader;
	m_defaultShader->SetProgram( CreateOrGetShaderProgram("Data/Shaders/passthroughTex") );
	m_defaultSampler = new Sampler();
	m_defaultSampler->Create();
	m_nearestSampler = new Sampler();
	m_nearestSampler->Create( SAMPLER_NEAREST );
	m_linearSampler = new Sampler();
	m_linearSampler->Create( SAMPLER_LINEAR );
	m_linearMipmapSampler = new Sampler();
	m_linearMipmapSampler->Create( SAMPLER_LINEAR_MIPMAP_LINEAR );
	m_shadowSampler = new Sampler();
	m_shadowSampler->Create( SAMPLER_SHADOW );

	m_defaultCamera = new Camera();
	m_defaultUICamera = new Camera();

	m_defaultColorTarget = CreateRenderTarget( Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight() );
	m_defaultDepthTarget = CreateRenderTarget( Window::GetInstance()->GetWidth(), Window::GetInstance()->GetHeight(), TEXTURE_FORMAT_D24S8 );
	m_defaultFrameBuffer = new FrameBuffer();
	m_defaultFrameBuffer->SetColorTarget(m_defaultColorTarget);
	m_defaultFrameBuffer->SetDepthStencilTarget(m_defaultDepthTarget);

	m_defaultCamera->SetFrameBuffer(m_defaultFrameBuffer);
	m_defaultUICamera->SetFrameBuffer(m_defaultFrameBuffer);
	m_defaultUICamera->SetProjection(Matrix44::MakeOrtho2D(Vector2(0.f, 0.f), Vector2(100.f, 100.f)));

	m_defaultCamera->Finalize();

	LoadShaders();
	LoadBuiltInShaders();
	LoadMaterials();
	EnableDepth(COMPARE_LESS, true);

	DisableAllLights();
}


void Renderer::BeginFrame() {

	SetShader(m_defaultShader);
	SetCamera(m_defaultCamera);

	glClearColor( 0.0f, 0.f, 0.0f, 1.f );
	glClear( GL_COLOR_BUFFER_BIT );
	ClearDepth();
	EnableDepth(COMPARE_LESS, true);


	m_modelMatrix = Matrix44();
}


GLenum GetGLDrawMode(DrawPrimitive mode) {
	switch (mode) {
	case LINES:
		return GL_LINES;
	case TRIANGLES:
		return GL_TRIANGLES;
	case QUADS:
		return GL_QUADS;
	default:
		return 0;
	}
}


void Renderer::DrawRenderable( Renderable* renderable ) {
	SetModelMatrix( renderable->GetModelMatrix() );
	BindMaterial( renderable->GetMaterial() );
	DrawMesh( renderable->GetMesh() );
}

void Renderer::Draw( DrawCall& drawCall ) {
	SetModelMatrix( drawCall.m_model );
	BindMaterial( drawCall.m_material );
	DrawMesh( drawCall.m_mesh );
}


void Renderer::BindLightState() {
	SetUniform("AMBIENT_COLOR", &m_ambientLightColor);
	SetUniform("AMBIENT_INTENSITY", &m_ambientLightIntensity);
	SetUniform("LIGHT_POSITION", m_lightPositions, MAX_LIGHTS);
	SetUniform("LIGHT_DIRECTION", m_lightDirections, MAX_LIGHTS);
	SetUniform("LIGHT_INNER_ANGLE", m_lightInnerAngles, MAX_LIGHTS);
	SetUniform("LIGHT_OUTER_ANGLE", m_lightOuterAngles, MAX_LIGHTS);
	SetUniform("LIGHT_COLOR", m_lightColors, MAX_LIGHTS);
	SetUniform("LIGHT_INTENSITY", m_lightIntensities, MAX_LIGHTS);
	SetUniform("LIGHT_ATTENUATION", m_lightAttenuation, MAX_LIGHTS);
	SetUniform("LIGHT_IS_POINT", m_isPointLight, MAX_LIGHTS);
	SetUniform("SPECULAR_POWER", &m_specularPower);
	SetUniform("SPECULAR_AMOUNT", &m_specularAmount);
	SetUniform("LIGHT_IS_SHADOWCASTING", m_isShadowcasting, MAX_LIGHTS);
	GLint shadowVPUniform			= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "SHADOW_VP");
	GLint shadowInverseVPUniform	= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "SHADOW_INVERSE_VP");
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), shadowVPUniform,		  MAX_LIGHTS, GL_FALSE, &(m_lightVP[0].Ix));
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), shadowInverseVPUniform, MAX_LIGHTS, GL_FALSE, &(m_inverseLightVP[0].Ix));
	
}


void Renderer::DrawMesh( Mesh* mesh ) {

	glUseProgram(m_currentShader->GetProgram()->GetHandle());
	
	glBindBuffer(GL_ARRAY_BUFFER, mesh->GetVertexBufferHandle());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->GetIndexBufferHandle());
	BindLayoutToProgram(mesh->GetVertexLayout());

	BindRenderState();

	GLint modelUniform		= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "MODEL");
	GLint viewUniform		= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "VIEW");
	GLint projectionUniform = glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "PROJECTION");
	GLint cameraUniform		= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "CAMERA");
	GLint cameraPosUniform	= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "EYE_POSITION");
	GLint cameraDirUniform	= glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), "EYE_DIRECTION");
	
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), modelUniform,	1, GL_FALSE, &(m_modelMatrix.Ix));
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), viewUniform, 1, GL_FALSE, &(m_currentCamera->m_viewMatrix.Ix));
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), projectionUniform, 1, GL_FALSE, &(m_currentCamera->m_projMatrix.Ix));
	glProgramUniformMatrix4fv(m_currentShader->GetProgram()->GetHandle(), cameraUniform, 1, GL_FALSE, &(m_currentCamera->m_cameraMatrix.Ix));
		
	Vector3 cameraPosition = m_currentCamera->m_cameraMatrix.GetTranslation();
	Vector3 cameraDirection = m_currentCamera->GetForward();
	glUniform3f(cameraPosUniform, cameraPosition.x, cameraPosition.y, cameraPosition.z);
	glUniform3f(cameraDirUniform, cameraDirection.x, cameraDirection.y, cameraDirection.z);

	BindLightState();
	SetUniform("MAX_FOG_DISTANCE", &m_fogMaxDistance);
	SetUniform("FOG_FACTOR", &m_fogFactor);
	SetUniform("FOG_COLOR", &m_fogColor);
	SetUniform("TIME_IN_SECONDS", &g_masterClock->total.seconds);

	DrawInstructions di = mesh->GetDrawInstructions();
	if (di.useIndices) {
		glDrawElements(GetGLDrawMode(di.type), di.indexCount, GL_UNSIGNED_INT, (void*) 0);
	}
	else {
		glDrawArrays(GetGLDrawMode(di.type), di.startIndex, di.vertexCount);
	}
}


void Renderer::DrawMeshImmediate( Vertex3D_PCU* verts, int numVerts, DrawPrimitive drawPrimitive ) {

	Matrix44 model;
	Mesh* immediateMesh = new Mesh(numVerts, verts);
	immediateMesh->SetDrawPrimitive(drawPrimitive);
	SetModelMatrix(model);
	DrawMesh(immediateMesh);
	delete immediateMesh;

}


void Renderer::DrawMeshImmediate( Vertex3D_Lit* verts, int numVerts, unsigned int* indices, int numIndices, DrawPrimitive drawPrimitive ) {

	Matrix44 model;
	Mesh* immediateMesh = new Mesh(numVerts, numIndices, verts, indices);
	immediateMesh->SetDrawPrimitive(drawPrimitive);
	SetModelMatrix(model);
	DrawMesh(immediateMesh);
	delete immediateMesh;

}


void Renderer::DisableTexture2D() {
	glDisable(GL_TEXTURE_2D);
}



//-----------------------------------------------------------------------------------------------
// Calls SwapBuffers, may include debug rendering
//
void Renderer::EndFrame() {

	CopyFrameBuffer(nullptr, m_currentCamera->m_frameBuffer);

	// "Present" the backbuffer by swapping the front (visible) and back (working) screen buffers
	SwapBuffers( g_displayDeviceContext ); // Note: call this once at the end of each frame
}


//-----------------------------------------------------------------------------------------------
// Draws a quad
//
void Renderer::DrawAABB( const AABB2& bounds, const Rgba& color) {
	UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/passthrough"));
	Vertex3D_PCU vertices[6];

	Vector2 topLeft(bounds.maxs.x, bounds.maxs.y);
	Vector2 topRight(bounds.mins.x, bounds.maxs.y);
	Vector2 bottomLeft(bounds.maxs.x, bounds.mins.y);
	Vector2 bottomRight(bounds.mins.x, bounds.mins.y);

	vertices[0].position = Vector3(topLeft.x, topLeft.y, 0.f);
	vertices[1].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[2].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);
	vertices[3].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[4].position = Vector3(bottomRight.x, bottomRight.y, 0.f);
	vertices[5].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);

	for (int i = 0; i < 6; i++) {
		vertices[i].color = color;
		vertices[i].uv = Vector2();
	}

	DrawMeshImmediate(vertices, 6, TRIANGLES);
}


void Renderer::DrawQuad(const Matrix44& transform, const Vector2& dimensions, const Rgba& color) {
	UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/passthrough"));
	Vertex3D_PCU vertices[6];

	float halfWidth = dimensions.x / 2.f;
	float halfHeight = dimensions.y / 2.f;

	Vector3 topLeft(-halfWidth, halfHeight, 0.f);
	Vector3 topRight(halfWidth, halfHeight, 0.f);
	Vector3 bottomLeft(-halfWidth, -halfHeight, 0.f);
	Vector3 bottomRight(halfWidth, -halfHeight, 0.f);

	topLeft =     transform.TransformPosition(topLeft);
	topRight =    transform.TransformPosition(topRight);
	bottomLeft =  transform.TransformPosition(bottomLeft);
	bottomRight = transform.TransformPosition(bottomRight);
		
	vertices[0].position = Vector3(topLeft.x, topLeft.y, 0.f);
	vertices[1].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[2].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);
	vertices[3].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[4].position = Vector3(bottomRight.x, bottomRight.y, 0.f);
	vertices[5].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);

	for (int i = 0; i < 6; i++) {
		vertices[i].color = color;
		vertices[i].uv = Vector2();
	}

	DrawMeshImmediate(vertices, 6, TRIANGLES);
}


void Renderer::DrawQuad(const Vector3& topLeft, const Vector3& bottomLeft, const Vector3& topRight, const Vector3& bottomRight, const AABB2& uvs, const Rgba& color /* = Rgba() */) {

	Vertex3D_PCU vertices[6];

	vertices[0].position = bottomLeft;
	vertices[1].position = topLeft;
	vertices[2].position = topRight;
	vertices[3].position = bottomLeft;
	vertices[4].position = topRight;
	vertices[5].position = bottomRight;
	vertices[0].uv = Vector2(uvs.mins.x, uvs.mins.y); 
	vertices[1].uv = Vector2(uvs.mins.x, uvs.maxs.y);
	vertices[2].uv = Vector2(uvs.maxs.x, uvs.maxs.y);
	vertices[3].uv = Vector2(uvs.mins.x, uvs.mins.y);
	vertices[4].uv = Vector2(uvs.maxs.x, uvs.maxs.y);
	vertices[5].uv = Vector2(uvs.maxs.x, uvs.mins.y);

	for (int i = 0; i < 6; i++) {
		vertices[i].color = color;
	}

	DrawMeshImmediate(vertices, 6, TRIANGLES);
}



void Renderer::DrawTexturedAABB( const AABB2& bounds, const Texture& texture,
	const Vector2& texCoordsAtMins, const Vector2& texCoordsAtMaxs, const Rgba& tint ) {
	Vertex3D_PCU vertices[6];
	//SetShader(GetShader("passthroughTex"));
	Vector2 topLeft(bounds.maxs.x, bounds.maxs.y);
	Vector2 topRight(bounds.mins.x, bounds.maxs.y);
	Vector2 bottomLeft(bounds.maxs.x, bounds.mins.y);
	Vector2 bottomRight(bounds.mins.x, bounds.mins.y);

	vertices[0].position = Vector3(topLeft.x, topLeft.y, 0.f);
	vertices[0].uv		 = Vector2(texCoordsAtMins.x, texCoordsAtMaxs.y);
	vertices[2].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[2].uv		 = Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y);
	vertices[1].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);
	vertices[1].uv		 = Vector2(texCoordsAtMins.x, texCoordsAtMins.y);
	vertices[3].position = Vector3(topRight.x, topRight.y, 0.f);
	vertices[3].uv		 = Vector2(texCoordsAtMaxs.x, texCoordsAtMaxs.y);
	vertices[5].position = Vector3(bottomRight.x, bottomRight.y, 0.f);
	vertices[5].uv		 = Vector2(texCoordsAtMaxs.x, texCoordsAtMins.y);
	vertices[4].position = Vector3(bottomLeft.x, bottomLeft.y, 0.f);
	vertices[4].uv		 = Vector2(texCoordsAtMins.x, texCoordsAtMins.y);

	for (int i = 0; i < 6; i++) {
		vertices[i].color = tint;
	}

	UseTexture(0, texture);
	DrawMeshImmediate(vertices, 6, TRIANGLES);
} 



//-----------------------------------------------------------------------------------------------
// Draws a regular polygon around a center point
//
void Renderer::DrawRegularPolygon(const Vector2& center, float radius, float degreesToRotate, int sides, Rgba color) {

	Vertex3D_PCU* vertices = new Vertex3D_PCU[2 * sides];

	float degreesPerVertex = 360.f / (float) sides;

	float startX = 0.f;
	float startY = 0.f;
	float endX = 0.f;
	float endY = 0.f;

	for (int i = 0; i < sides; i++) {

		// Find coordinates
		startX = center.x + ( radius * CosDegrees( degreesToRotate + (degreesPerVertex * (float) i ) ));
		startY = center.y + ( radius * SinDegrees( degreesToRotate + (degreesPerVertex * (float) i ) ));
		endX = center.x + ( radius * CosDegrees( degreesToRotate + (degreesPerVertex * (float) ( i + 1 )) ));
		endY = center.y + ( radius * SinDegrees( degreesToRotate + (degreesPerVertex * (float) ( i + 1 )) ));

		vertices[i*2].position = Vector3(startX, startY, 0.f);
		vertices[i*2].color = color;
		vertices[i*2 + 1].position = Vector3(endX, endY, 0.f);
		vertices[i*2 + 1].color = color;

	}

	//SetShader(GetShader("passthrough"));
	DrawMeshImmediate(vertices, 2 * sides, LINES);

}


void Renderer::DrawRegularPolygonLocal(const Vector2& center, float radius, int sides) {

	/*Vertex3D_PCU* vertices = new Vertex3D_PCU[2 * (sides - 1)];

	float degreesPerVertex = 360.f / (float) sides;

	float startX = 0.f;
	float startY = 0.f;
	float endX = 0.f;
	float endY = 0.f;

	for (int i = 0; i < sides; i++) {

		// Find coordinates
		startX = center.x + ( radius * CosDegrees( degreesPerVertex * (float) i ) );
		startY = center.y + ( radius * SinDegrees( degreesPerVertex * (float) i ) );
		endX = center.x + ( radius * CosDegrees( degreesPerVertex * (float) ( i + 1 ) ) );
		endY = center.y + ( radius * SinDegrees( degreesPerVertex * (float) ( i + 1 ) ) );

		vertices[i * 2].position = Vector3(startX, startY, 0.f);
		vertices[(i*2) + 1].position = Vector3(endX, endY, 0.f);

	}

	DrawMeshImmediate(vertices, 2 * (sides - 1), LINES);*/
	UNIMPLEMENTED();
}


//-----------------------------------------------------------------------------------------------
// Sets the screen shake values 
//
void Renderer::ActivateScreenShake(float intensity, float duration) {
	/*m_isScreenShaking = true;
	m_screenShakeIntensity = intensity;
	m_screenShakeLength = duration;
	m_timeScreenShakeStarts = (float) GetCurrentTimeSeconds();*/
	UNIMPLEMENTED();
}


//-----------------------------------------------------------------------------------------------
// Draws a single line from point to point
//
void Renderer::DrawLine(const Vector2& start, const Vector2& end, const Rgba& color) {

	Vertex3D_PCU vertices[2];

	vertices[0].position = Vector3(start, 0.f);
	vertices[1].position = Vector3(end, 0.f);
	vertices[0].color = color;
	vertices[1].color = color;

	DrawMeshImmediate(vertices, 2, LINES);
}


//-----------------------------------------------------------------------------------------------
// Draws a regular polygon but only every other side
//
void Renderer::DrawRegularPolygonDotted(const Vector2& center, float radius, float degreesToRotate, int sides, const Rgba& color) {

	/*Vertex3D_PCU* vertices = new Vertex3D_PCU[2 * (sides - 1)];

	float degreesPerVertex = 360.f / (float) sides;

	float startX = 0.f;
	float startY = 0.f;
	float endX = 0.f;
	float endY = 0.f;

	for (int i = 0; i < sides; i += 2) {

		// Find coordinates
		startX = center.x + ( radius * CosDegrees( degreesToRotate + (degreesPerVertex * (float) i ) ));
		startY = center.y + ( radius * SinDegrees( degreesToRotate + (degreesPerVertex * (float) i ) ));
		endX = center.x + ( radius * CosDegrees( degreesToRotate + (degreesPerVertex * (float) ( i + 1 )) ));
		endY = center.y + ( radius * SinDegrees( degreesToRotate + (degreesPerVertex * (float) ( i + 1 )) ));

		vertices[i * 2].position = Vector3(startX, startY, 0.f);
		vertices[i * 2].color = color;
		vertices[(i*2) + 1].position = Vector3(endX, endY, 0.f);
		vertices[(i*2) + 1].color = color;

	}

	DrawMeshImmediate(vertices, 2 * (sides - 1), LINES);*/ 
	UNIMPLEMENTED();
}


//-----------------------------------------------------------------------------------------------
// Draws a given mesh in the form of an array of vertices
//
void Renderer::DrawVertexArray(const Vector2* vertices, int numberOfVertices, const Vector2& position, float radius, float degreesToRotate, const Rgba& color, bool isPolygon) {
	/*
	glPushMatrix();

	glTranslatef(position.x, position.y, 0.f);
	glRotatef(degreesToRotate, 0.f, 0.f, 1.f);
	glScalef(radius, radius, 1.f);

	Vertex3D_PCU* vertices3D = new Vertex3D_PCU[numberOfVertices + 1];

	for (int i = 0; i < numberOfVertices; i++) {
		vertices3D[i].position = Vector3(vertices[i], 0.f);
		vertices3D[i].color = color;
	}

	if (isPolygon) {
		vertices3D[numberOfVertices].position = Vector3(vertices[0], 0.f);
	}
	else {
		vertices3D[numberOfVertices].position = Vector3(vertices[numberOfVertices - 1], 0.f);
	}
	vertices3D[numberOfVertices].color = color;
	
	DrawMeshImmediate(vertices3D, numberOfVertices + 1, LINES);*/
	UNIMPLEMENTED();
}



void Renderer::SetLineWidth(float width) const {
	UNIMPLEMENTED();
	//glLineWidth( width );
}


void Renderer::ClearScreen(const Rgba& color) const {
	glClear( GL_COLOR_BUFFER_BIT );
}


void Renderer::SetOrtho(float left, float right, float bottom, float top, float nearVal, float farVal) {
	/*glLoadIdentity();
	glOrtho(left, right, bottom, top, nearVal, farVal);*/
	UNIMPLEMENTED();

}


void Renderer::PushMatrix() const {
	//glPushMatrix();
	UNIMPLEMENTED();
}

void Renderer::PopMatrix() const {
	//glPopMatrix();
	UNIMPLEMENTED();
}

void Renderer::TranslateMatrix(float x, float y, float z) const {
	//glTranslatef(x, y, z);
	UNIMPLEMENTED();
}

void Renderer::ScaleMatrix(float x, float y, float z) const {
	//glScalef(x, y, z);
	UNIMPLEMENTED();
}

void Renderer::RotateMatrix(float x, float y, float z, float w) const {
	//glRotatef(x, y, z, w);
	UNIMPLEMENTED();
}

void Renderer::SetDrawColor(const Rgba& color) const {
	//glColor4ub(color.r, color.b, color.g, color.a);
	UNIMPLEMENTED();
}


void Renderer::SetAdditiveBlending() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
}


void Renderer::SetAlphaBlending() {
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}


Texture* Renderer::CreateOrGetTexture( const std::string& path ) {

		
	std::map< const std::string, Texture* >::const_iterator textureIterator = m_textures.find(path);

	if (textureIterator != m_textures.end()) {
		
		return textureIterator->second;

	}
	
	m_textures[path] = new Texture(path);

	return m_textures[path];
}


void Renderer::SetLineWidth(float width) {
	//glLineWidth(width);
	UNIMPLEMENTED();
}


BitmapFont* Renderer::CreateOrGetBitmapFont(const char* bitmapFontName) {

	std::string path("Data/Fonts/" + std::string(bitmapFontName) + ".png");

	std::map<const std::string, BitmapFont* >::const_iterator fontIterator = m_loadedFonts.find(path);

	if (fontIterator != m_loadedFonts.end()) {
		return fontIterator->second;
	}

	else {
		m_loadedFonts[path] = new BitmapFont(path);
	}

	return m_loadedFonts[path];
}


ShaderProgram* Renderer::CreateOrGetShaderProgram( const char* shaderName ) {


	std::map<const std::string, ShaderProgram* >::const_iterator shaderIterator = m_loadedShaders.find(shaderName);

	if (shaderIterator != m_loadedShaders.end() && shaderIterator->second != nullptr) {
		return shaderIterator->second;
	}

	else {
		ShaderProgram* newShader = new ShaderProgram();

		if ( newShader->LoadFromFiles(shaderName) == false ) {

			delete newShader;
			return m_defaultShader->GetProgram();
		}

		else {
			m_loadedShaders[shaderName] = newShader;
			return newShader;
		}
	}
}


void Renderer::UseShaderProgram(ShaderProgram* shader) {
	if (shader != nullptr) {
		m_defaultShader->SetProgram( shader );
	}
}

void Renderer::DrawText2D( const Vector2& drawMins,
						   const std::string& asciiText,
						   float cellHeight,
						   const Rgba& tint = Rgba(),
						   float aspectScale = 1.f, // multiplied by the font’s inherent m_baseAspect
						   const BitmapFont* font = nullptr ) {

	for (unsigned int character = 0; character < asciiText.length(); character++) {

		char glyph = asciiText[character];
		float cellWidth = cellHeight * aspectScale * font->GetGlyphAspect(/*glyph*/);

		Vector2 glyphDrawMins(drawMins);
		glyphDrawMins.x += cellWidth * character;

		Vector2 glyphDrawMaxs(glyphDrawMins);
		glyphDrawMaxs.x += cellWidth;
		glyphDrawMaxs.y += cellHeight;

		AABB2 glyphUVs = font->GetUVsForGlyph(glyph);
		UseTexture(0, *font->GetFontTexture());

		DrawTexturedAABB(AABB2(glyphDrawMins, glyphDrawMaxs), *(font->GetFontTexture()), Vector2(glyphUVs.maxs.x, glyphUVs.mins.y), Vector2(glyphUVs.mins.x, glyphUVs.maxs.y), tint);
		
	}
}


void Renderer::DrawText(const Vector3& position , const std::string& asciiText , float cellHeight , const Rgba& tint , float aspectScale , BitmapFont* font , const Vector3& up /* = Vector3::UP  */, const Vector3& right /* = Vector3::RIGHT */) {
	for (unsigned int character = 0; character < asciiText.length(); character++) {

		char glyph = asciiText[character];
		float cellWidth = cellHeight * aspectScale * font->GetGlyphAspect(/*glyph*/);

		UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/font"));
;
		float horizontalOffset = 0.5f * cellWidth;
		float verticalOffset = 0.5f * cellHeight;

		Vector3 topLeft(		position.x + (horizontalOffset * right.x),	position.y + (verticalOffset * up.y),	position.z + (horizontalOffset * right.z) );
		Vector3 bottomLeft(		position.x + (horizontalOffset * right.x),	position.y - (verticalOffset * up.y),	position.z + (horizontalOffset * right.z) );
		Vector3 topRight(		position.x - (horizontalOffset * right.x),	position.y + (verticalOffset * up.y),	position.z - (horizontalOffset * right.z) );
		Vector3 bottomRight(	position.x - (horizontalOffset * right.x),	position.y - (verticalOffset * up.y),	position.z - (horizontalOffset * right.z) );
		
		AABB2 glyphUVs = font->GetUVsForGlyph(glyph);
		UseTexture(0, *font->GetFontTexture());
		
		DrawQuad(topLeft, bottomLeft, topRight, bottomRight, glyphUVs, tint);
	}
}


void Renderer::DrawTextInBox2D(const AABB2& drawBox, const Vector2& alignment, const std::string& asciiText, 
					 float cellHeight, const Rgba& tint, float aspectScale, const BitmapFont* font, TextDrawMode mode) {

	UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/font"));
	UseTexture(0, *font->GetFontTexture());
	std::string asciiTextModifiable = asciiText;

	float width = drawBox.maxs.x - drawBox.mins.x;
	float height = drawBox.maxs.y - drawBox.mins.y;

	if (mode == TEXT_DRAW_WORD_WRAP) {
		int charIndex = 0; 
		int prevSubstringStart = 0;
		float substringWidth = 0.f;

		while (asciiText[charIndex] != '\0' && charIndex < asciiText.size()) {

			// Loop until we hit a new line, stop if we get to the end of the string or have a line
			// longer than the box itself
			while (asciiText[charIndex] != '\n') {
				substringWidth += font->GetGlyphAspect(/*asciiText[charIndex]*/) * cellHeight * aspectScale;
				if (asciiText[charIndex] == '\0' || asciiText.size() <= charIndex || substringWidth > width) {
					break;
				}
				charIndex++;
			}

			// Find the last space and change that to a new line, if this line is longer than the box.
			// If we hit the beginning of this line, exit the loop
			if (substringWidth > width) {
				while (asciiText[charIndex] != ' ') {
					if (charIndex == prevSubstringStart) {
						break;
					}
					charIndex--;
				}

				if (charIndex != prevSubstringStart) {
					asciiTextModifiable[charIndex] = '\n';
					prevSubstringStart = charIndex;
					substringWidth = 0.f;
				}
			}
		}
	}

	std::vector<std::string> lines = SplitString(asciiTextModifiable,'\n');
	if (mode == TEXT_DRAW_SHRINK_TO_FIT) {
		for (int line = 0; line < lines.size(); line++) {

			float stringWidth = 0.f;
			for (int characterIndex = 0; characterIndex < lines[line].length(); characterIndex++) {
				stringWidth += font->GetGlyphAspect() * cellHeight * aspectScale;
			}
			if (stringWidth >= width) {
				cellHeight = cellHeight * (width / stringWidth);
			}
		}

		if ((float) lines.size() * cellHeight >= height) {
			cellHeight = cellHeight * (height / ((float) lines.size() * cellHeight));
		}
	}

	for (int line = 0; line < lines.size(); line++) {
		// do overrun first
		
		float stringWidth = 0.f;
		for (int characterIndex = 0; characterIndex < lines[line].length(); characterIndex++) {
			stringWidth += font->GetGlyphAspect() * cellHeight * aspectScale;
		}

		float leftOffset = (width - stringWidth) * alignment.x;
		float bottomOffset = ((height - cellHeight) * alignment.y) + (cellHeight * (lines.size() - 1) * (1.f - alignment.y)) - (cellHeight * line);
		Vector2 offsetFromBottomLeft(leftOffset, bottomOffset);

		DrawText2D(drawBox.mins + offsetFromBottomLeft, lines[line], cellHeight, tint, aspectScale, font);
	}
}


void Renderer::ReloadAllShaders() {

	std::map<std::string, ShaderProgram*>::iterator it = m_loadedShaders.begin();

	while (it != m_loadedShaders.end()) {
		if (it->first != "invalid") {
			delete m_loadedShaders[it->first];
			m_loadedShaders[it->first] = nullptr;
			m_loadedShaders[it->first] = CreateOrGetShaderProgram((it->first).c_str());
		}			
		it++;

	}

	m_defaultShader->SetProgram( m_loadedShaders["Data/Shaders/passthrough"] );
	m_currentShader = m_defaultShader;

	Renderer::LoadBuiltInShaders();
}


void Renderer::LoadBuiltInShaders() {
	ShaderProgram* invalid = new ShaderProgram();
	char* invalidVertShaderText = "#version 420 core\nin vec3 POSITION;\nvoid main (void) {\ngl_Position = vec4(POSITION, 1);}";
	char* invalidFragShaderText = "#version 420 core\nout vec4 outColor;void main(void) {outColor = vec4(0, 1, 1, 1);}";
	invalid->LoadFromString(invalidVertShaderText, invalidFragShaderText);
	m_loadedShaders["invalid"] = invalid;
}


void Renderer::SetProjection(const Matrix44& proj) {
	m_currentCamera->SetProjection(proj);
}


void Renderer::DrawAABB3(const Vector3& center, const Vector3& halfSize, const Rgba& color) {

	UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/passthrough"));
	Vertex3D_PCU vertices[36];

	float left = center.x - halfSize.x;
	float right = center.x + halfSize.x;
	float top = center.y + halfSize.y;
	float bottom = center.y - halfSize.y;
	float front = center.z - halfSize.z;
	float back = center.z + halfSize.z;

	Vector3 leftTopFront(left, top, front);
	Vector3 rightTopFront(right, top, front);
	Vector3 leftBottomFront(left, bottom, front);
	Vector3 rightBottomFront(right, bottom, front);
	Vector3 leftTopBack(left, top, back);
	Vector3 rightTopBack(right, top, back);
	Vector3 leftBottomBack(left, bottom, back);
	Vector3 rightBottomBack(right, bottom, back);

	// front face
	vertices[0].position = leftTopFront;
	vertices[1].position = rightTopFront;
	vertices[2].position = leftBottomFront;
	vertices[3].position = rightTopFront;
	vertices[4].position = rightBottomFront;
	vertices[5].position = leftBottomFront;

	// back face
	vertices[6].position = rightTopBack;
	vertices[7].position = leftTopBack;
	vertices[8].position = rightBottomBack;
	vertices[9].position = leftTopBack;
	vertices[10].position = leftBottomBack;
	vertices[11].position = rightBottomBack;

	// left face
	vertices[12].position = leftTopBack;
	vertices[13].position = leftTopFront;
	vertices[14].position = leftBottomBack;
	vertices[15].position = leftTopFront;
	vertices[16].position = leftBottomFront;
	vertices[17].position = leftBottomBack;

	// right face
	vertices[18].position = rightTopFront;
	vertices[19].position = rightTopBack;
	vertices[20].position = rightBottomFront;
	vertices[21].position = rightTopBack;
	vertices[22].position = rightBottomBack;
	vertices[23].position = rightBottomFront;

	// top face
	vertices[24].position = leftTopBack;
	vertices[25].position = rightTopBack;
	vertices[26].position = leftTopFront;
	vertices[27].position = rightTopBack;
	vertices[28].position = rightTopFront;
	vertices[29].position = leftTopFront;

	// bottom face
	vertices[30].position = leftBottomFront;
	vertices[31].position = rightBottomFront;
	vertices[32].position = leftBottomBack;
	vertices[33].position = rightBottomFront;
	vertices[34].position = rightBottomBack;
	vertices[35].position = leftBottomBack;

	for (int i = 0; i < 36; i++) {
		vertices[i].color = color;
		vertices[i].uv = Vector2();
	}

	DrawMeshImmediate(vertices, 36, TRIANGLES);


}


void Renderer::DrawCube(const Vector3& center, const Vector3& size
	, const Rgba& color /* = Rgba(255, 255, 255, 255) */
	, const AABB2& topUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& sideUVs /* = Vector2::ZERO_TO_ONE */
	, const AABB2& bottomUVs /* = Vector2::ZERO_TO_ONE */) {
	
	
	//glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	Vertex3D_PCU vertices[36];

	Vector3 halfSize = size * 0.5f;

	float left = center.x - halfSize.x;
	float right = center.x + halfSize.x;
	float top = center.y + halfSize.y;
	float bottom = center.y - halfSize.y;
	float front = center.z - halfSize.z;
	float back = center.z + halfSize.z;

	Vector3 leftTopFront(left, top, front);
	Vector3 rightTopFront(right, top, front);
	Vector3 leftBottomFront(left, bottom, front);
	Vector3 rightBottomFront(right, bottom, front);
	Vector3 leftTopBack(left, top, back);
	Vector3 rightTopBack(right, top, back);
	Vector3 leftBottomBack(left, bottom, back);
	Vector3 rightBottomBack(right, bottom, back);

	// front face
	vertices[0].position = leftTopFront;
	vertices[1].position = rightTopFront;
	vertices[2].position = leftBottomFront;
	vertices[3].position = rightTopFront;
	vertices[4].position = rightBottomFront;
	vertices[5].position = leftBottomFront;
	vertices[0].uv = Vector2( sideUVs.mins.x, sideUVs.maxs.y );
	vertices[1].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[2].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );
	vertices[3].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[4].uv = Vector2( sideUVs.maxs.x, sideUVs.mins.y );
	vertices[5].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );

	// back face
	vertices[6].position = rightTopBack;
	vertices[7].position = leftTopBack;
	vertices[8].position = rightBottomBack;
	vertices[9].position = leftTopBack;
	vertices[10].position = leftBottomBack;
	vertices[11].position = rightBottomBack;
	vertices[6].uv = Vector2( sideUVs.mins.x, sideUVs.maxs.y );
	vertices[7].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[8].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );
	vertices[9].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[10].uv = Vector2( sideUVs.maxs.x, sideUVs.mins.y );
	vertices[11].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );

	// left face
	vertices[12].position = leftTopBack;
	vertices[13].position = leftTopFront;
	vertices[14].position = leftBottomBack;
	vertices[15].position = leftTopFront;
	vertices[16].position = leftBottomFront;
	vertices[17].position = leftBottomBack;
	vertices[12].uv = Vector2( sideUVs.mins.x, sideUVs.maxs.y );
	vertices[13].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[14].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );
	vertices[15].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[16].uv = Vector2( sideUVs.maxs.x, sideUVs.mins.y );
	vertices[17].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );

	// right face
	vertices[18].position = rightTopFront;
	vertices[19].position = rightTopBack;
	vertices[20].position = rightBottomFront;
	vertices[21].position = rightTopBack;
	vertices[22].position = rightBottomBack;
	vertices[23].position = rightBottomFront;
	vertices[18].uv = Vector2( sideUVs.mins.x, sideUVs.maxs.y );
	vertices[19].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[20].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );
	vertices[21].uv = Vector2( sideUVs.maxs.x, sideUVs.maxs.y );
	vertices[22].uv = Vector2( sideUVs.maxs.x, sideUVs.mins.y );
	vertices[23].uv = Vector2( sideUVs.mins.x, sideUVs.mins.y );

	// top face
	vertices[24].position = leftTopBack;
	vertices[25].position = rightTopBack;
	vertices[26].position = leftTopFront;
	vertices[27].position = rightTopBack;
	vertices[28].position = rightTopFront;
	vertices[29].position = leftTopFront;
	vertices[24].uv = Vector2( topUVs.mins.x, topUVs.maxs.y );
	vertices[25].uv = Vector2( topUVs.maxs.x, topUVs.maxs.y );
	vertices[26].uv = Vector2( topUVs.mins.x, topUVs.mins.y );
	vertices[27].uv = Vector2( topUVs.maxs.x, topUVs.maxs.y );
	vertices[28].uv = Vector2( topUVs.maxs.x, topUVs.mins.y );
	vertices[29].uv = Vector2( topUVs.mins.x, topUVs.mins.y );

	// bottom face
	vertices[30].position = leftBottomFront;
	vertices[31].position = rightBottomFront;
	vertices[32].position = leftBottomBack;
	vertices[33].position = rightBottomFront;
	vertices[34].position = rightBottomBack;
	vertices[35].position = leftBottomBack;
	vertices[30].uv = Vector2( bottomUVs.mins.x, bottomUVs.maxs.y );
	vertices[31].uv = Vector2( bottomUVs.maxs.x, bottomUVs.maxs.y );
	vertices[32].uv = Vector2( bottomUVs.mins.x, bottomUVs.mins.y );
	vertices[33].uv = Vector2( bottomUVs.maxs.x, bottomUVs.maxs.y );
	vertices[34].uv = Vector2( bottomUVs.maxs.x, bottomUVs.mins.y );
	vertices[35].uv = Vector2( bottomUVs.mins.x, bottomUVs.mins.y );

	for (int i = 0; i < 36; i++) {
		vertices[i].color = color;
	}

	DrawMeshImmediate(vertices, 36, TRIANGLES);
}


void Renderer::SetCamera(Camera* cam) {
	m_currentCamera = cam;
	m_currentCamera->Finalize();
	glBindFramebuffer(GL_FRAMEBUFFER, cam->m_frameBuffer->GetHandle());

}


void Renderer::SetViewport( int x, int y, int width, int height ) {
	glViewport(x, y, width, height);

}

void Renderer::SetCameraToDefault() {
	SetCamera(m_defaultCamera);

}

void Renderer::SetCameraToUI() {
	SetCamera(m_defaultUICamera);
}


void Renderer::UseTexture( unsigned int slot, const Texture& tex, Sampler* sampler ) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindSampler(slot, GetSamplerForMode( tex.GetSamplerMode() )->GetHandle());
	
	if (tex.IsCubemap()) {
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex.GetTextureID());
	}
	else {
		glBindTexture(GL_TEXTURE_2D, tex.GetTextureID());

	}
}


void Renderer::UseCubemap( unsigned int slot, const CubeMap& cubeMap, Sampler* sampler ) {
	glActiveTexture(GL_TEXTURE0 + slot);
	if (sampler == nullptr) {
		glBindSampler(slot, m_defaultSampler->GetHandle());
	}
	else {
		glBindSampler(slot, sampler->GetHandle());
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeMap.GetTextureID());
}


Texture* Renderer::CreateRenderTarget( int width, int height, eTextureFormat fmt )
{
	Texture* tex = new Texture();
	tex->CreateRenderTarget( width, height, fmt );    
	return tex;
}


bool Renderer::CopyFrameBuffer( FrameBuffer *dst, FrameBuffer *src )
{
	// we need at least the src.
	if (src == nullptr) {
		return false; 
	}

	// Get the handles - NULL refers to the "default" or back buffer FBO
	GLuint src_fbo = src->m_handle;
	GLuint dst_fbo = NULL; 
	if (dst != nullptr) {
		dst_fbo = dst->m_handle; 
	}

	// can't copy onto ourselves
	if (dst_fbo == src_fbo) {
		return false; 
	}

	// the GL_READ_FRAMEBUFFER is where we copy from
	glBindFramebuffer( GL_READ_FRAMEBUFFER, src_fbo ); 

	// what are we copying to?
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, dst_fbo ); 

	// blit it over - get teh size
	// (we'll assume dst matches for now - but to be safe,
	// you should get dst_width and dst_height using either
	// dst or the window depending if dst was nullptr or not
	unsigned int width = src->GetWidth();     
	unsigned int height = src->GetHeight(); 

	// Copy it over
	glBlitFramebuffer( 0, 0, // src start pixel
		width, height,        // src size
		0, 0,                 // dst start pixel
		width, height,        // dst size
		GL_COLOR_BUFFER_BIT,  // what are we copying (just colour)
		GL_NEAREST );         // resize filtering rule (in case src/dst don't match)

							  // Make sure it succeeded

	// cleanup after ourselves
	glBindFramebuffer( GL_READ_FRAMEBUFFER, NULL ); 
	glBindFramebuffer( GL_DRAW_FRAMEBUFFER, NULL ); 

	return true;
}


Texture* Renderer::GetDefaultColorTarget() {
	return m_defaultColorTarget;
}

Texture* Renderer::GetDefaultDepthTarget() {
	return m_defaultDepthTarget;
}

FrameBuffer* Renderer::GetDefaultFrameBuffer() {
	return m_defaultFrameBuffer;
}

void Renderer::EnableDepth( DepthCompare compare, bool shouldWrite ) {
	// enable/disable the dest
	//glEnable( GL_DEPTH_TEST ); 
	//glDepthFunc( ToGLCompare(compare) ); 

	// enable/disable write
	//glDepthMask( shouldWrite ? GL_TRUE : GL_FALSE ); 
	m_defaultShader->EnableDepth(compare, shouldWrite);
}



void Renderer::DisableDepth() 
{
	// You can glDisable( GL_DEPTH_TEST ) as well, 
	// but that prevents depth clearing from working, 
	// so I prefer to just use these settings which is 
	// effectively not using the depth buffer.
	m_defaultShader->DisableDepth(); 
}


void Renderer::ClearDepth( float depth )
{
	glDepthMask( GL_TRUE );
	glClearDepthf( depth );
	glClear( GL_DEPTH_BUFFER_BIT ); 
}


void Renderer::DrawSprite(const Vector3& position, Sprite* sprite, const Vector3& up, const Vector3& right, const Vector2& scale, const Rgba& tint /* = Rgba() */, bool isLit /* = false */) {
	
	if (isLit) {
		SetShader(GetShader("lit-sprite"));
	}
	else {
		UseShaderProgram(CreateOrGetShaderProgram("Data/Shaders/passthroughTex"));
	}

	UseTexture(0, *sprite->GetTexture());
	UseTexture(1, *CreateOrGetTexture("Data/Images/blank_normal.png"));

	Vector2 pivot = sprite->GetPivot();
	float leftPivotOffset = pivot.x * sprite->GetDimensions().x;
	float rightPivotOffset = (1.f - pivot.x) * sprite->GetDimensions().x;
	float bottomPivotOffset = pivot.y * sprite->GetDimensions().y;
	float topPivotOffset = (1.f - pivot.y) * sprite->GetDimensions().y;

	Vector3 forward = Vector3::CrossProduct(right, up);

	Vector3 topLeft(		position.x + (leftPivotOffset  * right.x * scale.x),	position.y + (topPivotOffset * up.y * scale.y),		position.z + (leftPivotOffset  * right.z * scale.x) );
	Vector3 bottomLeft(		position.x + (leftPivotOffset  * right.x * scale.x),	position.y - (bottomPivotOffset * up.y * scale.y),	position.z + (leftPivotOffset  * right.z * scale.x) );
	Vector3 topRight(		position.x - (rightPivotOffset * right.x * scale.x),	position.y + (topPivotOffset * up.y * scale.y),		position.z - (rightPivotOffset * right.z * scale.x) );
	Vector3 bottomRight(	position.x - (rightPivotOffset * right.x * scale.x),	position.y - (bottomPivotOffset * up.y * scale.y),	position.z - (rightPivotOffset * right.z * scale.x) );

	if(isLit) {
		Vector3 topCenter( position.x, position.y + (topPivotOffset * up.y * scale.y), position.z);
		Vector3 bottomCenter( position.x, position.y - (bottomPivotOffset * up.y * scale.y), position.z);

		AABB2 uvs = sprite->GetUVs();

		Vertex3D_Lit vertices[6];
		unsigned int indices[12] = { 0, 2, 1		// 1 - 3 - 5
								   , 2, 3, 1		// | \ | \ |
								   , 2, 4, 3		// |  \|  \|
								   , 4, 5, 3 };		// 0 - 2 - 4

		vertices[0].color = tint;
		vertices[0].normal = right;
		vertices[0].tangent = forward * -1.f;
		vertices[0].position = bottomLeft;
		vertices[0].uv = Vector2(uvs.mins);

		vertices[1].color = tint;
		vertices[1].normal = right;
		vertices[1].tangent = forward * -1.f;
		vertices[1].position = topLeft;
		vertices[1].uv = Vector2(uvs.mins.x, uvs.maxs.y);

		vertices[2].color = tint;
		vertices[2].normal = forward * -1.f;
		vertices[2].tangent = right * -1.f;
		vertices[2].position = bottomCenter;
		vertices[2].uv = Vector2((uvs.mins.x + uvs.maxs.x) * 0.5f, uvs.mins.y);

		vertices[3].color = tint;
		vertices[3].normal = forward * -1.f;
		vertices[3].tangent = right * -1.f;
		vertices[3].position = topCenter;
		vertices[3].uv = Vector2((uvs.mins.x + uvs.maxs.x) * 0.5f, uvs.maxs.y);

		vertices[4].color = tint;
		vertices[4].normal = right * -1.f;
		vertices[4].tangent = forward;
		vertices[4].position = bottomRight;
		vertices[4].uv = Vector2(uvs.maxs.x, uvs.mins.y);

		vertices[5].color = tint;
		vertices[5].normal = right * -1.f;
		vertices[5].tangent = forward;
		vertices[5].position = topRight;
		vertices[5].uv = Vector2(uvs.maxs);
		DrawMeshImmediate(vertices, 6, indices, 12, TRIANGLES);
	}
	else {
		DrawQuad(topLeft, bottomLeft, topRight, bottomRight, sprite->GetUVs(), tint);
	}
	

}


void Renderer::SetUniform(const std::string& name, Rgba* color, unsigned int size ) const {
	GLint uniformLocation = glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), name.c_str());
	
	if (uniformLocation >= 0) {
		//float* c = new float[MAX_LIGHTS * size];
		float c[32];
		for (unsigned int i = 0; i < size; i++) {
			unsigned int colorIndex = i * 4;
			color[i].GetAsFloats(c[colorIndex], c[colorIndex+1], c[colorIndex+2], c[colorIndex+3]);
		}
		glUniform4fv(uniformLocation, size, c);
	}
}


void Renderer::SetUniform(const std::string& name, float* param, unsigned int size) const {
	GLint uniformLocation = glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), name.c_str());
	if (uniformLocation >= 0) {
		glUniform1fv(uniformLocation, size, param);
	}
}


void Renderer::SetUniform( const std::string& name, Vector3* vec3, unsigned int size ) const {
	GLint uniformLocation = glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), name.c_str());
	if (uniformLocation >= 0) {
		glUniform3fv(uniformLocation, size, &(vec3->x));
	}
}

void Renderer::SetUniform( const std::string& name, Vector4* vec4, unsigned int size ) const {
	GLint uniformLocation = glGetUniformLocation(m_currentShader->GetProgram()->GetHandle(), name.c_str());
	if (uniformLocation >= 0) {
		glUniform4fv(uniformLocation, size, &(vec4->x));
	}
}


void Renderer::SetShader( Shader* shader ) {
	if (nullptr == shader) {
		shader = m_defaultShader;
	}
	m_currentShader = shader;
	glUseProgram(m_currentShader->GetProgram()->GetHandle());
}


void Renderer::BindRenderState() {
	RenderState* renderState = m_currentShader->GetRenderState();

	// Fill options
	glPolygonMode( GL_FRONT_AND_BACK, ToGLPolygonMode( renderState->fillMode ) );

	// Culling options
	glCullFace( ToGLCullMode( renderState->cullMode ) );
	glFrontFace( ToGLWindOrder( renderState->windOrder ) );

	// Depth options
	// enable/disable the dest
	glEnable( GL_DEPTH_TEST ); 
	glDepthFunc( ToGLCompare(renderState->compareMode) ); 
	glDepthMask( renderState->depthWrite ? GL_TRUE : GL_FALSE ); 

	// Blend options
	glEnable(GL_BLEND);
	glBlendEquation( ToGLBlendOp(renderState->alphaBlendOp) );
	glBlendFunc( ToGLBlendFactor(renderState->colorSrcFactor), ToGLBlendFactor(renderState->colorDstFactor));
	//glBlendFuncSeparate( ToGLBlendFactor(renderState->colorSrcFactor), ToGLBlendFactor(renderState->colorDstFactor), 
	//					 ToGLBlendFactor(renderState->alphaSrcFactor), ToGLBlendFactor(renderState->alphaDstFactor) );

}


void Renderer::BindLayoutToProgram(VertexLayout const *layout) {
	unsigned int attribCount = layout->GetAttributeCount();
	for (unsigned int attribIndex = 0; attribIndex < attribCount; attribIndex++) {
		const VertexAttribute& attrib = layout->GetAttribute(attribIndex);
		GLint bind = glGetAttribLocation( m_currentShader->GetProgram()->GetHandle(), attrib.handle.c_str() );
		if (bind >= 0) {
			glEnableVertexAttribArray( bind );
			glVertexAttribPointer( bind, 
				attrib.elementCount, 
				ToGLDataType(attrib.dataType), 
				attrib.isNormalized, 
				layout->stride, 
				(GLvoid*) (size_t) attrib.memberOffset);
		}
	}
}


Mesh* Renderer::CreateOrGetMesh( const std::string& path ) {
	std::map< const std::string, Mesh* >::const_iterator meshIterator = m_loadedMeshes.find(path);

	if (meshIterator != m_loadedMeshes.end()) {

		return meshIterator->second;

	}

	m_loadedMeshes[path] = new Mesh();
	MeshBuilder mb;
	mb.LoadMeshFromOBJ(path);
	m_loadedMeshes[path]->FromBuilderAsType<Vertex3D_Lit>(&mb);

	return m_loadedMeshes[path];
}


void Renderer::SetAmbientLight( float intensity, const Rgba& color ) {
	m_ambientLightColor = color;
	m_ambientLightIntensity = intensity;
}


void Renderer::SetPointLight( const Vector3& position, const Rgba& color, float intensity, float attenuation ) {
	m_lightPositions[0] = position;
	m_lightColors[0] = color;
	m_lightIntensities[0] = intensity;
	m_lightAttenuation[0] = attenuation;
	m_isPointLight[0] = 1.f;
	m_isShadowcasting[0] = 0.f;
}

void Renderer::SetPointLight( unsigned int index, const Vector3& position, const Rgba& color, float intensity, float attenuation ) {
	if (index >= MAX_LIGHTS) {
		ERROR_AND_DIE("Tried to set a point light index higher than max!");
	}
	m_lightPositions[index] = position;
	m_lightInnerAngles[index] = 360.f;
	m_lightOuterAngles[index] = 360.f;
	m_lightColors[index] = color;
	m_lightIntensities[index] = intensity;
	m_lightAttenuation[index] = attenuation;
	m_isPointLight[index] = 1.f;
	m_isShadowcasting[index] = 0.f;
}

void Renderer::SetDirectionalLight( unsigned int index, const Vector3& position, const Vector3& direction, const Rgba& color, float intensity, float attenuation ) {
	if (index >= MAX_LIGHTS) {
		ERROR_AND_DIE("Tried to set a directional light index higher than max!");
	}
	m_lightPositions[index] = position;
	m_lightDirections[index] = direction;
	m_lightInnerAngles[index] = 360.f;
	m_lightOuterAngles[index] = 360.f;
	m_lightColors[index] = color;
	m_lightIntensities[index] = intensity;
	m_lightAttenuation[index] = attenuation;
	m_isPointLight[index] = 0.f;
	m_isShadowcasting[index] = 0.f;
}


void Renderer::SetSpotLight( unsigned int index
	, const Vector3& position
	, const Vector3& direction
	, float innerAngle
	, float outerAngle
	, const Rgba& color
	, float intensity
	, float attenuation) {


	if (index >= MAX_LIGHTS) {
		ERROR_AND_DIE("Tried to set a spot light index higher than max!");
	}
	m_lightPositions[index] = position;
	m_lightDirections[index] = direction;
	m_lightInnerAngles[index] = innerAngle;
	m_lightOuterAngles[index] = outerAngle;
	m_lightColors[index] = color;
	m_lightIntensities[index] = intensity;
	m_lightAttenuation[index] = attenuation;
	m_isPointLight[index] = 1.f;
	m_isShadowcasting[index] = 0.f;

}


void Renderer::SetLight( unsigned int index, const Light& light ) {
	m_lightPositions[index]		= light.m_position;
	m_lightDirections[index]	= light.m_direction;
	m_lightInnerAngles[index]	= light.m_innerAngle;
	m_lightOuterAngles[index]	= light.m_outerAngle;
	m_lightColors[index]		= light.m_color;
	m_lightIntensities[index]	= light.m_intensity;
	m_lightAttenuation[index]	= light.m_attenuation;
	m_isPointLight[index]		= light.m_isPointLight;

	m_isShadowcasting[index]	= light.m_isShadowcasting;
	if (light.m_isShadowcasting > 0.f) {
		m_lightVP[index]		= light.m_viewProjection;
		m_inverseLightVP[index]	= light.m_inverseViewProjection;
		UseTexture(8, *light.m_depthTarget);
	}
}


void Renderer::SetSpecular( float power, float amount ) {
	m_specularPower = power;
	m_specularAmount = amount;
}


void Renderer::LoadShaders() {

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile("Data/Definitions/shaders.xml");

	const tinyxml2::XMLElement* root = doc->FirstChildElement("shaders");
	const tinyxml2::XMLElement* shader = root->FirstChildElement("shader");

	while (shader != nullptr) {
		Shader* temp = new Shader(*shader);
		m_shaders[temp->GetName()] = temp;
		shader = shader->NextSiblingElement("shader");
	}

	delete doc;
	doc = nullptr;

}


void Renderer::LoadMaterials() {

	tinyxml2::XMLDocument* doc = new tinyxml2::XMLDocument();
	doc->LoadFile("Data/Definitions/materials.xml");

	const tinyxml2::XMLElement* root = doc->FirstChildElement("materials");
	const tinyxml2::XMLElement* material = root->FirstChildElement("material");

	while (material != nullptr) {
		std::string name = "error";
		name = ParseXmlAttribute(*material, "name", name);

		if (name == "error") {
			ERROR_AND_DIE("MATERIAL HAD NO NAME");
		}
		Material* temp = new Material(*material);
		m_materials[name] = temp;
		material = material->NextSiblingElement("material");
	}

	delete doc;
	doc = nullptr;

}



Shader* Renderer::GetShader( const std::string& name ) {
	std::map< std::string, Shader* >::const_iterator it = m_shaders.find(name);
	if (it != m_shaders.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}


Material* Renderer::GetMaterial( const std::string& name ) {
	std::map< std::string, Material* >::const_iterator it = m_materials.find(name);
	if (it != m_materials.end()) {
		return it->second;
	}
	else {
		return nullptr;
	}
}


void Renderer::DisableAllLights() {
	for (int i = 0; i < MAX_LIGHTS; i++) {
		m_lightIntensities[i] = 0.f;
	}
}


void Renderer::BindMaterial( Material const* material ) {
	SetShader(material->shader);

	for (unsigned int texIndex = 0; texIndex < material->GetTextureCount(); texIndex++) {
		
		if (material->textures[texIndex] != nullptr) {
			if (material->samplers[texIndex] == nullptr) {
				UseTexture(texIndex, *material->textures[texIndex]);
			}
			else {
				UseTexture(texIndex, *material->textures[texIndex]);//, *material->samplers[texIndex]);
			}
		}
	}

	for (unsigned int propIndex = 0;  propIndex < material->GetPropertyCount(); propIndex++) {
		material->properties[propIndex]->Bind();
	}
}


void Renderer::SetModelMatrix( const Matrix44& modelMatrix ) {
	m_modelMatrix = modelMatrix;
}


CubeMap* Renderer::CreateCubeMap( const std::string& path ) {
	return new CubeMap(path);
}


void Renderer::SaveScreenshot() {

	unsigned int width = Window::GetInstance()->GetWidth();
	unsigned int height= Window::GetInstance()->GetHeight();
	unsigned int size = width * height * 4;
	unsigned char* data = (unsigned char*) malloc(size);

	glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	std::time_t currentTime = std::time(nullptr);
	std::tm* timeComponents = std::localtime(&currentTime);
	std::string timeStamp	= "Data/Screenshots/"
							+ std::to_string(timeComponents->tm_year + 1900) 
							+ std::to_string(timeComponents->tm_mon)
							+ std::to_string(timeComponents->tm_mday)
							+ "_"
							+ std::to_string(timeComponents->tm_hour)
							+ std::to_string(timeComponents->tm_min)
							+ std::to_string(timeComponents->tm_sec)
							+ ".jpg";

	stbi_flip_vertically_on_write(1);
	stbi_write_jpg(timeStamp.c_str(), width, height, 4, data, 80);

	free(data);
}


void Renderer::SetFog( float fogFactor, float maxFogDistance, Rgba const& fogColor ) {
	m_fogColor = fogColor;
	m_fogFactor = fogFactor;
	m_fogMaxDistance = maxFogDistance;
}

Sampler* Renderer::GetSamplerForMode( eSamplerModes mode ) {
	switch ( mode ) {
		case SAMPLER_NEAREST:
			return m_nearestSampler;
		case SAMPLER_LINEAR:
			return m_linearSampler;
		case SAMPLER_NEAREST_MIPMAP_LINEAR:
			return m_linearMipmapSampler;
		case SAMPLER_LINEAR_MIPMAP_LINEAR:
			return m_linearMipmapSampler;
		case SAMPLER_SHADOW:
			return m_shadowSampler;
		default: 
			return m_defaultSampler;
	}
}