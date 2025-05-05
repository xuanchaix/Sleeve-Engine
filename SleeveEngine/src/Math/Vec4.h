#pragma once
#include "Math/MathCommon.h"
#include "Math/MathUtils.h"
#include "Math/Vec3.h"

template<FloatingPointType T>
struct Vector4 {
public:
	T x = T{};
	T y = T{};
	T z = T{};
	T w = T{};

public:
	// Construction/Destruction
	~Vector4() {}
	Vector4() = default;
	Vector4( Vector4 const& copyFrom );
	Vector4( Vector3<T> const& copyFrom );
	Vector4( Vector2<T> const& copyFrom );
	explicit Vector4( T X, T Y, T Z, T W );
	//bool SetFromText( char const* text );

	// Operators (const)
	bool	operator==( Vector4<T> const& compare ) const;
	bool	operator!=( Vector4<T> const& compare ) const;
	Vector4<T> operator+( Vector4<T> const& vecToAdd ) const;
	Vector4<T> operator-( Vector4<T> const& vecToSubtract ) const;
	Vector4<T> operator-() const;
	Vector4<T> operator*( T uniformScale ) const;
	Vector4<T> operator*( Vector4<T> const& vecToMultiply ) const;
	Vector4<T> operator/( T inverseScale ) const;

	// Operators (self-mutating / non-const)
	void		operator+=( Vector4 const& vecToAdd );
	void		operator-=( Vector4 const& vecToSubtract );
	void		operator*=( T uniformScale );
	void		operator/=( T uniformDivisor );
	void		operator=( Vector3<T> const& copyFrom );
	void		operator=( Vector4 const& copyFrom );

	friend Vector4 operator*( T uniformScale, Vector4 const& vecToScale );
};

template<FloatingPointType T>
Vector4<T>::Vector4( Vector2<T> const& copyFrom )
	:x( copyFrom.x ), y( copyFrom.y ), z( static_cast<T>(0.f) ), w( static_cast<T>(0.f) )
{}

template<FloatingPointType T>
void Vector4<T>::operator=( Vector4 const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
	w = copyFrom.w;
}

template<FloatingPointType T>
void Vector4<T>::operator=( Vector3<T> const& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

template<FloatingPointType T>
void Vector4<T>::operator/=( T uniformDivisor )
{
	T inversedDivisor = static_cast<T>(1.f) / uniformDivisor;
	x *= inversedDivisor;
	y *= inversedDivisor;
	z *= inversedDivisor;
	w *= inversedDivisor;
}

template<FloatingPointType T>
void Vector4<T>::operator*=( T uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
	w *= uniformScale;
}

template<FloatingPointType T>
void Vector4<T>::operator-=( Vector4 const& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
	w -= vecToSubtract.w;
}

template<FloatingPointType T>
void Vector4<T>::operator+=( Vector4 const& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
	w += vecToAdd.w;
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator/( T inverseScale ) const
{
	T inversedDivisor = static_cast<T>(1.f) / inverseScale;
	return Vector4( x * inverseScale, y * inverseScale, z * inverseScale, w * inverseScale );
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator*( Vector4 const& vecToMultiply ) const
{
	return Vector4( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z, w * vecToMultiply.w );
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator*( T uniformScale ) const
{
	return Vector4( x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale );
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator-() const
{
	return Vector4( -x, -y, -z, -w );
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator-( Vector4 const& vecToSubtract ) const
{
	return Vector4( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w );
}

template<FloatingPointType T>
Vector4<T> Vector4<T>::operator+( Vector4 const& vecToAdd ) const
{
	return Vector4( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w );
}

template<FloatingPointType T>
bool Vector4<T>::operator!=( Vector4 const& compare ) const
{
	return x != compare.x || y != compare.y || z != compare.z || w != compare.w;
}

template<FloatingPointType T>
bool Vector4<T>::operator==( Vector4 const& compare ) const
{
	return x == compare.x && y == compare.y && z == compare.z && w == compare.w;
}

template<FloatingPointType T>
Vector4<T>::Vector4( T X, T Y, T Z, T W )
	:x(X), y(Y), z(Z), w(W)
{}

template<FloatingPointType T>
Vector4<T>::Vector4( Vector3<T> const& copyFrom )
	:x(copyFrom.x), y(copyFrom.y), z(copyFrom.z), w(static_cast<T>(0.f))
{}

template<FloatingPointType T>
Vector4<T>::Vector4( Vector4 const& copyFrom )
	:x(copyFrom.x), y(copyFrom.y), z(copyFrom.z), w(copyFrom.w)
{}
