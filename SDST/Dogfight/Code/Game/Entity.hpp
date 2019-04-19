#pragma once
#include "Game/EntityDefinition.hpp"

#include "Engine/Math/Vector3.hpp"
#include "Engine/Math/Disc2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/CubicSpline.hpp"
#include "Engine/Core/Transform.hpp"
#include "Engine/Core/BytePacker.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Renderer/Renderable.h"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Renderer/Light.hpp"
#include "Engine/Renderer/ParticleEmitter.hpp"


class NetMessage;
class EntityController;


constexpr int ENTITY_SNAPSHOT_HISTORY_LENGTH = 3;
constexpr float CLIENT_NUDGE_FACTOR_PER_SECOND = 1.f;


struct EntitySnapshot_T {
	int		id;
	Transform transform;
	Vector3 velocity = Vector3(0.f, 0.f, 200.f);
	Vector3 acceleration;
	Vector3 angularVelocity;
	float	currentThrust = 0.f;
	float	age = 0.f;
	float	ageAtDeath = -1.f;
	float	health = 1.f;
	bool	isAlive = true;
	uint8_t killedBy = 255;
	float	throttle = 0.f;
	float	rollAxis = 0.f;
	float	pitchAxis = 0.f;
	float	yawAxis = 0.f;
	bool	isFireMissilePressed = false;
	bool	isFireGunPressed = false;
	int		lockedEntityID = -1;
	float	timestamp;

	void WriteToBytePacker( BytePacker* bp ) {
		bp->WriteValue<int>(id);
		bp->WriteValue<Vector3>( transform.position );
		bp->WriteValue<Vector3>( transform.euler );
		bp->WriteValue<Vector3>( velocity );
		bp->WriteValue<Vector3>( acceleration );
		bp->WriteValue<Vector3>( angularVelocity );
		bp->WriteValue<float>( currentThrust );
		bp->WriteValue<float>( age );
		bp->WriteValue<float>( ageAtDeath );
		bp->WriteValue<float>( health );
		bp->WriteValue<float>( throttle );
		bp->WriteValue<float>( rollAxis );
		bp->WriteValue<float>( pitchAxis );
		bp->WriteValue<float>( yawAxis );
		bp->WriteValue<int>( lockedEntityID );
		bp->WriteValue<bool>( isAlive );
		bp->WriteValue<uint8_t>( killedBy );
		bp->WriteValue<float>( timestamp );
		bp->WriteValue<bool>( isFireMissilePressed );
		bp->WriteValue<bool>( isFireGunPressed );
	}

	void ReadFromBytePacker( BytePacker* bp ) {
		bp->ReadValue<int>( &id );
		bp->ReadValue<Vector3>( &transform.position );
		bp->ReadValue<Vector3>( &transform.euler );
		bp->ReadValue<Vector3>( &velocity );
		bp->ReadValue<Vector3>( &acceleration );
		bp->ReadValue<Vector3>( &angularVelocity );
		bp->ReadValue<float>( &currentThrust );
		bp->ReadValue<float>( &age );
		bp->ReadValue<float>( &ageAtDeath );
		bp->ReadValue<float>( &health );
		bp->ReadValue<float>( &throttle );
		bp->ReadValue<float>( &rollAxis );
		bp->ReadValue<float>( &pitchAxis );
		bp->ReadValue<float>( &yawAxis );
		bp->ReadValue<int>(	&lockedEntityID );
		bp->ReadValue<bool>( &isAlive );
		bp->ReadValue<uint8_t>( &killedBy );
		bp->ReadValue<float>( &timestamp );
		bp->ReadValue<bool>( &isFireMissilePressed );
		bp->ReadValue<bool>( &isFireGunPressed );
	}

};


class Entity {

public:
								Entity( const EntityDefinition& def );
								~Entity();

	virtual void				Kill( int killedByPlayerID );
	virtual void				Spawn();
			void				Update();

			float				GetAge() const;	
			float				GetHealth() const;
			bool				IsAlive() const;
			bool				IsWeapon() const;

			void				Damage( float damageAmount, int damagingPlayerID, bool wasMissile );
			void				Heal( float healAmount );

			void				LockOntoEntity( int idToLock );
			void				SwitchLockedTarget();
			void				FireMissile();
			void				FireMachineGun();
			bool				ValidateLockedEntity();

			void				UpdateLastReceivedSnapshot( EntitySnapshot_T* ss, float snapshotAge );

			Vector3				GetPosition();
			Vector3				GetVelocity();
			Vector3				GetForward();

private:
			Renderable*			CreatePlaneRenderable();

			//void				SimulateFlightPhysics();
	static	void				SimulateFlightPhysicsOnSnapshot( float deltaTime, EntitySnapshot_T* ss, const EntityDefinition& def, const CubicSpline2D& liftAngleOfAttackCurve );
	static	void				SimulateDumbPhysicsOnSnapshot( float deltaTime, EntitySnapshot_T* ss );

			void				NudgeClientTowardsHostSnapshot();

			Stopwatch*			m_machineGunTimer = nullptr;
			Stopwatch*			m_missileTimer = nullptr;

			EntitySnapshot_T	m_lastReceivedSnapshot;
			bool				m_isLastReceivedSnapshotValid = false;

			std::deque<EntitySnapshot_T> m_snapshotHistory; // Used to keep track of player inputs for correcting client players


public:
	const	EntityDefinition&	def;
			//int					id = 0;
			//int					lockedEntityID = -1; // none locked on
			//Transform			transform = Transform();
			//Vector3				velocity = Vector3(0.f, 0.f, 200.f);
			//Vector3				acceleration = Vector3(0.f, 0.f, 1.f);
			//Vector3				localAngularVelocity = Vector3();

			//float				age = 0.f;
			//float				health = 100.f;
			//bool				isAlive = false;
			//float				ageAtDeath = -1.f;

			EntitySnapshot_T	currentState;

			Renderable*			renderable = nullptr;
			EntityController*	controller = nullptr;
			Camera*				followCamera = nullptr;
			ParticleEmitter*	contrails = nullptr;


public:
			float				currentThrust = 80000.f; // newtons
			float				maxAltitude = 18000.f; // meters
			CubicSpline2D		liftAngleOfAttackCurve;

};

void	UpdateContrailParticle( Particle* p, float deltaTime );
void	UpdateContrailEmitter( ParticleEmitter* pe );
void	PreRenderContrailEmitter( ParticleEmitter* pe, Camera* camera );


void	SendEntityCreate( NetMessage* msg, void* obj );
void*	RecvEntityCreate( NetMessage* msg );
void	SendEntityDestroy( NetMessage* msg, void* obj );
void	RecvEntityDestroy( NetMessage* msg, void* obj );

void	GetEntitySnapshot( void*& snapshot, void* obj );
void	SendEntitySnapshot( NetMessage* msg, void* snapshot );
void	RecvEntitySnapshot( NetMessage* msg, void* snapshot );
void	ApplyEntitySnapshot( void* snapshot, void* obj, float snapshotAge );