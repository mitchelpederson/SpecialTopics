#include "Game/Entity.hpp"
#include "Game/AIController.hpp"
#include "Game/MissileController.hpp"
#include "Game/TheGame.hpp"
#include "Game/GameCommon.hpp"

#include "Engine/Renderer/DebugRender.hpp"


//----------------------------------------------------------------------------------------------------------------
Entity::Entity( const EntityDefinition& def ) : def( def ) {

	renderable = CreatePlaneRenderable();
	liftAngleOfAttackCurve.AppendPoint( Vector2( -90.f, 0.5f ) );
	liftAngleOfAttackCurve.AppendPoint( Vector2( -5.f, 0.5f ) );
	liftAngleOfAttackCurve.AppendPoint( Vector2( 10.f, 1.25f ) );
	liftAngleOfAttackCurve.AppendPoint( Vector2( 30.f, 1.0f ) );
	liftAngleOfAttackCurve.AppendPoint( Vector2( 60.f, 1.0f ) );
	liftAngleOfAttackCurve.AppendPoint( Vector2( 90.f, 0.8f ) );

	m_missileTimer = new Stopwatch( g_theGame->m_gameClock );
	m_missileTimer->SetTimer( 1.f );

	m_machineGunTimer = new Stopwatch( g_theGame->m_gameClock );
	m_machineGunTimer->SetTimer( 0.1f );
	currentState.health = def.GetMaxHealth();

	currentState.transform.position = Vector3( 0.f, 5000.f, 0.f );
}


//----------------------------------------------------------------------------------------------------------------
Entity::~Entity() {
	if ( renderable != nullptr ) {
		TheGame::GetMultiplayerState()->m_scene->RemoveRenderable( renderable );
		delete renderable;
		renderable = nullptr;
	}

	if ( followCamera != nullptr ) {
		followCamera = nullptr;
	}

	if ( contrails != nullptr ) {
		if ( !contrails->IsSafeToDestroy() ) {
			contrails->transform.position = GetPosition() + Vector3(0.f, -1.f, -5.f);
			contrails->transform.parent = nullptr;
			TheGame::GetMultiplayerState()->m_orphanedParticleEmitters.push_back( contrails );
		} else {
			delete contrails;
		}
	}

	if ( m_machineGunTimer != nullptr ) {
		delete m_machineGunTimer;
		m_machineGunTimer = nullptr;
	}

	if ( m_missileTimer != nullptr ) {
		delete m_missileTimer;
		m_missileTimer = nullptr;
	}
}


//----------------------------------------------------------------------------------------------------------------
Renderable* Entity::CreatePlaneRenderable() {
	Renderable* r = new Renderable();
	r->SetMesh( g_theRenderer->CreateOrGetMesh( def.GetMeshPath() ) );
	r->SetMaterial( g_theRenderer->GetMaterial( def.GetMaterialName() ) );
	r->SetModelMatrix( Matrix44() );

	contrails = new ParticleEmitter();
	contrails->renderable->SetMaterial( g_theRenderer->GetMaterial("contrail") );
	contrails->transform.parent = &currentState.transform;
	contrails->transform.position = Vector3( 0.f, -1.f, -15.f );

	contrails->SetLifetime(20.f, 20.f);
	contrails->spawnInWorldSpace = true;
	contrails->spawnRate = 0.2f;
	contrails->particleSize = FloatRange(1.f, 1.f);
	contrails->startColor = def.GetTrailColor();
	contrails->endColor = contrails->startColor;
	contrails->endColor.a = 0;
	contrails->particleLifespan = FloatRange( def.GetTrailLifespan() );
	contrails->Update = UpdateContrailEmitter;
	contrails->PreRender = PreRenderContrailEmitter;
	contrails->UpdateParticle = UpdateContrailParticle;

	TheGame::GetMultiplayerState()->m_scene->AddParticleEmitter( contrails );

	return r;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Update() {

	currentState.throttle = controller->throttle;
	currentState.rollAxis = controller->rollAxis;
	currentState.yawAxis = controller->yawAxis;
	currentState.pitchAxis = controller->pitchAxis;
	currentState.isFireGunPressed = controller->isFireGunPressed;
	currentState.isFireMissilePressed = controller->isFireMissilePressed;

	if( g_theInputSystem->WasKeyJustPressed('8') ) {
		int a = 0;
	}

	ValidateLockedEntity();

	// Physics logic
	if ( def.GetFlightStyle() != FLIGHT_DUMB ) {
		SimulateFlightPhysicsOnSnapshot( g_theGame->GetDeltaTime(), &currentState, def, liftAngleOfAttackCurve );
	} else {
		SimulateDumbPhysicsOnSnapshot( g_theGame->GetDeltaTime(), &currentState );
	}

	// If I'm a client, update my latest snapshot based on what the host knows.
	if ( !g_theGame->netSession->AmIHost() && m_isLastReceivedSnapshotValid ) {
		if ( def.GetFlightStyle() != FLIGHT_DUMB ) {
			SimulateFlightPhysicsOnSnapshot( g_theGame->GetDeltaTime(), &m_lastReceivedSnapshot, def, liftAngleOfAttackCurve );
		} else {
			SimulateDumbPhysicsOnSnapshot( g_theGame->GetDeltaTime(), &m_lastReceivedSnapshot );
		}	

		NudgeClientTowardsHostSnapshot();
	}

	// Weapon logic
	if ( g_theGame->netSession->AmIHost() ) {
		if ( currentState.isFireGunPressed && m_machineGunTimer->CheckAndReset() ) {
			FireMachineGun();
		}

		if ( currentState.isFireMissilePressed && m_missileTimer->CheckAndReset() ) {
			FireMissile();
		}
	}
	
	renderable->SetModelMatrix( currentState.transform.GetLocalToWorldMatrix() );

	if ( g_theGame->netSession->AmIHost() && g_theGame->GetMultiplayerState()->IsPointBelowTerrain( currentState.transform.position ) ) {
		Kill(-1);
	}

	if ( followCamera != nullptr ) {
		Vector3 forward = currentState.transform.GetWorldForward();
		Vector3 up = currentState.transform.GetWorldUp();
		followCamera->transform.position = currentState.transform.position - (forward * 30.f) + (up * 5.f);
		followCamera->transform.euler = currentState.transform.euler;
		followCamera->transform.Rotate( Vector3( controller->cameraPitchAxis * 60.f, controller->cameraYawAxis * 60.f, 0.f ) );
	}

	if ( contrails != nullptr ) {
		contrails->Update( contrails );
	}

	currentState.age += g_theGame->GetDeltaTime();

	// If I'm a client and this entity is controlled by me
	if ( !g_theGame->netSession->AmIHost() && g_theGame->netSession->GetMyConnectionIndex() == controller->connectionID ) {
		m_snapshotHistory.push_back( currentState );
	}

	if ( !g_theGame->netSession->AmIHost() && g_theGame->GetMultiplayerState()->m_debugDraw ) {	
		DebugRenderWireSphere( 0.f, currentState.transform.position, def.GetPhysicalRadius(), Rgba(0, 255, 0, 255), Rgba(0, 255, 0, 255) );
		DebugRenderWireSphere( 0.f, m_lastReceivedSnapshot.transform.position, def.GetPhysicalRadius(), Rgba(255, 0, 0, 255), Rgba(255, 0, 0, 255) );
	}

}


//----------------------------------------------------------------------------------------------------------------
void Entity::SimulateFlightPhysicsOnSnapshot( float deltaTime, EntitySnapshot_T* ss, const EntityDefinition& def, const CubicSpline2D& liftAngleOfAttackCurve ) {
	float dt = deltaTime;
	float speed = ss->velocity.GetLength();
	Vector3 velocityDirection = ss->velocity.GetNormalized();

	// We will need our percentage of max velocity to figure out our drag force and limits on rotations
	// We also need to know how aligned our plane is with the direction it is moving.
	float percentOfMaxVelocity = speed / def.GetMaxVelocity();
	float forwardVelocityDot = fabsf( DotProduct( ss->transform.GetWorldForward().GetNormalized(), velocityDirection ) );

	// We will handle rotating the plane next
	ss->angularVelocity.x += ss->pitchAxis * 75.f * dt * ClampFloat( 1.f - percentOfMaxVelocity, 0.25f, 1.f);
	ss->angularVelocity.y += ss->yawAxis * 12.f * dt * ClampFloat(1.f - percentOfMaxVelocity, 0.3f, 1.f);
	ss->angularVelocity.z += ss->rollAxis * 75.f * dt; // Roll isn't affected by velocity

													// gotta have angular drag to slow down rotations if there's no input.
	ss->angularVelocity.x *= 1.f - (def.GetPitchDrag() * dt);
	ss->angularVelocity.y *= 1.f - (def.GetYawDrag() * dt);
	ss->angularVelocity.z *= 1.f - (def.GetRollDrag() * dt);

	// Perform plane rotations first
	ss->transform.Rotate( ss->angularVelocity * dt );

	float angleOfAttack = 90.f - AcosDegrees( DotProduct( ss->transform.GetWorldForward(), Vector3::UP ) );
	float angleOfRoll = fabsf( 90.f - AcosDegrees( DotProduct( ss->transform.GetWorldRight(), Vector3::UP ) ) );


	// Rotate the plane down a bit if looking up
	Vector3 forwardOnHorizontal = Vector3::CrossProduct( ss->transform.GetWorldRight(), Vector3::UP ).GetNormalized();
	if ( forwardOnHorizontal.GetLengthSquared() != 1.f ) {
		forwardOnHorizontal = ss->transform.GetWorldForward(); // Check for gimbal lock
	}

	Vector3 rightOnHorizontal = Vector3::CrossProduct( Vector3::UP, forwardOnHorizontal ).GetNormalized();
	if ( rightOnHorizontal.GetLengthSquared() != 1.f ) {
		rightOnHorizontal = ss->transform.GetWorldRight(); // Check for gimbal lock
	}

	Matrix44 targetOrientation( rightOnHorizontal, Vector3::UP, forwardOnHorizontal );
	ss->transform.TurnToward( ss->transform.GetLocalToWorldMatrix(), targetOrientation, ClampFloatZeroToOne(angleOfAttack) * 0.1f * g_theGame->GetDeltaTime() );

	// Force the plane to rotate nose down if we reach stalling speed or are above the max altitude
	if ( ss->velocity.GetLength() < def.GetStallSpeed() || ss->transform.position.y > MAX_ALTITUDE ) {
		Matrix44 stallOrientation( rightOnHorizontal, forwardOnHorizontal, Vector3::UP * -1.f );
		float dotForwardWithStallOrientation = DotProduct( Vector3::UP * -1.f, ss->transform.GetWorldForward() );

		// Don't force stall if we're close to pointing down - this causes jittering and some disorienting flipping due to
		// using eulers to represent rotation. 
		if ( dotForwardWithStallOrientation < 0.95f ) {
			ss->transform.TurnToward( ss->transform.GetLocalToWorldMatrix(), stallOrientation, 80.f * g_theGame->GetDeltaTime() );
		}
	}

	// we also want to make the plane pitch up locally a bit when rolling
	float rollPitchFactor = RangeMapFloat( angleOfRoll, 0.f, 90.f, 0.f, 1.f );
	ss->transform.Rotate( Vector3( rollPitchFactor * dt * 10.f, 0.f, 0.f ) );

	// Save off plane orientation now that we've finished rotations and update our angle of attack and roll
	Vector3 planeForward = ss->transform.GetWorldForward();
	Vector3 planeUp = ss->transform.GetWorldUp();
	Vector3 planeRight = ss->transform.GetWorldRight();

	angleOfAttack = 90.f - AcosDegrees( DotProduct( planeForward, Vector3::UP ) );
	angleOfRoll = fabsf( 90.f - AcosDegrees( DotProduct( planeRight, Vector3::UP ) ) );

	// Get the current thrust
	float targetThrustMagnitude = Interpolate( def.GetThrustRange().min, def.GetThrustRange().max, ss->throttle ); 
	ss->currentThrust = Interpolate( ss->currentThrust, targetThrustMagnitude, 0.99f * dt );
	Vector3 currentThrustForce = planeForward * ss->currentThrust;

	
	// Force opposite of the direction we are moving. F = Cd * d * v^2 * A * 0.5
	// Our cross section area is different based on how the plane is oriented. The more 
	// perpendicular to velocity it is, the more resistance is put up, slowing the plane
	// in that direction. Thrust causes it to move forward instead.
	// We will manually increase drag by a percentage if throttle is below a threshold to
	// approximate airbrakes.
	float currentDragCoefficient = def.GetDragCoefficient().Evaluate( 1.f - DotProduct( planeForward.GetNormalized(), velocityDirection ) );
	currentDragCoefficient *= ClampFloat( RangeMapFloat( ss->throttle, 0.f, 0.25f, 10.f, 1.f ), 1.f, 10.f );	
	Vector3 currentDragForce = velocityDirection * -( currentDragCoefficient *  AIR_DENSITY * speed * speed * def.GetCrossSectionArea().Evaluate( 1.f - DotProduct( planeForward.GetNormalized(), velocityDirection ) ) );

	// Now we need lift and gravity, lift is affected by the angle of attack, the more
	// flat the plane is, the more lift it generates. We approximate this using a dot product
	// with the plane's up and the world up. In reality, the lift generated by the wings is
	// greatest at an angle of attack greater than 0, usually between 10 and 25 degrees.
	// Gravity is just world down.
	float liftForceMagnitude = def.GetMass() * GRAVITY_ACCEL;
	float planeUpDot = RangeMapFloat( AcosDegrees( DotProduct( planeUp.GetNormalized(), Vector3::UP ) ), -180.f, 180.f, 0.f, 1.f );
	Vector3 liftForce = planeUp * liftForceMagnitude * liftAngleOfAttackCurve.EvaluateAtNormalizedParametric( planeUpDot ).y;
	Vector3 weightForce = Vector3::UP * -( def.GetMass() * GRAVITY_ACCEL);

	// After the four forces of flight are calculated, we need some stablizing forces
	// to make flying feel better.
	// A lateral damping force will make our yaw tighter, preventing the plane from flying laterally.
	// We need to get the velocity in the lateral direction, and the more lateral movement we have
	// the stronger a counteracting force needs to be.
	// If the dot is positive, it's right velocity, if negative its left.
	// We repeat the process for vertical velocity. We want a little wiggle room but not much, just 
	// enough to feel like we are flying but not enough to make the plane feel difficult to control.
	float planeLateralFactor = DotProduct( planeRight.GetNormalized(), velocityDirection );
	Vector3 lateralDampingCorrection = planeRight * -planeLateralFactor * speed * 0.2f;

	float planeVerticalFactor = DotProduct( planeUp.GetNormalized(), velocityDirection );
	Vector3 verticalDampingCorrection = planeUp * -planeVerticalFactor * speed * 0.02f;

	// now just add all forces together, get the net acceleration from these forces,
	// and do our Verlet integration.
	Vector3 totalForce = currentThrustForce + currentDragForce + weightForce + liftForce;

	ss->acceleration = ( totalForce * ( 1.f / 16000.f ) );
	ss->velocity += ss->acceleration * dt;

	// Add in our lateral movement correction to make yaw feel good
	ss->velocity += lateralDampingCorrection + verticalDampingCorrection;

	// Cap our max velocity
	if ( ss->velocity.GetLengthSquared() > (def.GetMaxVelocity() * def.GetMaxVelocity()) ) {
		ss->velocity = ss->velocity.GetNormalized() * def.GetMaxVelocity();
	}

	ss->transform.position += ss->velocity * dt;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SimulateDumbPhysicsOnSnapshot( float deltaTime, EntitySnapshot_T* ss ) {
	ss->transform.position += ss->velocity * deltaTime;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::UpdateLastReceivedSnapshot( EntitySnapshot_T* ss, float snapshotAge ) {
	// we gotta instantly change a few gameplay things, like locked targets
	currentState.lockedEntityID = ss->lockedEntityID;
	currentState.health = ss->health;

	// let's check if the new snapshot said this enemy died but we haven't registered it yet
	if ( !ss->isAlive && currentState.isAlive ) {
		Kill(ss->killedBy);
	}

	// Update the last received snapshot from the host
	m_lastReceivedSnapshot = *ss;

	// Simulate on the last received snapshot from the host for the amount of time it took
	// for the snapshot to reach the client.
	if ( def.GetFlightStyle() != FLIGHT_DUMB ) {
		SimulateFlightPhysicsOnSnapshot( snapshotAge, &m_lastReceivedSnapshot, def, liftAngleOfAttackCurve );
	} else {
		SimulateDumbPhysicsOnSnapshot( snapshotAge, &m_lastReceivedSnapshot );
	}

	// For the first snapshot received from the host, we will actually just overwrite the local
	// state entirely
	if ( !m_isLastReceivedSnapshotValid ) {
		currentState = *ss;
	}
	m_isLastReceivedSnapshotValid = true;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::NudgeClientTowardsHostSnapshot() {

	Vector3 displacementBetweenCurrentAndHost = currentState.transform.position - m_lastReceivedSnapshot.transform.position;
	if ( displacementBetweenCurrentAndHost.GetLengthSquared() > NET_SNAPPING_THRESHOLD ) {
		currentState = m_lastReceivedSnapshot;
	}

	else {
		float nudgeFactor = CLIENT_NUDGE_FACTOR_PER_SECOND * g_theGame->GetDeltaTime();

		Matrix44 currentRotation = Matrix44::MakeRotationDegrees( currentState.transform.euler );
		Matrix44 hostEstimatedRotation = Matrix44::MakeRotationDegrees( m_lastReceivedSnapshot.transform.euler );
		Matrix44 newClientRotation = InterpolateRotation( currentRotation, hostEstimatedRotation, nudgeFactor );
		currentState.transform.euler = newClientRotation.GetRotation();

		currentState.transform.position = Interpolate( currentState.transform.position, m_lastReceivedSnapshot.transform.position, nudgeFactor );
		currentState.velocity = Interpolate( currentState.velocity, m_lastReceivedSnapshot.velocity, nudgeFactor );
		currentState.angularVelocity = Interpolate( currentState.angularVelocity, m_lastReceivedSnapshot.angularVelocity, nudgeFactor );
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Kill( int killedByPlayerID ) {

	if ( killedByPlayerID != -1 ) {

		if ( g_theGame->GetMyPlayerInfo()->GetConnectionID() == killedByPlayerID ) {
			g_theGame->GetMultiplayerState()->hud->NotifyKill();
		}

		g_theGame->GetPlayerInfo( killedByPlayerID )->RecordKill();
	}

	currentState.killedBy = (uint8_t) killedByPlayerID;
	currentState.isAlive = false;

	if ( renderable != nullptr ) {
		TheGame::GetMultiplayerState()->m_scene->RemoveRenderable( renderable );
	}

	if ( contrails != nullptr ) {
		if ( !contrails->IsSafeToDestroy() ) {
			contrails->transform.position = GetPosition() + Vector3(0.f, -1.f, -5.f);
			contrails->transform.parent = nullptr;
			TheGame::GetMultiplayerState()->m_orphanedParticleEmitters.push_back( contrails );
			contrails = nullptr;
		} else {
			delete contrails;
			contrails = nullptr;
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Spawn() {
	currentState.isAlive = true;
	if ( renderable != nullptr ) {
		TheGame::GetMultiplayerState()->m_scene->AddRenderable( renderable );
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::FireMissile() {

	Vector3 missileSpawnPos = currentState.transform.position + (currentState.transform.GetWorldUp() * -3.f);
	Vector3 missileSpawnOrientation = currentState.transform.euler;
	Vector3 missileSpawnVel = currentState.velocity;

	Entity* missile = TheGame::GetMultiplayerStateAsHost()->CreateEntity( 10, new MissileController(), controller->connectionID );
	missile->LockOntoEntity( currentState.lockedEntityID );
	missile->Spawn();
	missile->currentState.transform.position = missileSpawnPos;
	missile->currentState.transform.euler = missileSpawnOrientation;
	missile->currentState.velocity = missileSpawnVel;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::FireMachineGun() {

	Entity* missile = TheGame::GetMultiplayerStateAsHost()->CreateEntity( 20, new EntityController(), controller->connectionID );
	missile->LockOntoEntity( currentState.lockedEntityID );
	missile->Spawn();
	missile->currentState.transform.position = currentState.transform.position + (currentState.transform.GetWorldForward() * 20.f);
	missile->currentState.transform.euler = currentState.transform.euler;
	missile->currentState.velocity = currentState.velocity + (currentState.transform.GetWorldForward() * 400.f);

 	if ( followCamera != nullptr ) {
 		FirstPersonCamera* cam = dynamic_cast<FirstPersonCamera*>( followCamera );
 		if (cam != nullptr) {
 			cam->ShakeCamera( 0.05f, 0.3f );
 		}
 	}
}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetHealth() const {
	return currentState.health;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Damage(float damageAmount, int damagingPlayerID, bool wasMissile ) {

	currentState.health -= damageAmount;

	if ( damagingPlayerID != -1 ) {
		
		if ( wasMissile ) {
			g_theGame->GetPlayerInfo( damagingPlayerID )->RecordMissileHit();
			if ( damagingPlayerID == g_theGame->GetMyPlayerInfo()->GetConnectionID() ) {
				g_theGame->GetMultiplayerState()->hud->NotifyMissileHit();
				g_theGame->GetMyPlayerInfo()->RecordMissileHit();
			}
		} else {
			g_theGame->GetPlayerInfo( damagingPlayerID )->RecordGunHit();
			if ( damagingPlayerID == g_theGame->GetMyPlayerInfo()->GetConnectionID() ) {
				g_theGame->GetMultiplayerState()->hud->NotifyGunHit();
				g_theGame->GetMyPlayerInfo()->RecordGunHit();
			}
		}
	}

	if ( currentState.health <= 0.f ) {
		currentState.health = 0.f;
		Kill( damagingPlayerID );
	}
}

//----------------------------------------------------------------------------------------------------------------
void Entity::Heal(float healAmount) {

	currentState.health += healAmount;

}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetAge() const {
	return currentState.age;
}


//----------------------------------------------------------------------------------------------------------------
Vector3 Entity::GetPosition() {
	return currentState.transform.position;
}


//----------------------------------------------------------------------------------------------------------------
Vector3 Entity::GetVelocity() {
	return currentState.velocity;
}


//----------------------------------------------------------------------------------------------------------------
Vector3 Entity::GetForward() {
	return currentState.transform.GetWorldForward();
}


//----------------------------------------------------------------------------------------------------------------
bool Entity::IsAlive() const {
	return currentState.isAlive;
}


//----------------------------------------------------------------------------------------------------------------
bool Entity::IsWeapon() const {
	return def.IsWeapon();
}


//----------------------------------------------------------------------------------------------------------------
void Entity::LockOntoEntity( int idToLock ) {
	if ( TheGame::GetMultiplayerState()->GetEntityByID( idToLock ) == nullptr ) {
		currentState.lockedEntityID = -1;
		return;
	}
	else {
		currentState.lockedEntityID = idToLock;
	}
}


//----------------------------------------------------------------------------------------------------------------
bool Entity::ValidateLockedEntity() {
	if ( currentState.lockedEntityID != -1 ) {
		if ( TheGame::GetMultiplayerState()->DoesEntityExist(currentState.lockedEntityID) ) {
			return true;
		} else {
			currentState.lockedEntityID = -1;
			return false;
		}
	}
	return false;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SwitchLockedTarget() {
	if ( g_theGame->netSession->AmIHost() ) {
		currentState.lockedEntityID = TheGame::GetMultiplayerState()->GetNextEntityToLockFromID( currentState.id, currentState.lockedEntityID );
	}
	else {
		NetMessage changeTarget( NETMSG_CHANGE_TARGET );
		g_theGame->netSession->GetHostConnection()->Send( changeTarget );
	}
}



//----------------------------------------------------------------------------------------------------------------
// Contrail Particle Emitter Callbacks
//----------------------------------------------------------------------------------------------------------------
void UpdateContrailParticle( Particle* p, float deltaTime ) {
	
}


//----------------------------------------------------------------------------------------------------------------
void UpdateContrailEmitter( ParticleEmitter* pe ) {
	if (pe->clock->total.seconds - pe->timeAtLastSpawn > pe->spawnRate) {
		int timesElapsed = (int) ((pe->clock->total.seconds - pe->timeAtLastSpawn) / pe->spawnRate);
		for (int i = 0; i < timesElapsed; i++) {
			pe->SpawnParticle();
		}
		pe->timeAtLastSpawn = pe->clock->total.seconds;
	}

	for (int i = 0; i < pe->particles.size(); i++) {
		Particle* p = &(pe->particles[i]);
		p->force = Vector3(0.f, -1.f, 0.f);
		p->Update(p, pe->clock->frame.seconds);
	}

	if ( pe->particles.size() > 0 && pe->particles.front().IsDead( pe->clock->total.seconds ) ) {
		pe->particles.pop_front();
	}
}


//----------------------------------------------------------------------------------------------------------------
void PreRenderContrailEmitter( ParticleEmitter* pe, Camera* camera ) {
	if (pe->mesh != nullptr) {
		delete pe->mesh;
	}
	pe->mesh = new Mesh();
	MeshBuilder builder;
	builder.Begin(TRIANGLES, false);

	if ( pe->particles.size() > 1 ) {

		for ( int i = pe->particles.size() - 1; i > 0; i-- ) {
			Particle* first = &(pe->particles[i]);
			Particle* second = &(pe->particles[i-1]);

			Vector3 particleDisplacement = first->position - second->position;
			Vector3 quadForward = particleDisplacement.GetNormalized();
			Vector3 quadRight = Vector3::CrossProduct( quadForward, Vector3::UP );
			Vector3 quadUp = Vector3::CrossProduct( quadForward, Vector3::RIGHT );
			quadRight.Normalize();
			quadUp.Normalize();

			float firstAge = first->GetNormalizedAge( pe->clock->total.seconds );
			float secondAge = second->GetNormalizedAge( pe->clock->total.seconds );
			int indexCount = pe->particles.size() - 1 - i;
			unsigned char currentAlpha = ClampInt( indexCount * 32, 0, 255 );
			Rgba firstStartColor = pe->startColor;
			firstStartColor.a = currentAlpha;
			Rgba firstColor = Interpolate( firstStartColor, pe->endColor, firstAge );
			
			currentAlpha = ClampInt( (indexCount + 1) * 32, 0, 255 );
			Rgba secondStartColor = pe->startColor;
			secondStartColor.a = currentAlpha;
			Rgba secondColor = Interpolate( secondStartColor, pe->endColor, secondAge );

			float firstSize = RangeMapFloat(firstAge, 0.f, pe->particleLifespan.max, 1.f, 50.f);
			float secondSize = RangeMapFloat(secondAge, 0.f, pe->particleLifespan.max, 1.f, 50.f);

			Vector3 bl = second->position + (quadUp * -1.f * secondSize);
			Vector3 br = first->position + (quadUp * -1.f * firstSize);
			Vector3 tr = first->position + (quadUp * firstSize);
			Vector3 tl = second->position + (quadUp * secondSize);
			
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 0.f));
			builder.PushVertex(bl);
			builder.SetColor( firstColor );
			builder.SetUV(Vector2(1.f, 0.f));
			builder.PushVertex(br);
			builder.SetUV(Vector2(1.f, 1.f));
			builder.PushVertex(tr);
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 0.f));
			builder.PushVertex(bl);
			builder.SetColor( firstColor );
			builder.SetUV(Vector2(1.f, 1.f));
			builder.PushVertex(tr);
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 1.f));
			builder.PushVertex(tl);

			bl = second->position + (quadRight * -1.f * secondSize);
			tl = second->position + (quadRight * secondSize);
			tr = first->position + (quadRight * firstSize);
			br = first->position + (quadRight * -1.f * firstSize);
			
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 0.f));
			builder.PushVertex(bl);
			builder.SetColor( firstColor );
			builder.SetUV(Vector2(1.f, 0.f));
			builder.PushVertex(br);
			builder.SetUV(Vector2(1.f, 1.f));
			builder.PushVertex(tr);
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 0.f));
			builder.PushVertex(bl);
			builder.SetColor( firstColor );
			builder.SetUV(Vector2(1.f, 1.f));
			builder.PushVertex(tr);
			builder.SetColor( secondColor );
			builder.SetUV(Vector2(0.f, 1.f));
			builder.PushVertex(tl);
		}
	}

	builder.End();
	pe->mesh->FromBuilderAsType<Vertex3D_PCU>( &builder );
	pe->renderable->SetMesh( pe->mesh );
	pe->renderable->SetModelMatrix( Matrix44() );
}


