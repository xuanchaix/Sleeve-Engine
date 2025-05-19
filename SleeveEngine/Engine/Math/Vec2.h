#pragma once
#include "Math/MathCommon.h"
#include "Math/MathUtils.h"
#include "Math/Vec3.h"

template<FloatingPointType T>
struct Vector2 {
public:
	T x = T{};
	T y = T{};

public:
	// Construction/Destruction
	~Vector2() {}
	Vector2() = default;
	Vector2( Vector2 const& copyFrom ) noexcept;
	//Vector2( IntVec2 const& copyFrom );
	Vector2( Vector3<T> const& copyFrom ) noexcept;
	explicit Vector2( T initialX, T initialY ) noexcept;
	//bool SetFromText( char const* text );

	// static methods
	static Vector2<T> MakeFromPolarRadians( T orientationRadians, T length = static_cast<T>(1.f) ) noexcept;

	static Vector2<T> MakeFromPolarDegrees( T orientationDegrees, T length = static_cast<T>(1.f) ) noexcept;

	// Accessors
	T GetLength() const noexcept;

	T GetLengthSquared() const noexcept;

	T GetOrientationRadians() const noexcept;

	T GetOrientationDegrees() const noexcept;

	Vector2 GetRotated90Degrees() const noexcept;

	Vector2 GetRotatedMinus90Degrees() const noexcept;

	Vector2 GetRotatedRadians( T deltaRadians ) const noexcept;

	Vector2 GetRotatedDegrees( T deltaDegrees ) const noexcept;

	//Vec2 const GetRotatedNewBasis( Vec2 const& iBasisNormal ) const;

	Vector2 GetClamped( T maxLength ) const noexcept;

	Vector2 GetNormalized() const noexcept;

	Vector2 GetReflected( Vector2 const reflectNormal ) const noexcept;

	void SetOrientationRadians( float newOrientationRadians ) noexcept;

	void SetOrientationDegrees( float newOrientationDegrees ) noexcept;

	void SetPolarRadians( float newOrientationRadians, float newLength ) noexcept;

	void SetPolarDegrees( float newOrientationDegrees, float newLength ) noexcept;

	void Rotate90Degrees() noexcept;

	void RotateMinus90Degrees() noexcept;

	void RotateRadians( float deltaRadians ) noexcept;

	void RotateDegrees( float deltaDegrees ) noexcept;

	//void RotateNewBasis( Vec2 const& iBasisNormal );
	void SetLength( T newLength ) noexcept;

	void ClampLength( T maxLength ) noexcept;

	void Normalize() noexcept;

	T NormalizeAndGetPreviousLength() noexcept;

	void Reflect( Vector2 const& NormalizedReflectNormal ) noexcept;

	// Operators (const)
	bool		operator==( Vector2 const& compare ) const noexcept;
	bool		operator!=( Vector2 const& compare ) const noexcept;
	Vector2 operator+( Vector2 const& vecToAdd ) const noexcept;
	Vector2 operator-( Vector2 const& vecToSubtract ) const noexcept;
	Vector2 operator-() const noexcept;
	Vector2 operator*( T uniformScale ) const noexcept;
	Vector2 operator*( Vector2 const& vecToMultiply ) const noexcept;
	Vector2 operator/( T inverseScale ) const noexcept;

	// Operators (self-mutating / non-const)
	void		operator+=( Vector2 const& vecToAdd ) noexcept;
	void		operator-=( Vector2 const& vecToSubtract ) noexcept;
	void		operator*=( T uniformScale ) noexcept;
	void		operator/=( T uniformDivisor ) noexcept;
	void		operator/=(  Vector2 const& vecToDivide ) noexcept;
	void		operator=( Vector2 const& copyFrom ) noexcept;
	//void		operator=( Vector3 const& copyFrom );


	friend Vector2 operator*( T uniformScale, Vector2 const& vecToScale ) noexcept;
};

template<FloatingPointType T>
Vector2<T>::Vector2( Vector3<T> const& copyFrom ) noexcept
	: x( copyFrom.x ), y( copyFrom.y ) {}

template<FloatingPointType T>
Vector2<T>::Vector2( Vector2 const& copyFrom ) noexcept
	: x( copyFrom.x ), y( copyFrom.y ) {}

template<FloatingPointType T>
Vector2<T>::Vector2( T initialX, T initialY ) noexcept
	: x( initialX ), y( initialY ) {}

template<FloatingPointType T>
void Vector2<T>::operator=( Vector2 const& copyFrom ) noexcept
{
	x = copyFrom.x;
	y = copyFrom.y;
}

template<FloatingPointType T>
void Vector2<T>::operator/=( T uniformDivisor ) noexcept
{
	T inversedDivisor = static_cast<T>( 1.f ) / uniformDivisor;
	x *= inversedDivisor;
	y *= inversedDivisor;
}

template<FloatingPointType T>
void Vector2<T>::operator*=( T uniformScale ) noexcept
{
	x *= uniformScale;
	y *= uniformScale;
}

template<FloatingPointType T>
void Vector2<T>::operator-=( Vector2 const& vecToSubtract ) noexcept
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

template<FloatingPointType T>
void Vector2<T>::operator+=( Vector2 const& vecToAdd ) noexcept
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator/( T inverseScale ) const noexcept
{
	T inversedDivisor = static_cast<T>(1.f) / inverseScale;
	return Vector2( x * inversedDivisor, y * inversedDivisor );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator*( Vector2 const& vecToMultiply ) const noexcept
{
	return Vector2( x * vecToMultiply.x, y * vecToMultiply.y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator*( T uniformScale ) const noexcept
{
	return Vector2( x * uniformScale, y * uniformScale );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator-() const noexcept
{
	return Vector2( -x, -y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator-( Vector2 const& vecToSubtract ) const noexcept
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator+( Vector2 const& vecToAdd ) const noexcept
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y );
}

template<FloatingPointType T>
bool Vector2<T>::operator!=( Vector2 const& compare ) const noexcept
{
	return x != compare.x || y != compare.y;
}

template<FloatingPointType T>
bool Vector2<T>::operator==( Vector2 const& compare ) const noexcept
{
	return x == compare.x && y == compare.y;
}

template<FloatingPointType T>
void Vector2<T>::operator/=( Vector2 const& vecToDivide ) noexcept
{
	x /= vecToDivide.x;
	y /= vecToDivide.y;
}

template<FloatingPointType T>
void Vector2<T>::Reflect( Vector2 const& NormalizedReflectNormal ) noexcept
{
	*this = *this - static_cast<T>(2.f) * (DotProduct2D( NormalizedReflectNormal, *this ) * NormalizedReflectNormal);
}

template<FloatingPointType T>
T Vector2<T>::NormalizeAndGetPreviousLength() noexcept
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f)) {
		return static_cast<T>(0.f);
	}
	T length = GetLength();
	T scale = static_cast<T>(1.f) / length;
	x *= scale;
	y *= scale;
	return length;
}

template<FloatingPointType T>
void Vector2<T>::Normalize() noexcept
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f)) {
		return;
	}
	T length = GetLength();
	T scale = static_cast<T>(1.f) / length;
	x *= scale;
	y *= scale;
}

template<FloatingPointType T>
void Vector2<T>::ClampLength( T maxLength ) noexcept
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		x = x * maxLength / length;
		y = y * maxLength / length;
	}
}

template<FloatingPointType T>
void Vector2<T>::SetLength( T newLength ) noexcept
{
	if (x != static_cast<T>(0.f) && y != static_cast<T>(0.f)) {
		T length = GetLength();
		x = x * newLength / length;
		y = y * newLength / length;
	}
}

template<FloatingPointType T>
void Vector2<T>::RotateDegrees( float deltaDegrees ) noexcept
{
	Vector2 iBasis = Vector2::MakeFromPolarDegrees( deltaDegrees );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	*this = iBasis * x + jBasis * y;
}

template<FloatingPointType T>
void Vector2<T>::RotateRadians( float deltaRadians ) noexcept
{
	Vector2 iBasis = Vector2::MakeFromPolarRadians( deltaRadians );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	*this = iBasis * x + jBasis * y;
}

template<FloatingPointType T>
void Vector2<T>::RotateMinus90Degrees() noexcept
{
	std::swap(x, y);
	y = -y;
}

template<FloatingPointType T>
void Vector2<T>::Rotate90Degrees() noexcept
{
	std::swap( x, y );
	x = -x;
}

template<FloatingPointType T>
void Vector2<T>::SetPolarDegrees( float newOrientationDegrees, float newLength ) noexcept
{
	x = CosDegrees( newOrientationDegrees ) * newLength;
	y = SinDegrees( newOrientationDegrees ) * newLength;
}

template<FloatingPointType T>
void Vector2<T>::SetPolarRadians( float newOrientationRadians, float newLength ) noexcept
{
	x = CosRadians( newOrientationRadians ) * newLength;
	y = SinRadians( newOrientationRadians ) * newLength;
}

template<FloatingPointType T>
void Vector2<T>::SetOrientationDegrees( float newOrientationDegrees ) noexcept
{
	T length = GetLength();
	x = CosDegrees( newOrientationDegrees ) * length;
	y = SinDegrees( newOrientationDegrees ) * length;
}

template<FloatingPointType T>
void Vector2<T>::SetOrientationRadians( float newOrientationRadians ) noexcept
{
	T length = GetLength();
	x = CosRadians( newOrientationRadians ) * length;
	y = SinRadians( newOrientationRadians ) * length;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetReflected( Vector2 const reflectNormal ) const noexcept
{
	return *this - static_cast<T>(2.f) * (DotProduct2D( reflectNormal, *this ) * reflectNormal);
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetNormalized() const noexcept
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f)) {
		return;
	}
	T length = GetLength();
	T scale = static_cast<T>(1.f) / length;
	return Vector2( x * scale, y * scale );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetClamped( T maxLength ) const noexcept
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		T ratio = maxLength / length;
		return Vector2( x * ratio, y * ratio );
	}
	return Vector2( x, y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedDegrees( T deltaDegrees ) const noexcept
{
	Vector2 iBasis = Vector2::MakeFromPolarDegrees( deltaDegrees );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	return iBasis * x + jBasis * y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedRadians( T deltaRadians ) const noexcept
{
	Vector2 iBasis = Vector2::MakeFromPolarRadians( deltaRadians );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	return iBasis * x + jBasis * y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedMinus90Degrees() const noexcept
{
	return Vector2( y, -x );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotated90Degrees() const noexcept
{
	return Vector2( -y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetOrientationDegrees() const noexcept
{
	return Atan2Degrees( y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetOrientationRadians() const noexcept
{
	return Atan2Radians( y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetLengthSquared() const noexcept
{
	return x * x + y * y;
}

template<FloatingPointType T>
T Vector2<T>::GetLength() const noexcept
{
	return std::sqrt( x * x + y * y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::MakeFromPolarDegrees( T orientationDegrees, T length ) noexcept
{
	return Vector2( CosDegrees( orientationDegrees ) * length, SinDegrees( orientationDegrees ) * length );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::MakeFromPolarRadians( T orientationRadians, T length ) noexcept
{
	return Vector2( CosRadians( orientationRadians ) * length, CosRadians( orientationRadians ) * length );
}

template<FloatingPointType T>
Vector2<T> operator*( T uniformScale, Vector2<T> const& vecToScale )  noexcept {
	return Vector2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}