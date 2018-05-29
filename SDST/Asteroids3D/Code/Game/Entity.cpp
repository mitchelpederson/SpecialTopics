#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

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
	, m_canWalk(true)
	, m_canFly(false)
	, m_canSwim(false)
{}


void Entity::Update(float deltaSeconds) {

	const XboxController& controller = g_theInputSystem->GetController(0);

	if (controller.GetLeftStickMagnitude() > 0.0f) {
		m_orientationDegrees = controller.GetLeftStickAngle();
		m_position += 3.f * Vector2::MakeDirectionAtDegrees(controller.GetLeftStickAngle()) * controller.GetLeftStickMagnitude() * deltaSeconds;
	}

	if (g_theInputSystem->IsKeyPressed('W')) {
		m_position.y += deltaSeconds;
	}
	if (g_theInputSystem->IsKeyPressed('D')) {
		m_position.x += deltaSeconds;
	}

}


void Entity::Render() const {

	g_theRenderer->DrawRegularPolygonDotted(m_position, m_cosmeticRadius, 0.5f, 40, Rgba(0, 255, 255, 255));
	g_theRenderer->DrawRegularPolygonDotted(m_position, m_physicalRadius, 0.3f, 40, Rgba(255, 0, 255, 255));

}


//-----------------------------------------------------------------------------------------------
// Getters and Setters
//
void Entity::SetPosition(const Vector2& position) {
	m_position = position;
}

void Entity::SetCosmeticRadius(float newRadius) {
	m_cosmeticRadius = newRadius;
}

void Entity::SetPhysicalRadius(float newRadius) {
	m_physicalRadius = newRadius;
}

Vector2	Entity::GetPosition() const {
	return m_position;
}

void Entity::SetVelocity(const Vector2& velocity) {
	m_velocity = velocity;
}

Vector2 Entity::GetVelocity() const {
	return m_velocity;
}

Disc2 Entity::GetCosmeticDisc2() const {
	return Disc2(m_position, m_cosmeticRadius);
}

Disc2 Entity::GetPhysicalDisc2() const {
	return Disc2(m_position, m_physicalRadius);
}

float Entity::GetOrientationDegrees() const {
	return m_orientationDegrees;
}

void Entity::SetOrientationDegrees(float orientation) {
	m_orientationDegrees = orientation;
}

float Entity::GetSpinSpeed() const {
	return m_spinSpeed;
}

float Entity::GetAge() const {
	return m_age;
}

bool Entity::IsAlive() const {
	return m_isAlive;
}

AABB2 Entity::GetPhysicalAABB2() const {
	return AABB2(Vector2(m_position.x - m_physicalRadius, m_position.y - m_physicalRadius)
		, Vector2(m_position.x + m_physicalRadius, m_position.y + m_physicalRadius));
}


void Entity::Revive() {
	m_isAlive = true;
}

float Entity::GetHealth() const {
	return m_health;
}

void Entity::Damage(float damageAmount) {

	m_health -= damageAmount;

	if (m_health <= 0.f) {
		m_health = 0.f;
		//Kill();
	}
}

void Entity::Heal(float healAmount) {

	m_health += healAmount;

}


bool Entity::CanFly() const {
	return m_canFly;
}


bool Entity::CanWalk() const {
	return m_canWalk;
}


bool Entity::CanSwim() const {
	return m_canSwim;
}