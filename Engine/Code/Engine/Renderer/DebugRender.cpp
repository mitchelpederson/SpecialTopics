#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/MeshBuilder.hpp"
#include "Engine/DevConsole/Command.hpp"

Renderer*						DebugRenderState::currentRenderer = nullptr;
Camera*							DebugRenderState::currentCamera = nullptr;
std::vector<DebugRenderObject>* DebugRenderState::objects = nullptr;
bool							DebugRenderState::isActive = true;


typedef void (*command_cb)( const std::string& command ); 

void ClearCommand( const std::string& command ) {
	DebugRenderClear();
}

void ToggleCommand( const std::string& command ) {
	DebugRenderToggle();
}

void DebugRenderStartup( Renderer* renderer ) {
	DebugRenderState::currentRenderer = renderer;
	DebugRenderState::currentCamera = nullptr;
	DebugRenderState::objects = new std::vector<DebugRenderObject>();
	CommandRegistration::RegisterCommand("drclear", ClearCommand, "Clears all debug draws");
	CommandRegistration::RegisterCommand("drtoggle", ToggleCommand, "Toggles debug render");
}


void DebugRenderShutdown() {
	for (unsigned int index = 0; index < DebugRenderState::objects->size(); index++) {
		delete DebugRenderState::objects->at(index).mesh;
	}
	delete DebugRenderState::objects;
	DebugRenderState::objects = nullptr;
}


void DebugRenderAndUpdate() {

	if (DebugRenderState::isActive) {

		// Render section
		Renderer* r = DebugRenderState::currentRenderer;
		if (DebugRenderState::currentCamera != nullptr) {
			r->SetCamera(DebugRenderState::currentCamera);
		}
		else {
			//		r->SetCameraToDefault();
		}
		r->BindMaterial(r->GetMaterial("debugRender"));
		std::vector<DebugRenderObject>& objects = *DebugRenderState::objects;


		// Update section
		for (unsigned int index = 0; index < objects.size(); index++) {
			const DebugRenderObject& current = objects.at(index);
			float timeSinceSpawn = g_masterClock->total.seconds - current.spawnTime;

			if (timeSinceSpawn >= current.lifetime) {
				delete objects[index].mesh;
				objects[index].mesh = nullptr;
				objects[index] = objects[objects.size()-1];
				objects.pop_back();
			}
		}

		for (unsigned int index = 0; index < objects.size(); index++) {
			const DebugRenderObject& current = objects.at(index);

			float timeSinceSpawn = g_masterClock->total.seconds - current.spawnTime;
			float fractionIntoObjectLife = timeSinceSpawn / current.lifetime;
			Rgba color = Interpolate(current.startColor, current.endColor, fractionIntoObjectLife);
			r->SetUniform("IN_COLOR", &color);
			r->SetModelMatrix(Matrix44());
			r->DrawMesh(current.mesh);
		}

		
	}
}


void DebugRenderSet3DCamera( Camera *camera ) {
	DebugRenderState::currentCamera = camera;
}


void DebugRenderPoint( float lifetime, const Vector3& position, const Rgba& start_color, const Rgba& end_color, DebugRenderMode mode ) {
	MeshBuilder builder;
	Mesh* pointMesh = new Mesh();
	builder.BuildCube(pointMesh, position, Vector3(0.1f, 0.1f, 0.1f));
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, pointMesh));
}


void DebugRenderLineSegment(float lifetime, const Vector3& p0, const Rgba& p0_color, const Vector3& p1, const Rgba& p1_color, const Rgba& start_color /* = Rgba(0, 255, 0, 255) */, const Rgba& end_color /* = Rgba(255, 0, 0, 255) */, DebugRenderMode mode /* = DEBUG_RENDER_USE_DEPTH */) {
	MeshBuilder builder;
	Mesh* lineMesh = new Mesh();
	builder.BuildLine(lineMesh, p0, p1, p0_color, p1_color);
	lineMesh->SetDrawPrimitive(LINES);
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, lineMesh));
}


void DebugRenderBasis(float lifetime, const Matrix44& basis, const Rgba& start_color /* = Rgba(0, 255, 0, 255) */, const Rgba& end_color /* = Rgba(255, 0, 0, 255) */, DebugRenderMode mode /* = DEBUG_RENDER_USE_DEPTH */) {
	MeshBuilder builder;
	Mesh* basisMesh = new Mesh();
	builder.BuildBasis(basisMesh, basis, basis.GetTranslation() );
	basisMesh->SetDrawPrimitive(LINES);
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, basisMesh));
}

void DebugRenderWireSphere(float lifetime, const Vector3& pos, float radius, const Rgba& start_color /* = Rgba(0, 255, 0, 255) */, const Rgba& end_color /* = Rgba(255, 0, 0, 255) */, DebugRenderMode mode /* = DEBUG_RENDER_USE_DEPTH */) {
	MeshBuilder builder;
	Mesh* sphereMesh = new Mesh();
	builder.BuildWireSphere(sphereMesh, pos, radius, 15, 10);
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, sphereMesh));
}


void DebugRenderQuad(float lifetime, const Vector3& pos, const Vector3& right, float const x_min, float const x_max, const Vector3& up, float const y_min, float const y_max, Texture* texture, const Rgba& start_color /* = Rgba(0, 255, 0, 255) */, const Rgba& end_color /* = Rgba(255, 0, 0, 255) */, DebugRenderMode mode /* = DEBUG_RENDER_USE_DEPTH */) {
	MeshBuilder builder;
	Mesh* quadMesh = new Mesh();
	builder.BuildQuad(quadMesh, pos, up, right, Rgba());
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, quadMesh));
}


void DebugRenderWireCube(float lifetime, const Vector3& position, const Rgba& start_color /* = Rgba(0, 255, 0, 255) */, const Rgba& end_color /* = Rgba(255, 0, 0, 255) */, DebugRenderMode mode /* = DEBUG_RENDER_USE_DEPTH */) {
	MeshBuilder builder;
	Mesh* cubeMesh = new Mesh();
	builder.BuildWireCube(cubeMesh, position, Vector3(0.5f, 0.5f, 0.5f));
	cubeMesh->SetDrawPrimitive(LINES);
	DebugRenderState::objects->push_back(DebugRenderObject(start_color, end_color, lifetime, g_masterClock->total.seconds, mode, cubeMesh));

}


void DebugRenderClear() {
	DebugRenderState::objects->clear();
}


void DebugRenderToggle() {
	DebugRenderState::isActive = !(DebugRenderState::isActive);
}