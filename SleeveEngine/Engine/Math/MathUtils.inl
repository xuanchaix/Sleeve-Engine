#pragma once
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

template<FloatingPointType T>
T GetClamped( T value, T minValue, T maxValue )
{
	if (value <= minValue) {
		return minValue;
	}
	if (value >= maxValue) {
		return maxValue;
	}
	return value;
}

int GetClamped( int value, int minValue, int maxValue )
{
	if (value <= minValue) {
		return minValue;
	}
	if (value >= maxValue) {
		return maxValue;
	}
	return value;
}

template<FloatingPointType T>
T GetClampedZeroToOne( T value )
{
	if (value < static_cast<T>(0.f)) {
		return static_cast<T>(0.f);
	}
	if (value > static_cast<T>(1.f)) {
		return static_cast<T>(1.f);
	}
	return value;
}

float Interpolate( float start, float end, float fractionTowardEnd )
{
	return (end - start) * fractionTowardEnd + start;
}

double Interpolate( double start, double end, double fractionTowardEnd )
{
	return (end - start) * fractionTowardEnd + start;
}

template<FloatingPointType T>
Vector2<T> Interpolate( Vector2<T> const& start, Vector2<T> const& end, T fractionTowardEnd )
{
	return (end - start) * fractionTowardEnd + start;
}

template<FloatingPointType T>
Vector3<T> Interpolate( Vector3<T> const& start, Vector3<T> const& end, T fractionTowardEnd )
{
	return (end - start) * fractionTowardEnd + start;
}

template<FloatingPointType T>
Vector4<T> Interpolate( Vector4<T> const& start, Vector4<T> const& end, T fractionTowardEnd )
{
	return (end - start) * fractionTowardEnd + start;
}

template<FloatingPointType T>
T GetFractionWithinRange( T value, T rangeStart, T rangeEnd )
{
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

template<FloatingPointType T>
T RangeMap( T inValue, T inStart, T inEnd, T outStart, T outEnd )
{
	return Interpolate( outStart, outEnd, GetFractionWithinRange( inValue, inStart, inEnd ) );
}

template<FloatingPointType T>
T RangeMapClamped( T inValue, T inStart, T inEnd, T outStart, T outEnd )
{
	if (inValue < inStart) {
		inValue = inStart;
	}
	if (inValue > inEnd) {
		inValue = inEnd;
	}
	return Interpolate( outStart, outEnd, GetFractionWithinRange( inValue, inStart, inEnd ) );
}

int RoundDownToInt( float value )
{
	return (int)floorf( value );
}

int RoundToInt( float value )
{
	return (int)floorf( value + 0.5f );
}

template<FloatingPointType T>
T ConvertDegreesToRadians( T degrees )
{
	return degrees / static_cast<T>(180.f) * static_cast<T>(PI);
}

template<FloatingPointType T>
T ConvertRadiansToDegrees( T radians )
{
	return radians * static_cast<T>(180.f) / static_cast<T>(PI);
}

template<FloatingPointType T>
T CosDegrees( T degrees )
{
	return std::cos( ConvertDegreesToRadians( degrees ) );
}

template<FloatingPointType T>
T SinDegrees( T degrees )
{
	return std::sin( ConvertDegreesToRadians( degrees ) );
}

template<FloatingPointType T>
T CosRadians( T radians )
{
	return std::cos( radians );
}

template<FloatingPointType T>
T SinRadians( T radians )
{
	return std::sin( radians );
}

template<FloatingPointType T>
T Atan2Degrees( T y, T x )
{
	return ConvertRadiansToDegrees( std::atan2( y, x ) );
}

template<FloatingPointType T>
T Atan2Radians( T y, T x )
{
	return std::atan2( y, x );
}

template<FloatingPointType T>
T GetShortestAngularDispDegrees( T startDegrees, T endDegrees )
{
	T dispDegrees = endDegrees - startDegrees;
	return GetNormalizedDegrees180( dispDegrees );
}

template<FloatingPointType T>
T GetTurnedTowardDegrees( T currentDegrees, T goalDegrees, T maxDeltaDegrees )
{
	T shortestDispDegrees = GetShortestAngularDispDegrees( currentDegrees, goalDegrees );
	if (std::abs( shortestDispDegrees ) <= maxDeltaDegrees) {
		return goalDegrees;
	}
	else {
		if (shortestDispDegrees < 0.f) {
			return GetNormalizedDegrees180( currentDegrees - maxDeltaDegrees );
		}
		else {
			return GetNormalizedDegrees180( currentDegrees + maxDeltaDegrees );
		}
	}
}

template<FloatingPointType T>
T GetAngleDegreesBetweenVectors2D( Vector2<T> const& a, Vector2<T> const& b )
{
	return ConvertRadiansToDegrees( std::acos( DotProduct2D( a, b ) / a.GetLength() / b.GetLength() ) );
}

template<FloatingPointType T>
Vector2<T> GetInversedOrthonormaliBasis( Vector2<T> const& iBasisNormal )
{
	return Vector2( iBasisNormal.x, -iBasisNormal.y );
}

template<FloatingPointType T>
T GetNormalizedDegrees180( T degrees )
{
	while (degrees > static_cast<T>(180.f)) {
		degrees -= static_cast<T>(360.f);
	}
	while (degrees <= static_cast<T>(-180.f)) {
		degrees += static_cast<T>(360.f);
	}
	return degrees;
}


template<FloatingPointType T>
T DotProduct2D( Vector2<T> const& a, Vector2<T> const& b )
{
	return a.x * b.x + a.y * b.y;
}

template<FloatingPointType T>
T DotProduct3D( Vector3<T> const& a, Vector3<T> const& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

template<FloatingPointType T>
T DotProduct4D( Vector4<T> const& a, Vector4<T> const& b )
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

template<FloatingPointType T>
T CrossProduct2D( Vector2<T> const& a, Vector2<T> const& b )
{
	return a.x * b.y - b.x * a.y;
}

template<FloatingPointType T>
Vector3<T> CrossProduct3D( Vector3<T> const& a, Vector3<T> const& b )
{
	return Vector3( a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x );
}

template<FloatingPointType T>
T GetDistance2D( Vector2<T> const& positionA, Vector2<T> const& positionB )
{
	return std::sqrt( (positionA.x - positionB.x) * (positionA.x - positionB.x) + (positionA.y - positionB.y) * (positionA.y - positionB.y) );
}

template<FloatingPointType T>
T GetDistanceSquared2D( Vector2<T> const& positionA, Vector2<T> const& positionB )
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x) + (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

template<FloatingPointType T>
T GetTaxicabDistance2D( Vector2<T> const& positionA, Vector2<T> const& positionB )
{
	return std::abs( positionA.x - positionB.x ) + std::abs( positionA.y - positionB.y );
}

template<FloatingPointType T>
T GetDistance3D( Vector3<T> const& positionA, Vector3<T> const& positionB )
{
	return std::sqrt(
		  (positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y)
		+ (positionA.z - positionB.z) * (positionA.z - positionB.z) );
}

template<FloatingPointType T>
T GetDistanceSquared3D( Vector3<T> const& positionA, Vector3<T> const& positionB )
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x)
		 + (positionA.y - positionB.y) * (positionA.y - positionB.y)
		 + (positionA.z - positionB.z) * (positionA.z - positionB.z);
}

template<FloatingPointType T>
T GetDistanceXY3D( Vector3<T> const& positionA, Vector3<T> const& positionB )
{
	return std::sqrt(
		  (positionA.x - positionB.x) * (positionA.x - positionB.x)
		+ (positionA.y - positionB.y) * (positionA.y - positionB.y));
}

template<FloatingPointType T>
T GetDistanceXYSquared3D( Vector3<T> const& positionA, Vector3<T> const& positionB )
{
	return (positionA.x - positionB.x) * (positionA.x - positionB.x)
		 + (positionA.y - positionB.y) * (positionA.y - positionB.y);
}

template<FloatingPointType T>
T GetProjectedLength2D( Vector2<T> const& vectorToProject, Vector2<T> const& vectorToProjectOnto )
{
	Vector2 normal = vectorToProjectOnto.GetNormalized();
	return DotProduct2D( vectorToProject, normal );
}

template<FloatingPointType T>
Vector2<T> GetProjectedOnto2D( Vector2<T> const& vectorToProject, Vector2<T> const& vectorToProjectOnto )
{
	Vector2 normal = vectorToProjectOnto.GetNormalized();
	return normal * DotProduct2D( vectorToProject, normal );
}

