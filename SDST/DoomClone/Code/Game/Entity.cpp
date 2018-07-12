#include "Game/Entity.hpp"
#include "Game/EntityDefinition.hpp"
#include "Game/GameCommon.hpp"


//----------------------------------------------------------------------------------------------------------------
Entity::Entity() 
	: m_position(2.f, 2.f)
	, m_velocity(0.f, 0.f)
	, m_cosmeticRadius(0.5f)
	, m_physicalRadius(0.3f)
	, m_orientationDegrees(0.f)
	, m_spinSpeed(0.f)
	, m_age(0.f)
	, m_health(1.f)
	, m_maxHealth(1.f)
	, m_isAlive(true)
	, m_ageAtDeath(9999999.f)
	, m_isSolid(true)
	, m_firingDelayTimer(g_theGame->m_gameClock)
	, m_cantMoveTimer(g_theGame->m_gameClock)
{}


//----------------------------------------------------------------------------------------------------------------
Entity::Entity( unsigned char id )
	: m_firingDelayTimer(g_theGame->m_gameClock)
	, m_cantMoveTimer(g_theGame->m_gameClock) {

	m_def = EntityDefinition::s_definitions[id];
	m_cosmeticRadius = m_def->GetCosmeticRadius();
	m_physicalRadius = m_def->GetPhysicalRadius();
	m_maxHealth = m_def->GetMaxHealth();
	m_health = m_maxHealth;
	m_isSolid = m_def->IsSolid();
	m_animSetName = m_def->GetSpriteSetName();
	m_currentSpriteAnim = new IsoSpriteAnim(IsoSpriteAnimDef::s_definitions[m_animSetName + ".walk"], g_theGame->m_gameClock);
	m_firingDelayTimer.SetTimer(m_def->GetShootingDelay());
	m_firingDelayTimer.Reset();
	m_cantMoveTimer.SetTimer(0.75f);
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Update() {

	GameMap* map = g_theGame->GetCurrentMap();

	if (m_isAlive) {

		IntVector2 currentTile(m_position);
		IntVector2 leftTile			= IntVector2( -1,  0 ) + currentTile;
		IntVector2 rightTile		= IntVector2(  1,  0 ) + currentTile;
		IntVector2 upTile			= IntVector2(  0,  1 ) + currentTile;
		IntVector2 downTile			= IntVector2(  0, -1 ) + currentTile;
		IntVector2 leftUpTile		= IntVector2( -1,  1 ) + currentTile;
		IntVector2 rightUpTile		= IntVector2(  1,  1 ) + currentTile;
		IntVector2 leftDownTile		= IntVector2( -1, -1 ) + currentTile;
		IntVector2 rightDownTile	= IntVector2(  1, -1 ) + currentTile;

		map->PushEntityOutOfTile( this, leftTile );
		map->PushEntityOutOfTile( this, rightTile );
		map->PushEntityOutOfTile( this, upTile );
		map->PushEntityOutOfTile( this, downTile );
		map->PushEntityOutOfTile( this, leftUpTile );
		map->PushEntityOutOfTile( this, rightUpTile );
		map->PushEntityOutOfTile( this, leftDownTile );
		map->PushEntityOutOfTile( this, rightDownTile );
	}

	UpdateSpriteAnim();

	if (m_def->IsHostile() && m_isAlive) {
		Entity* player = g_theGame->GetPlayer();

		Vector2 displacementToPlayer = player->m_position - m_position;

		m_orientationDegrees = TurnToward(m_orientationDegrees, displacementToPlayer.GetOrientationDegrees(), m_def->GetMaxTurnRate() * g_theGame->GetDeltaTime() );

		// Check if I can shoot based on how aligned my angle is to the direction to the player
		if (DotProduct(displacementToPlayer.GetNormalized(), Vector2::MakeDirectionAtDegrees(m_orientationDegrees)) > 0.8f) {
			Vector2 bulletStart = m_position + (Vector2::MakeDirectionAtDegrees(m_orientationDegrees) * m_physicalRadius);
			RaycastResult result = g_theGame->GetCurrentMap()->Raycast(bulletStart, m_orientationDegrees);

			if (result.hitEntity && result.entity == g_theGame->GetPlayer()) {
				Shoot();
			}
		}

		// If not shooting, then move towards player
		if (m_cantMoveTimer.HasElapsed()) {
			m_position += Vector2::MakeDirectionAtDegrees(m_orientationDegrees) * m_def->GetMaxMoveSpeed() * g_theGame->GetDeltaTime();
		}
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::UpdateSpriteAnim() {
	if (m_currentSpriteAnim->HasFinished() && m_isAlive) {
		delete m_currentSpriteAnim;
		m_currentSpriteAnim = new IsoSpriteAnim( IsoSpriteAnimDef::s_definitions[m_animSetName + ".walk"], g_theGame->m_gameClock );
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Render() const {

	if ("NONE" != m_animSetName) {
		Vector3 spritePos(m_position.x, 0.f, m_position.y);
		Vector3 cameraRight = g_theGame->GetPlayerCamera()->GetRight();
		Vector3 cameraForward = g_theGame->GetPlayerCamera()->GetForward();
		Vector3 entityDirection = Vector3( CosDegrees(m_orientationDegrees + 45.f), 0.f, SinDegrees(m_orientationDegrees + 45.f) );
		Sprite* currentSprite = m_currentSpriteAnim->GetCurrentIsoSprite()->GetSpriteForViewAngle(entityDirection, cameraForward, cameraRight);
		g_theRenderer->DrawSprite(spritePos, currentSprite, Vector3::UP, cameraRight);
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::RenderMinimap() const {
	if (m_isAlive) {
		Vector2 minimapPosition;
		minimapPosition.x = RangeMapFloat(m_position.x, 0.f, 32.f, 0.f, 30.f);
		minimapPosition.y = RangeMapFloat(m_position.y, 0.f, 32.f, 0.f, 30.f);
		g_theRenderer->DrawRegularPolygon(m_position, m_def->GetPhysicalRadius(), 0.f, 14, m_def->GetMinimapColor());
		g_theRenderer->DrawLine(m_position, m_position + Vector2::MakeDirectionAtDegrees(m_orientationDegrees), Rgba());
	}
}


//////////////////////////////////////////////////////////////////////////
// Behaviors
//----------------------------------------------------------------------------------------------------------------
void Entity::Shoot() {
	if (m_firingDelayTimer.HasElapsed() && m_currentAmmo > 0) {
		Vector2 bulletStart = m_position + (Vector2::MakeDirectionAtDegrees(m_orientationDegrees) * m_physicalRadius);
		RaycastResult result = g_theGame->GetCurrentMap()->Raycast(bulletStart, m_orientationDegrees);

		if (result.hitEntity) {
			result.entity->Damage(m_def->GetShotDamage());
		}

		delete m_currentSpriteAnim;
		m_currentSpriteAnim = new IsoSpriteAnim( IsoSpriteAnimDef::s_definitions[m_animSetName + ".shoot"], g_theGame->m_gameClock );

		m_firingDelayTimer.Reset();
		m_cantMoveTimer.Reset();
		m_currentAmmo--;

		SoundID pistolShot = g_audioSystem->CreateOrGetSound("Data/Audio/pistol_shot.wav");
		Vector3 position3D( m_position.x, 0.5f, m_position.y );
		g_audioSystem->PlaySoundAtLocation(pistolShot, position3D, Vector3::ZERO);
	}
}


//////////////////////////////////////////////////////////////////////////
// Getters and Setters
//----------------------------------------------------------------------------------------------------------------
void Entity::SetPosition(const Vector2& position) {
	m_position = position;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SetCosmeticRadius(float newRadius) {
	m_cosmeticRadius = newRadius;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SetPhysicalRadius(float newRadius) {
	m_physicalRadius = newRadius;
}


//----------------------------------------------------------------------------------------------------------------
Vector2	Entity::GetPosition() const {
	return m_position;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SetVelocity(const Vector2& velocity) {
	m_velocity = velocity;
}


//----------------------------------------------------------------------------------------------------------------
Vector2 Entity::GetVelocity() const {
	return m_velocity;
}


//----------------------------------------------------------------------------------------------------------------
Disc2 Entity::GetCosmeticDisc2() const {
	return Disc2(m_position, m_cosmeticRadius);
}


//----------------------------------------------------------------------------------------------------------------
Disc2 Entity::GetPhysicalDisc2() const {
	return Disc2(m_position, m_physicalRadius);
}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetOrientationDegrees() const {
	return m_orientationDegrees;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::SetOrientationDegrees(float orientation) {
	m_orientationDegrees = orientation;
}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetSpinSpeed() const {
	return m_spinSpeed;
}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetAge() const {
	return m_age;
}


//----------------------------------------------------------------------------------------------------------------
bool Entity::IsAlive() const {
	return m_isAlive;
}


//----------------------------------------------------------------------------------------------------------------
AABB2 Entity::GetPhysicalAABB2() const {
	return AABB2(Vector2(m_position.x - m_physicalRadius, m_position.y - m_physicalRadius)
		, Vector2(m_position.x + m_physicalRadius, m_position.y + m_physicalRadius));
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Revive() {
	m_isAlive = true;
}


//----------------------------------------------------------------------------------------------------------------
float Entity::GetHealth() const {
	return m_health;
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Damage(float damageAmount) {

	m_health -= damageAmount;

	if (m_health <= 0.f) {
		m_health = 0.f;
		Kill();
	}
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Kill() {
	m_isAlive = false;
	delete m_currentSpriteAnim;
	m_currentSpriteAnim = new IsoSpriteAnim( IsoSpriteAnimDef::s_definitions[m_animSetName + ".death"], g_theGame->m_gameClock);
}


//----------------------------------------------------------------------------------------------------------------
void Entity::Heal(float healAmount) {

	m_health += healAmount;

}


//----------------------------------------------------------------------------------------------------------------
bool Entity::IsSolid() const {
	return m_isSolid;
}