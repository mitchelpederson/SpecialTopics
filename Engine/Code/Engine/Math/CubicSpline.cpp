#include "Engine/Math/CubicSpline.hpp"


CubicSpline2D::CubicSpline2D( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/* =nullptr */ ) {
	for (int posIndex = 0; posIndex < numPoints; posIndex++) {
		m_positions.push_back(positionsArray[posIndex]);

		if (velocitiesArray != nullptr) {
			m_velocities.push_back(velocitiesArray[posIndex]);
		}
		else {
			m_velocities.push_back(Vector2::ZERO);
		}
	}
}


int CubicSpline2D::GetPositions( std::vector<Vector2>& out_positions ) const {
	out_positions = m_positions;
	return (int) m_positions.size();
}


int CubicSpline2D::GetVelocities( std::vector<Vector2>& out_velocities ) const {
	out_velocities = m_velocities;
	return (int) m_velocities.size();
}


const Vector2 CubicSpline2D::GetPosition( int pointIndex ) const {
	return m_positions[pointIndex];
}


const Vector2 CubicSpline2D::GetVelocity( int pointIndex ) const {
	return m_velocities[pointIndex];
}


Vector2 CubicSpline2D::EvaluateAtCumulativeParametric( float t ) const {
	int splineStartIndex = (int) t;
	float fractionInSplineSegment = fmodf(t, 1.f);

	return EvaluateCubicHermite(m_positions[splineStartIndex], m_velocities[splineStartIndex], m_positions[splineStartIndex + 1], m_velocities[splineStartIndex + 1], fractionInSplineSegment);

}

Vector2 CubicSpline2D::EvaluateAtNormalizedParametric( float t ) const {
	float posInSpline = Interpolate( 0.f, (float) m_positions.size() - 1, t);
	Vector2 answer = EvaluateAtCumulativeParametric(posInSpline);
	return answer;
}


void CubicSpline2D::AppendPoint( const Vector2& position, const Vector2& velocity/* =Vector2::ZERO */ ) {
	m_positions.push_back(position);
	m_velocities.push_back(velocity);
}


void CubicSpline2D::AppendPoints( const Vector2* positionsArray, int numPoints, const Vector2* velocitiesArray/* =nullptr */ ) {
	for (int posIndex = 0; posIndex < numPoints; posIndex++) {
		m_positions.push_back(positionsArray[posIndex]);

		if (velocitiesArray != nullptr) {
			m_velocities.push_back(velocitiesArray[posIndex]);
		}
		else {
			m_velocities.push_back(Vector2::ZERO);
		}
	}
}


void CubicSpline2D::InsertPoint( int insertBeforeIndex, const Vector2& position, const Vector2& velocity/* =Vector2::ZERO */ ) {
	m_positions.insert(m_positions.begin() + insertBeforeIndex, position);
	m_velocities.insert(m_velocities.begin() + insertBeforeIndex, velocity);
}

void CubicSpline2D::RemovePoint( int pointIndex ) {
	m_positions.erase(m_positions.begin() + pointIndex);
	m_velocities.erase(m_velocities.begin() + pointIndex);
}


void CubicSpline2D::RemoveAllPoints() {
	m_positions.clear();
	m_velocities.clear();
}


void CubicSpline2D::SetPoint( int pointIndex, const Vector2& newPosition, const Vector2& newVelocity ) {
	m_positions[pointIndex] = newPosition;
	m_velocities[pointIndex] = newVelocity;
}


void CubicSpline2D::SetPosition( int pointIndex, const Vector2& newPosition ) {
	m_positions[pointIndex] = newPosition;
}


void CubicSpline2D::SetVelocity( int pointIndex, const Vector2& newVelocity ) {
	m_velocities[pointIndex] = newVelocity;
}


void CubicSpline2D::SetCardinalVelocities( float tension/* =0.f */, const Vector2& startVelocity/* =Vector2::ZERO */, const Vector2& endVelocity/* =Vector2::ZERO */ ) {
	m_tension = tension;
	m_velocities[0] = startVelocity;
	m_velocities[m_velocities.size() - 1] = endVelocity;

	for (int velIndex = 1; velIndex < m_velocities.size() - 1; velIndex++) {
		m_velocities[velIndex] = (m_positions[velIndex + 1] - m_positions[velIndex - 1]) * 0.5f * (1.f - m_tension);
	}
}

