#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Matrix44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"

Camera::Camera()
	: m_cameraMatrix()
	, m_viewMatrix()
	, m_projMatrix() {
	m_frameBuffer = new FrameBuffer();
	m_frameBuffer->SetColorTarget(g_theRenderer->GetDefaultColorTarget());
	m_frameBuffer->SetDepthStencilTarget(g_theRenderer->GetDefaultDepthTarget());
}

Camera::~Camera() {

}


void Camera::Update() {

}


void Camera::LookAt( const Vector3& position, const Vector3& target, const Vector3& up /* = Vector3::UP */ ) {

	Vector3 direction = target - position;
	Vector3 newForward = direction.GetNormalized();
	Vector3 newRight = Vector3::CrossProduct(up, newForward);
	newRight.Normalize();
	Vector3 newUp = Vector3::CrossProduct(newForward, newRight);
	newUp.Normalize();

	m_cameraMatrix = Matrix44(newRight, newUp, newForward, position);

	transform.position = m_cameraMatrix.GetTranslation();
	transform.euler = m_cameraMatrix.GetRotation();
	m_viewMatrix = m_cameraMatrix.GetInverse();

}

void Camera::Translate( const Vector3& position ) {
	Matrix44 trans = Matrix44::MakeTranslation(position);
	trans.Append(m_cameraMatrix);
	m_cameraMatrix = trans;
}



void Camera::SetProjection( Matrix44 proj ) {
	m_projMatrix = proj;
}


void Camera::SetProjectionOrtho( float size, float zNear, float zFar ) {

	float height = Window::GetInstance()->GetHeight() / size;
	float width = Window::GetInstance()->GetWidth() / size;
	float aspect = width / height;

	float l = -(width * aspect) / 2.f;
	float r = -l;
	float t = height / 2.f;
	float b = -t;

	Matrix44 newProj;

	newProj.Ix = 2.f / width;
	newProj.Jy = 2.f / height;
	newProj.Kz = -2.f / (zFar - zNear);
	newProj.Tx = -(l + r) / width;
	newProj.Ty = -(b + t) / height;
	newProj.Tz = -(zNear + zFar) / (zFar - zNear);

	m_projMatrix = newProj;

}


void Camera::SetColorTarget( Texture* colorTarget ) {
	m_frameBuffer->SetColorTarget( colorTarget );
}

void Camera::SetDepthStencilTarget( Texture* depthTarget ) {
	m_frameBuffer->SetDepthStencilTarget( depthTarget );
}


void Camera::SetFrameBuffer( FrameBuffer* frameBuffer ) {
	if (m_frameBuffer != nullptr) {
		delete m_frameBuffer;
		m_frameBuffer = nullptr;
	}

	m_frameBuffer = frameBuffer;
}

void Camera::Finalize() {
	m_frameBuffer->BindTargets();
}


Vector3 Camera::GetForward() const {
	Vector3 kBasis( m_cameraMatrix.Kx, m_cameraMatrix.Ky, m_cameraMatrix.Kz );
	return kBasis.GetNormalized();
}

Vector3 Camera::GetRight() const {
	Vector3 iBasis( m_cameraMatrix.Ix, m_cameraMatrix.Iy, m_cameraMatrix.Iz );
	return iBasis.GetNormalized();
}

Vector3 Camera::GetUp() const {
	Vector3 iBasis( m_cameraMatrix.Jx, m_cameraMatrix.Jy, m_cameraMatrix.Jz );
	return iBasis.GetNormalized();
}

Matrix44 Camera::GetViewProjection() const {
	Matrix44 vp = m_projMatrix;
	vp.Append(m_viewMatrix);
	return vp;
}


void Camera::SetSkybox( CubeMap* skyboxTexture ) {
	// If there already is a skybox, delete it
	if (skybox != nullptr) {
		delete skybox;
		skybox = nullptr;
	}

	// If we are setting a nullptr, just return
	if (skyboxTexture == nullptr) {
		return;
	}

	// If we get here, create a new skybox renderable
	else {
		skybox = new Renderable();

		Material* skyboxMaterial = new Material(g_theRenderer->GetShader("skybox"));
		skyboxMaterial->SetTexture( 0, skyboxTexture );
		skybox->SetMaterial(skyboxMaterial);

		MeshBuilder mb;
		Mesh* cube = new Mesh();
		mb.BuildCube(cube, Vector3::ZERO, Vector3(10.f, 10.f, 10.f));
		skybox->SetMesh(cube);
	}
}