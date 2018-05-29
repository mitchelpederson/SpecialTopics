#include "Game/Bullet.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/Renderer/Renderer.hpp"

Bullet::Bullet() {
	m_light = new Light();
	m_light->SetAsPointLight(Vector3(), Rgba(255, 0, 120, 255), 0.2f);
	g_theGame->m_scene->AddLight(m_light);
	SetUpBulletRenderable();
	linearDrag = 0.f;
	angularDrag = 0.f;
}


Bullet::~Bullet() {
	delete m_light;
	m_light = nullptr;
}


void Bullet::Update() {

	UpdatePhysics();

	m_light->m_position = transform.position;
	m_lifespan -= g_theGame->GetDeltaTime();
	if (m_lifespan <= 0.f) {
		Kill();
	}

	m_renderable->SetModelMatrix(transform.GetLocalToWorldMatrix());

}


void Bullet::Kill() {
	GameObject::Kill();
	g_theGame->m_scene->RemoveLight(m_light);
	g_theGame->RemoveBullet(this);
}


void Bullet::SetUpBulletRenderable() {
	m_renderable = new Renderable();
	Mesh* bulletMesh = new Mesh();
	MeshBuilder mb;
	mb.Begin(TRIANGLES, false);

	mb.SetColor(Rgba(200, 20, 120, 255));
	mb.PushQuad( Vector3(-0.5f, -0.5f, -2.0f), Vector3(-0.5f, -0.5f,  2.0f)
			   , Vector3( 0.5f,  0.5f,  2.0f), Vector3( 0.5f,  0.5f, -2.0f) );
	
	mb.PushQuad( Vector3( 0.5f, -0.5f, -2.0f), Vector3( 0.5f, -0.5f,  2.0f)
			   , Vector3(-0.5f,  0.5f,  2.0f), Vector3(-0.5f,  0.5f, -2.0f) );

	mb.End();
	bulletMesh->FromBuilderAsType<Vertex3D_PCU>(&mb);
	m_renderable->SetMesh(bulletMesh);

	m_renderable->SetMaterial(g_theRenderer->GetMaterial("particle"));
	g_theGame->m_scene->AddRenderable(m_renderable);
}