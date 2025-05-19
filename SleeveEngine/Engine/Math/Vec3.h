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
	Vector3( Vector3 const& copyFrom ) noexcept;
	//Vector2( IntVec2 const& copyFrom );
	Vector3( Vector2<T> const& copyFrom ) noexcept;
	explicit Vector3( T X, T Y, T Z ) noexcept;
	//bool SetFromText( char const* text );

	// Accessors
	T GetLength() const noexcept;

	T GetLengthSquared() const noexcept;

	//Vec2 const GetRotatedNewBasis( Vec2 const& iBasisNormal ) const;

	Vector3 GetClamped( T maxLength ) const noexcept;

	Vector3 GetNormalized() const noexcept;

	void SetLength( T newLength ) noexcept;

	void ClampLength( T maxLength ) noexcept;

	void Normalize() noexcept;

	// Operators (const)
	bool		operator==( Vector3 const& compare ) const noexcept;
	bool		operator!=( Vector3 const& compare ) const noexcept;
	Vector3 operator+( Vector3 const& vecToAdd ) const noexcept;
	Vector3 operator-( Vector3 const& vecToSubtract ) const noexcept;
	Vector3 operator-() const noexcept;
	Vector3 operator*( T uniformScale ) const noexcept;
	Vector3 operator*( Vector3 const& vecToMultiply ) const noexcept;
	Vector3 operator/( T inverseScale ) const noexcept;

	// Operators (self-mutating / non-const)
	void		operator+=( Vector3 const& vecToAdd ) noexcept;
	void		operator-=( Vector3 const& vecToSubtract ) noexcept;
	void		operator*=( T uniformScale ) noexcept;
	void		operator/=( T uniformDivisor ) noexcept;
	void		operator=( Vector2<T> const& copyFrom ) noexcept;
	void		operator=( Vector3 const& copyFrom ) noexcept;


	friend Vector3 operator*( T uniformScale, Vector3 const& vecToScale ) noexcept;
};

template<FloatingPointType T>
void Vector3<T>::operator=( Vector3 const& copyFrom ) noexcept
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

template<FloatingPointType T>
void Vector3<T>::operator=( Vector2<T> const& copyFrom ) noexcept
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = static_cast<T>(0.f);
}

template<FloatingPointType T>
void Vector3<T>::operator/=( T uniformDivisor ) noexcept
{
	T inversedDivisor = static_cast<T>(1.f) / uniformDivisor;
	x *= inversedDivisor;
	y *= inversedDivisor;
	z *= inversedDivisor;
}

template<FloatingPointType T>
void Vector3<T>::operator*=( T uniformScale ) noexcept
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

template<FloatingPointType T>
void Vector3<T>::operator-=( Vector3 const& vecToSubtract ) noexcept
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

template<FloatingPointType T>
void Vector3<T>::operator+=( Vector3 const& vecToAdd ) noexcept
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator/( T inverseScale ) const noexcept
{
	T inversedDivisor = static_cast<T>(1.f) / inverseScale;
	return Vector3( x * inversedDivisor, y * inversedDivisor, z * inversedDivisor );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator*( Vector3 const& vecToMultiply ) const noexcept
{
	return Vector3( x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator*( T uniformScale ) const noexcept
{
	return Vector3( x * uniformScale, y * uniformScale, z * uniformScale );
}

template<FloatingPointType T>
Vector3<T> operator*( T uniformScale, Vector3<T> const& vecToScale ) noexcept
{
	return Vector3( uniformScale * vecToScale.x, uniformScale * vecToScale.y, uniformScale * vecToScale.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator-() const noexcept
{
	return Vector3( -x, -y, -z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator-( Vector3 const& vecToSubtract ) const noexcept
{
	return Vector3( x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::operator+( Vector3 const& vecToAdd ) const noexcept
{
	return Vector3( x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z );
}

template<FloatingPointType T>
bool Vector3<T>::operator!=( Vector3 const& compare ) const noexcept
{
	return x != compare.x || y != compare.y || z != compare.z;
}

template<FloatingPointType T>
bool Vector3<T>::operator==( Vector3 const& compare ) const noexcept
{
	return x == compare.x && y == compare.y && z == compare.z;
}

template<FloatingPointType T>
void Vector3<T>::Normalize() noexcept
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
void Vector3<T>::ClampLength( T maxLength ) noexcept
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
void Vector3<T>::SetLength( T newLength ) noexcept
{
	if (x != static_cast<T>(0.f) && y != static_cast<T>(0.f) && z != static_cast<T>(0.f)) {
		T ratio = newLength * GetLength();
		x = x * ratio;
		y = y * ratio;
		z = z * ratio;
	}
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::GetNormalized() const noexcept
{
	if (x == static_cast<T>(0.f) && y == static_cast<T>(0.f) && z == static_cast<T>(0.f)) {
		return Vector3();
	}
	T inversedLength = static_cast<T>(1.f) / GetLength();
	return Vector3( x * inversedLength, y * inversedLength, z * inversedLength );
}

template<FloatingPointType T>
Vector3<T> Vector3<T>::GetClamped( T maxLength ) const noexcept
{
	T length = GetLength();
	if (length > maxLength && maxLength >= static_cast<T>(0.f) && length != static_cast<T>(0.f)) {
		float ratio = maxLength / length;
		return Vector3( x * ratio, y * ratio, z * ratio );
	}
	return Vector3( x, y, z );
}

template<FloatingPointType T>
T Vector3<T>::GetLengthSquared() const noexcept
{
	return x * x + y * y + z * z;
}

template<FloatingPointType T>
T Vector3<T>::GetLength() const noexcept
{
	return std::sqrt( x * x + y * y + z * z );
}

template<FloatingPointType T>
Vector3<T>::Vector3( T X, T Y, T Z ) noexcept
	:x(X), y(Y), z(Z)
{

}

template<FloatingPointType T>
Vector3<T>::Vector3( Vector2<T> const& copyFrom ) noexcept
	:x(copyFrom.x), y(copyFrom.y), z(static_cast<T>(0.f))
{

}

template<FloatingPointType T>
Vector3<T>::Vector3( Vector3 const& copyFrom ) noexcept
	:x( copyFrom.x ), y( copyFrom.y ), z( copyFrom.z )
{

}
