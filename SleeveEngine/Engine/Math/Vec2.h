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
	Vector2( Vector2 const& copyFrom );
	//Vector2( IntVec2 const& copyFrom );
	Vector2( Vector3<T> const& copyFrom );
	explicit Vector2( T initialX, T initialY );
	//bool SetFromText( char const* text );

	// static methods
	static Vector2<T> MakeFromPolarRadians( T orientationRadians, T length = static_cast<T>(1.f) );

	static Vector2<T> MakeFromPolarDegrees( T orientationDegrees, T length = static_cast<T>(1.f) );

	// Accessors
	T GetLength() const;

	T GetLengthSquared() const;

	T GetOrientationRadians() const;

	T GetOrientationDegrees() const;

	Vector2 GetRotated90Degrees() const;

	Vector2 GetRotatedMinus90Degrees() const;

	Vector2 GetRotatedRadians( T deltaRadians ) const;

	Vector2 GetRotatedDegrees( T deltaDegrees ) const;

	//Vec2 const GetRotatedNewBasis( Vec2 const& iBasisNormal ) const;

	Vector2 GetClamped( T maxLength ) const;

	Vector2 GetNormalized() const;

	Vector2 GetReflected( Vector2 const reflectNormal ) const;

	void SetOrientationRadians( float newOrientationRadians );

	void SetOrientationDegrees( float newOrientationDegrees );

	void SetPolarRadians( float newOrientationRadians, float newLength );

	void SetPolarDegrees( float newOrientationDegrees, float newLength );

	void Rotate90Degrees();

	void RotateMinus90Degrees();

	void RotateRadians( float deltaRadians );

	void RotateDegrees( float deltaDegrees );

	//void RotateNewBasis( Vec2 const& iBasisNormal );
	void SetLength( T newLength );

	void ClampLength( T maxLength );

	void Normalize();

	T NormalizeAndGetPreviousLength();

	void Reflect( Vector2 const& NormalizedReflectNormal );

	// Operators (const)
	bool		operator==( Vector2 const& compare ) const;
	bool		operator!=( Vector2 const& compare ) const;
	Vector2 operator+( Vector2 const& vecToAdd ) const;
	Vector2 operator-( Vector2 const& vecToSubtract ) const;
	Vector2 operator-() const;
	Vector2 operator*( T uniformScale ) const;
	Vector2 operator*( Vector2 const& vecToMultiply ) const;
	Vector2 operator/( T inverseScale ) const;

	// Operators (self-mutating / non-const)
	void		operator+=( Vector2 const& vecToAdd );
	void		operator-=( Vector2 const& vecToSubtract );
	void		operator*=( T uniformScale );
	void		operator/=( T uniformDivisor );
	void		operator/=(  Vector2 const& vecToDivide );
	void		operator=( Vector2 const& copyFrom );
	//void		operator=( Vector3 const& copyFrom );


	friend Vector2 operator*( T uniformScale, Vector2 const& vecToScale );
};

template<FloatingPointType T>
Vector2<T>::Vector2( Vector3<T> const& copyFrom )
	: x( copyFrom.x ), y( copyFrom.y ) {}

template<FloatingPointType T>
Vector2<T>::Vector2( Vector2 const& copyFrom )
	: x( copyFrom.x ), y( copyFrom.y ) {}

template<FloatingPointType T>
Vector2<T>::Vector2( T initialX, T initialY )
	: x( initialX ), y( initialY ) {}

template<FloatingPointType T>
void Vector2<T>::operator=( Vector2 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}

template<FloatingPointType T>
void Vector2<T>::operator/=( T uniformDivisor )
{
	T inversedDivisor = static_cast<T>( 1.f ) / uniformDivisor;
	x *= inversedDivisor;
	y *= inversedDivisor;
}

template<FloatingPointType T>
void Vector2<T>::operator*=( T uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}

template<FloatingPointType T>
void Vector2<T>::operator-=( Vector2 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}

template<FloatingPointType T>
void Vector2<T>::operator+=( Vector2 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator/( T inverseScale ) const
{
	T inversedDivisor = static_cast<T>(1.f) / inverseScale;
	return Vector2( x * inversedDivisor, y * inversedDivisor );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator*( Vector2 const& vecToMultiply ) const
{
	return Vector2( x * vecToMultiply.x, y * vecToMultiply.y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator*( T uniformScale ) const
{
	return Vector2( x * uniformScale, y * uniformScale );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator-() const
{
	return Vector2( -x, -y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator-( Vector2 const& vecToSubtract ) const
{
	return Vector2( x - vecToSubtract.x, y - vecToSubtract.y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::operator+( Vector2 const& vecToAdd ) const
{
	return Vector2( x + vecToAdd.x, y + vecToAdd.y );
}

template<FloatingPointType T>
bool Vector2<T>::operator!=( Vector2 const& compare ) const
{
	return x != compare.x || y != compare.y;
}

template<FloatingPointType T>
bool Vector2<T>::operator==( Vector2 const& compare ) const
{
	return x == compare.x && y == compare.y;
}

template<FloatingPointType T>
void Vector2<T>::operator/=( Vector2 const& vecToDivide )
{
	x /= vecToDivide.x;
	y /= vecToDivide.y;
}

template<FloatingPointType T>
void Vector2<T>::Reflect( Vector2 const& NormalizedReflectNormal )
{
	*this = *this - static_cast<T>(2.f) * (DotProduct2D( NormalizedReflectNormal, *this ) * NormalizedReflectNormal);
}

template<FloatingPointType T>
T Vector2<T>::NormalizeAndGetPreviousLength()
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
void Vector2<T>::Normalize()
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
void Vector2<T>::ClampLength( T maxLength )
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		x = x * maxLength / length;
		y = y * maxLength / length;
	}
}

template<FloatingPointType T>
void Vector2<T>::SetLength( T newLength )
{
	if (x != static_cast<T>(0.f) && y != static_cast<T>(0.f)) {
		T length = GetLength();
		x = x * newLength / length;
		y = y * newLength / length;
	}
}

template<FloatingPointType T>
void Vector2<T>::RotateDegrees( float deltaDegrees )
{
	Vector2 iBasis = Vector2::MakeFromPolarDegrees( deltaDegrees );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	*this = iBasis * x + jBasis * y;
}

template<FloatingPointType T>
void Vector2<T>::RotateRadians( float deltaRadians )
{
	Vector2 iBasis = Vector2::MakeFromPolarRadians( deltaRadians );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	*this = iBasis * x + jBasis * y;
}

template<FloatingPointType T>
void Vector2<T>::RotateMinus90Degrees()
{
	std::swap(x, y);
	y = -y;
}

template<FloatingPointType T>
void Vector2<T>::Rotate90Degrees()
{
	std::swap( x, y );
	x = -x;
}

template<FloatingPointType T>
void Vector2<T>::SetPolarDegrees( float newOrientationDegrees, float newLength )
{
	x = CosDegrees( newOrientationDegrees ) * newLength;
	y = SinDegrees( newOrientationDegrees ) * newLength;
}

template<FloatingPointType T>
void Vector2<T>::SetPolarRadians( float newOrientationRadians, float newLength )
{
	x = CosRadians( newOrientationRadians ) * newLength;
	y = SinRadians( newOrientationRadians ) * newLength;
}

template<FloatingPointType T>
void Vector2<T>::SetOrientationDegrees( float newOrientationDegrees )
{
	T length = GetLength();
	x = CosDegrees( newOrientationDegrees ) * length;
	y = SinDegrees( newOrientationDegrees ) * length;
}

template<FloatingPointType T>
void Vector2<T>::SetOrientationRadians( float newOrientationRadians )
{
	T length = GetLength();
	x = CosRadians( newOrientationRadians ) * length;
	y = SinRadians( newOrientationRadians ) * length;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetReflected( Vector2 const reflectNormal ) const
{
	return *this - static_cast<T>(2.f) * (DotProduct2D( reflectNormal, *this ) * reflectNormal);
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetNormalized() const
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f)) {
		return;
	}
	T length = GetLength();
	T scale = static_cast<T>(1.f) / length;
	return Vector2( x * scale, y * scale );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetClamped( T maxLength ) const
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		T ratio = maxLength / length;
		return Vector2( x * ratio, y * ratio );
	}
	return Vector2( x, y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedDegrees( T deltaDegrees ) const
{
	Vector2 iBasis = Vector2::MakeFromPolarDegrees( deltaDegrees );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	return iBasis * x + jBasis * y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedRadians( T deltaRadians ) const
{
	Vector2 iBasis = Vector2::MakeFromPolarRadians( deltaRadians );
	Vector2 jBasis = iBasis.GetRotated90Degrees();
	return iBasis * x + jBasis * y;
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotatedMinus90Degrees() const
{
	return Vector2( y, -x );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::GetRotated90Degrees() const
{
	return Vector2( -y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetOrientationDegrees() const
{
	return Atan2Degrees( y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetOrientationRadians() const
{
	return Atan2Radians( y, x );
}

template<FloatingPointType T>
T Vector2<T>::GetLengthSquared() const
{
	return x * x + y * y;
}

template<FloatingPointType T>
T Vector2<T>::GetLength() const
{
	return std::sqrt( x * x + y * y );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::MakeFromPolarDegrees( T orientationDegrees, T length )
{
	return Vector2( CosDegrees( orientationDegrees ) * length, SinDegrees( orientationDegrees ) * length );
}

template<FloatingPointType T>
Vector2<T> Vector2<T>::MakeFromPolarRadians( T orientationRadians, T length )
{
	return Vector2( CosRadians( orientationRadians ) * length, CosRadians( orientationRadians ) * length );
}

template<FloatingPointType T>
Vector2<T> operator*( T uniformScale, Vector2<T> const& vecToScale ) {
	return Vector2( vecToScale.x * uniformScale, vecToScale.y * uniformScale );
}