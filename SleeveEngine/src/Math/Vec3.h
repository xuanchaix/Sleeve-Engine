#pragma once
#include "Math/MathCommon.h"
#include "Math/MathUtils.h"
#include "Math/Vec2.h"

template<FloatingPointType T>
struct Vector3 {
public:
	T x = T{};
	T y = T{};
	T z = T{};

public:
	// Construction/Destruction
	~Vector3() {}
	Vector3() = default;
	Vector3( Vector3 const& copyFrom );
	//Vector2( IntVec2 const& copyFrom );
	Vector3( Vector2<T> const& copyFrom );
	explicit Vector3( T X, T Y, T Z );
	//bool SetFromText( char const* text );

	// Accessors
	T GetLength() const;

	T GetLengthSquared() const;

	//Vec2 const GetRotatedNewBasis( Vec2 const& iBasisNormal ) const;

	Vector3 GetClamped( T maxLength ) const;

	Vector3 GetNormalized() const;

	void SetLength( T newLength );

	void ClampLength( T maxLength );

	void Normalize();

	// Operators (const)
	bool		operator==( Vector3 const& compare ) const;
	bool		operator!=( Vector3 const& compare ) const;
	Vector3 operator+( Vector3 const& vecToAdd ) const;
	Vector3 operator-( Vector3 const& vecToSubtract ) const;
	Vector3 operator-() const;
	Vector3 operator*( T uniformScale ) const;
	Vector3 operator*( Vector3 const& vecToMultiply ) const;
	Vector3 operator/( T inverseScale ) const;

	// Operators (self-mutating / non-const)
	void		operator+=( Vector3 const& vecToAdd );
	void		operator-=( Vector3 const& vecToSubtract );
	void		operator*=( T uniformScale );
	void		operator/=( T uniformDivisor );
	void		operator=( Vector2<T> const& copyFrom );
	void		operator=( Vector3 const& copyFrom );


	friend Vector3 operator*( T uniformScale, Vector3 const& vecToScale );
};

template<FloatingPointType T>
void Vector3<T>::operator=( Vector3 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

template<FloatingPointType T>
void Vector3<T>::operator=( Vector2<T> const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = static_cast<T>(0.f);
}

template<FloatingPointType T>
void Vector3<T>::operator/=( T uniformDivisor )
{
	T inversedDivisor = static_cast<T>(1.f) / uniformDivisor;
	x *= inversedDivisor;
	y *= inversedDivisor;
	z *= inversedDivisor;
}

template<FloatingPointType T>
void Vector3<T>::operator*=( T uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

template<FloatingPointType T>
void Vector3<T>::operator-=( Vector3 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

template<FloatingPointType T>
void Vector3<T>::operator+=( Vector3 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator/( T inverseScale ) const
{
	T inversedDivisor = static_cast<T>(1.f) / inverseScale;
	return Vector3( x * inversedDivisor, y * inversedDivisor, z * inversedDivisor );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator*( Vector3 const& vecToMultiply ) const
{
	return Vector3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator*( T uniformScale ) const
{
	return Vector3( x * uniformScale, y * uniformScale, z * uniformScale );
}

template<FloatingPointType T>
Vector3<T> operator*( T uniformScale, Vector3<T> const& vecToScale ) 
{
	return Vector3( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator-() const
{
	return Vector3( -x, -y, -z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator-( Vector3 const& vecToSubtract ) const
{
	return Vector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator+( Vector3 const& vecToAdd ) const
{
	return Vector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

template<FloatingPointType T>
bool Vector3<T>::operator!=( Vector3 const& compare ) const
{
	return x != compare.x || y != compare.y || z != compare.z;
}

template<FloatingPointType T>
bool Vector3<T>::operator==( Vector3 const& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z;
}

template<FloatingPointType T>
void Vector3<T>::Normalize()
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f) && z == static_cast<T>(0.f)) {
		return;
	}
	T inversedLength = static_cast<T>(1.f) / GetLength();
	x *= inversedLength;
	y *= inversedLength;
	z *= inversedLength;
}

template<FloatingPointType T>
void Vector3<T>::ClampLength( T maxLength )
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		float ratio = maxLength / length;
		x = x * ratio;
		y = y * ratio;
		z = z * ratio;
	}
}

template<FloatingPointType T>
void Vector3<T>::SetLength( T newLength )
{
	if (x != static_cast<T>(0.f) && y != static_cast<T>(0.f) && z != static_cast<T>(0.f)) {
		T ratio = newLength * GetLength();
		x = x * ratio;
		y = y * ratio;
		z = z * ratio;
	}
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::GetNormalized() const
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f) && z == static_cast<T>(0.f)) {
		return Vector3();
	}
	T inversedLength = static_cast<T>(1.f) / GetLength();
	return Vector3( x * inversedLength, y * inversedLength, z * inversedLength );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::GetClamped( T maxLength ) const
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		float ratio = maxLength / length;
		return Vector3( x * ratio, y * ratio, z * ratio );
	}
	return Vector3( x, y, z );
}

template<FloatingPointType T>
T Vector3<T>::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

template<FloatingPointType T>
T Vector3<T>::GetLength() const
{
	return std::sqrt( x * x + y * y + z * z );
}

template<FloatingPointType T>
Vector3<T>::Vector3( T X, T Y, T Z )
	:x(X), y(Y), z(Z)
{

}

template<FloatingPointType T>
Vector3<T>::Vector3( Vector2<T> const& copyFrom )
	:x(copyFrom.x), y(copyFrom.y), z(static_cast<T>(0.f))
{

}

template<FloatingPointType T>
Vector3<T>::Vector3( Vector3 const& copyFrom )
	:x( copyFrom.x ), y( copyFrom.y ), z( copyFrom.z )
{

}
