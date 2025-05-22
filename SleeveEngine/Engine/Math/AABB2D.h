#pragma once
#include "Math/MathCommon.h"
#include "Math/Vec2.h"

template<FloatingPointType T>
struct AABB2D {

	AABB2D() = default;
	AABB2D( Vector2<T> const& mins, Vector2<T> const& maxs );
	Vector2<T> m_mins;
	Vector2<T> m_maxs;

	bool IsPointInside( Vector2<T> const& point ) const;

	static AABB2D<T> const Identity;
};

template<FloatingPointType T>
bool AABB2D<T>::IsPointInside( Vector2<T> const& point ) const
{
	if (point.x < m_maxs.x && point.x > m_mins.x && point.y < m_maxs.y && point.y > m_mins.y) {
		return true;
	}
	return false;
}

AABB2D<float> const AABB2D<float>::Identity = AABB2D( Vector2<float>( 0.f, 0.f ), Vector2<float>( 1.f, 1.f ) );
AABB2D<double> const AABB2D<double>::Identity = AABB2D( Vector2<double>( 0.0, 0.0 ), Vector2<double>( 1.0, 1.0 ) );

template<FloatingPointType T>
AABB2D<T>::AABB2D( Vector2<T> const& mins, Vector2<T> const& maxs )
	:m_mins(mins), m_maxs(maxs)
{

}
