#pragma once
#include "Math/MathCommon.h"

template<FloatingPointType T>
struct Vector2;
template<FloatingPointType T>
struct Vector3;
template<FloatingPointType T>
struct Vector4;

//---------------------------
// Clamp and lerp

/// Clamp the value into the two bounds, the caller needs to make sure maxValue > minValue
template<FloatingPointType T>
T GetClamped( T value, T minValue, T maxValue );
/// Clamp the value into the two bounds, the caller needs to make sure maxValue > minValue
inline int GetClamped( int value, int minValue, int maxValue );
/// Clamp the value into 0 to 1
template<FloatingPointType T>
T GetClampedZeroToOne( T value );
/// Get the value between start and end by faction (float)
inline float Interpolate( float start, float end, float fractionTowardEnd );
/// Get the value between start and end by faction (double)
inline double Interpolate( double start, double end, double fractionTowardEnd );
/// Get the value between start and end by faction
template<FloatingPointType T>
Vector2<T> Interpolate( Vector2<T> const& start, Vector2<T> const& end, T fractionTowardEnd );
/// Get the value between start and end by faction
template<FloatingPointType T>
Vector3<T> Interpolate( Vector3<T> const& start, Vector3<T> const& end, T fractionTowardEnd );
/// Get the value between start and end by faction
template<FloatingPointType T>
Vector4<T> Interpolate( Vector4<T> const& start, Vector4<T> const& end, T fractionTowardEnd );
/// Get the faction of the value between start and end
template<FloatingPointType T>
T GetFractionWithinRange( T value, T rangeStart, T rangeEnd );
/// Map the value from range in to range out
template<FloatingPointType T>
T RangeMap( T inValue, T inStart, T inEnd, T outStart, T outEnd );
/// Clamp and Map the value from range in to range out
template<FloatingPointType T>
T RangeMapClamped( T inValue, T inStart, T inEnd, T outStart, T outEnd );
/// Round down a float to a int
inline int RoundDownToInt( float value );
/// Round the float to a nearest integer
inline int RoundToInt( float value );

//----------------------------
// Angle utilities

/// Convert degrees to radians
template<FloatingPointType T>
T ConvertDegreesToRadians( T degrees );
/// Convert degrees to radians
template<FloatingPointType T>
T ConvertRadiansToDegrees( T radians );
/// Calculate the cosine value of a degree value
template<FloatingPointType T>
T CosDegrees( T degrees );
/// Calculate the sine value of a degree value
template<FloatingPointType T>
T SinDegrees( T degrees );
/// Calculate the cosine value of a radian value
template<FloatingPointType T>
T CosRadians( T radians );
/// Calculate the sine value of a radian value
template<FloatingPointType T>
T SinRadians( T radians );
/// Calculate the orientation degrees by position 
template<FloatingPointType T>
T Atan2Degrees( T y, T x );
/// Calculate the orientation degrees by position
template<FloatingPointType T>
T Atan2Radians( T y, T x );
/// Calculate the shortest displacement degrees from start to end
template<FloatingPointType T>
T GetShortestAngularDispDegrees( T startDegrees, T endDegrees );
// Has max delta degrees to turn, try best to turn to goal degrees from current degrees
template<FloatingPointType T>
T GetTurnedTowardDegrees( T currentDegrees, T goalDegrees, T maxDeltaDegrees );
// Get the angular degrees between two vectors 
template<FloatingPointType T>
T GetAngleDegreesBetweenVectors2D( Vector2<T> const& a, Vector2<T> const& b );
/// Get rotate back(inversed) normalized iBasis
template<FloatingPointType T>
Vector2<T> GetInversedOrthonormaliBasis( Vector2<T> const& iBasisNormal );
/// Normalize degrees to -180 - 180
template<FloatingPointType T>
T GetNormalizedDegrees180( T degrees );

//---------------------------------------
// Products

/// Calculate the dot product of two vectors 
template<FloatingPointType T>
T DotProduct2D( Vector2<T> const& a, Vector2<T> const& b );
/// Calculate the dot product of two vectors 
template<FloatingPointType T>
T DotProduct3D( Vector3<T> const& a, Vector3<T> const& b );
/// Calculate the dot product of two vectors 
template<FloatingPointType T>
T DotProduct4D( Vector4<T> const& a, Vector4<T> const& b );
/// Calculate the cross product of two vectors
template<FloatingPointType T>
T CrossProduct2D( Vector2<T> const& a, Vector2<T> const& b );
/// Calculate the cross product of two vectors
template<FloatingPointType T>
Vector3<T> CrossProduct3D( Vector3<T> const& a, Vector3<T> const& b );

//---------------------------------------
// Basic 2D & 3D utilities
// Distance & projections utilities

/// Get distance between two 2D position
template<FloatingPointType T>
T GetDistance2D( Vector2<T> const& positionA, Vector2<T> const& positionB );
/// Get squared distance between two 2D position - No Squared Root
template<FloatingPointType T>
T GetDistanceSquared2D( Vector2<T> const& positionA, Vector2<T> const& positionB );
/// Get Manhattan distance between two 2D points
template<FloatingPointType T>
T GetTaxicabDistance2D( Vector2<T> const& positionA, Vector2<T> const& positionB );
/// Get distance between two 3D position
template<FloatingPointType T>
T GetDistance3D( Vector3<T> const& positionA, Vector3<T> const& positionB );
/// Get squared distance between two 3D position - No Squared Root
template<FloatingPointType T>
T GetDistanceSquared3D( Vector3<T> const& positionA, Vector3<T> const& positionB );
/// Get distance on XY between two 3D position
template<FloatingPointType T>
T GetDistanceXY3D( Vector3<T> const& positionA, Vector3<T> const& positionB );
/// Get squared distance on XY between two 3D position - No Squared Root
template<FloatingPointType T>
T GetDistanceXYSquared3D( Vector3<T> const& positionA, Vector3<T> const& positionB );
/// Get Manhattan distance between two int coordinate points
//int GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
/// Get the length of projection from one vector to another
template<FloatingPointType T>
T GetProjectedLength2D( Vector2<T> const& vectorToProject, Vector2<T> const& vectorToProjectOnto );
/// Get the vector of projection from one vector to another
template<FloatingPointType T>
Vector2<T> GetProjectedOnto2D( Vector2<T> const& vectorToProject, Vector2<T> const& vectorToProjectOnto );

///
template<FloatingPointType T>
T ComputeCubicBezier1D( T A, T B, T C, T D, T t );
/// 
template<FloatingPointType T>
T ComputeQuinticBezier1D( T A, T B, T C, T D, T E, T F, T t );
/// t * t
template<FloatingPointType T>
T SmoothStart2( T t );
/// t * t * t
template<FloatingPointType T>
T SmoothStart3( T t );
/// t * t * t * t
template<FloatingPointType T>
T SmoothStart4( T t );
/// t * t * t * t * t
template<FloatingPointType T>
T SmoothStart5( T t );
/// t * t * t * t * t * t
template<FloatingPointType T>
T SmoothStart6( T t );
/// 1 - (1 - t) ^ 2
template<FloatingPointType T>
T SmoothStop2( T t );
/// 1 - (1 - t) ^ 3
template<FloatingPointType T>
T SmoothStop3( T t );
/// 1 - (1 - t) ^ 4
template<FloatingPointType T>
T SmoothStop4( T t );
/// 1 - (1 - t) ^ 5
template<FloatingPointType T>
T SmoothStop5( T t );
/// 1 - (1 - t) ^ 6
template<FloatingPointType T>
T SmoothStop6( T t );
/// first slow then fast then slow
template<FloatingPointType T>
T SmoothStep3( T t );
/// first slow then fast then slow
template<FloatingPointType T>
T SmoothStep5( T t );
/// first fast then slow then fast
template<FloatingPointType T>
T Hesitate3( T t );
/// first fast then slow then fast
template<FloatingPointType T>
T Hesitate5( T t );

#include "MathUtils.inl"