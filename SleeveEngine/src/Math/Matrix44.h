#pragma once
#include "Math/MathCommon.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"

template<FloatingPointType T>
struct Mat4 {
public:
	enum {Ix, Iy, Iz, Iw, Jx, Jy, Jz, Jw, Kx, Ky, Kz, Kw, Tx, Ty, Tz, Tw};

	union {
		Vector4<T> m_cols[4];
		T m_values[16] = { static_cast<T>(0.f) };
	};

	Mat4();
	~Mat4();
	Mat4( Mat4 const& mat );
	explicit Mat4( T const* values );
	explicit Mat4( Vector2<T> const& iBasis, Vector2<T> const& jBasis, Vector2<T> const& kBasis, Vector2<T> const& translation );
	explicit Mat4( Vector3<T> const& iBasis, Vector3<T> const& jBasis, Vector3<T> const& kBasis, Vector3<T> const& translation );
	explicit Mat4( Vector4<T> const& iBasis, Vector4<T> const& jBasis, Vector4<T> const& kBasis, Vector4<T> const& translation );

	void operator=( Mat4<T> const& copyFrom );

	static Mat4<T> MakeZRotationDegrees( float rotationAboutZ );

	/// transpose the matrix
	void Transpose();

	//-----------------------------------------
	// thisMatrix = thisMatrix * matToAppend;
	// [this] = [this][append]
	void Append( Mat4<T> const& matToAppend );
	//-----------------------------------------
	// returnMatrix = thisMatrix * matToMultiply;
	// [return] = [this][mul]
	Mat4<T> MultiplyRight( Mat4<T> const& matToMultiply ) const;
};

template<FloatingPointType T>
void Mat4<T>::Transpose()
{
	std::swap( m_values[Iy], m_values[Jx] );
	std::swap( m_values[Iz], m_values[Kx] );
	std::swap( m_values[Jz], m_values[Ky] );
}

template<FloatingPointType T>
void Mat4<T>::operator=( Mat4<T> const& copyFrom )
{
	memcpy( m_values, copyFrom.m_values, 16 * sizeof( T ) );
}

template<FloatingPointType T>
Mat4<T>::Mat4( Mat4 const& mat )
{
	memcpy( m_values, mat.m_values, 16 * sizeof( T ) );
}

template<FloatingPointType T>
Mat4<T>::~Mat4()
{

}

template<FloatingPointType T>
Mat4<T>::Mat4( Vector4<T> const& iBasis, Vector4<T> const& jBasis, Vector4<T> const& kBasis, Vector4<T> const& translation )
	: m_cols{ iBasis, jBasis, kBasis, translation }
{}

template<FloatingPointType T>
Mat4<T>::Mat4( Vector3<T> const& iBasis, Vector3<T> const& jBasis, Vector3<T> const& kBasis, Vector3<T> const& translation )
	: m_cols{ Vector4( iBasis ), Vector4( jBasis ), Vector4( kBasis ), Vector4( translation.x, translation.y, translation.z, static_cast<T>(1.f) ) }
{}

template<FloatingPointType T>
Mat4<T>::Mat4( Vector2<T> const& iBasis, Vector2<T> const& jBasis, Vector2<T> const& kBasis, Vector2<T> const& translation )
	: m_cols{ Vector4( iBasis ), Vector4( jBasis ), Vector4( kBasis ), Vector4( translation.x, translation.y, static_cast<T>(0.f), static_cast<T>(1.f) ) }
{}

template<FloatingPointType T>
Mat4<T>::Mat4( T const* values )
{
	memcpy( m_values, values, 16 * sizeof(T) );
}

template<FloatingPointType T>
Mat4<T>::Mat4()
{
	m_values[Ix] = static_cast<T>(1.f);
	m_values[Jy] = static_cast<T>(1.f);
	m_values[Kz] = static_cast<T>(1.f);
	m_values[Tw] = static_cast<T>(1.f);
}

template<FloatingPointType T>
Mat4<T> Mat4<T>::MakeZRotationDegrees( float rotationAboutZ )
{
	Mat4<T> retValue = Mat4<T>();
	T cos = CosDegrees( rotationAboutZ );
	T sin = SinDegrees( rotationAboutZ );
	retValue.m_values[Ix] = cos;
	retValue.m_values[Iy] = sin;
	retValue.m_values[Jx] = -sin;
	retValue.m_values[Jy] = cos;
	return retValue;
}

template<FloatingPointType T>
void Mat4<T>::Append( Mat4<T> const& matToAppend )
{
	float tempValues[16];
	memcpy( tempValues, m_values, 16 * sizeof( float ) );
	m_values[Ix] = tempValues[Ix] * matToAppend.m_values[Ix] + tempValues[Jx] * matToAppend.m_values[Iy] + tempValues[Kx] * matToAppend.m_values[Iz] + tempValues[Tx] * matToAppend.m_values[Iw];
	m_values[Jx] = tempValues[Ix] * matToAppend.m_values[Jx] + tempValues[Jx] * matToAppend.m_values[Jy] + tempValues[Kx] * matToAppend.m_values[Jz] + tempValues[Tx] * matToAppend.m_values[Jw];
	m_values[Kx] = tempValues[Ix] * matToAppend.m_values[Kx] + tempValues[Jx] * matToAppend.m_values[Ky] + tempValues[Kx] * matToAppend.m_values[Kz] + tempValues[Tx] * matToAppend.m_values[Kw];
	m_values[Tx] = tempValues[Ix] * matToAppend.m_values[Tx] + tempValues[Jx] * matToAppend.m_values[Ty] + tempValues[Kx] * matToAppend.m_values[Tz] + tempValues[Tx] * matToAppend.m_values[Tw];
	m_values[Iy] = tempValues[Iy] * matToAppend.m_values[Ix] + tempValues[Jy] * matToAppend.m_values[Iy] + tempValues[Ky] * matToAppend.m_values[Iz] + tempValues[Ty] * matToAppend.m_values[Iw];
	m_values[Jy] = tempValues[Iy] * matToAppend.m_values[Jx] + tempValues[Jy] * matToAppend.m_values[Jy] + tempValues[Ky] * matToAppend.m_values[Jz] + tempValues[Ty] * matToAppend.m_values[Jw];
	m_values[Ky] = tempValues[Iy] * matToAppend.m_values[Kx] + tempValues[Jy] * matToAppend.m_values[Ky] + tempValues[Ky] * matToAppend.m_values[Kz] + tempValues[Ty] * matToAppend.m_values[Kw];
	m_values[Ty] = tempValues[Iy] * matToAppend.m_values[Tx] + tempValues[Jy] * matToAppend.m_values[Ty] + tempValues[Ky] * matToAppend.m_values[Tz] + tempValues[Ty] * matToAppend.m_values[Tw];
	m_values[Iz] = tempValues[Iz] * matToAppend.m_values[Ix] + tempValues[Jz] * matToAppend.m_values[Iy] + tempValues[Kz] * matToAppend.m_values[Iz] + tempValues[Tz] * matToAppend.m_values[Iw];
	m_values[Jz] = tempValues[Iz] * matToAppend.m_values[Jx] + tempValues[Jz] * matToAppend.m_values[Jy] + tempValues[Kz] * matToAppend.m_values[Jz] + tempValues[Tz] * matToAppend.m_values[Jw];
	m_values[Kz] = tempValues[Iz] * matToAppend.m_values[Kx] + tempValues[Jz] * matToAppend.m_values[Ky] + tempValues[Kz] * matToAppend.m_values[Kz] + tempValues[Tz] * matToAppend.m_values[Kw];
	m_values[Tz] = tempValues[Iz] * matToAppend.m_values[Tx] + tempValues[Jz] * matToAppend.m_values[Ty] + tempValues[Kz] * matToAppend.m_values[Tz] + tempValues[Tz] * matToAppend.m_values[Tw];
	m_values[Iw] = tempValues[Iw] * matToAppend.m_values[Ix] + tempValues[Jw] * matToAppend.m_values[Iy] + tempValues[Kw] * matToAppend.m_values[Iz] + tempValues[Tw] * matToAppend.m_values[Iw];
	m_values[Jw] = tempValues[Iw] * matToAppend.m_values[Jx] + tempValues[Jw] * matToAppend.m_values[Jy] + tempValues[Kw] * matToAppend.m_values[Jz] + tempValues[Tw] * matToAppend.m_values[Jw];
	m_values[Kw] = tempValues[Iw] * matToAppend.m_values[Kx] + tempValues[Jw] * matToAppend.m_values[Ky] + tempValues[Kw] * matToAppend.m_values[Kz] + tempValues[Tw] * matToAppend.m_values[Kw];
	m_values[Tw] = tempValues[Iw] * matToAppend.m_values[Tx] + tempValues[Jw] * matToAppend.m_values[Ty] + tempValues[Kw] * matToAppend.m_values[Tz] + tempValues[Tw] * matToAppend.m_values[Tw];
}

template<FloatingPointType T>
Mat4<T> Mat4<T>::MultiplyRight( Mat4<T> const& matToMultiply ) const
{
	Mat4<T> retMatrix;
	retMatrix.m_values[Ix] = m_values[Ix] * matToMultiply.m_values[Ix] + m_values[Jx] * matToMultiply.m_values[Iy] + m_values[Kx] * matToMultiply.m_values[Iz] + m_values[Tx] * matToMultiply.m_values[Iw];
	retMatrix.m_values[Jx] = m_values[Ix] * matToMultiply.m_values[Jx] + m_values[Jx] * matToMultiply.m_values[Jy] + m_values[Kx] * matToMultiply.m_values[Jz] + m_values[Tx] * matToMultiply.m_values[Jw];
	retMatrix.m_values[Kx] = m_values[Ix] * matToMultiply.m_values[Kx] + m_values[Jx] * matToMultiply.m_values[Ky] + m_values[Kx] * matToMultiply.m_values[Kz] + m_values[Tx] * matToMultiply.m_values[Kw];
	retMatrix.m_values[Tx] = m_values[Ix] * matToMultiply.m_values[Tx] + m_values[Jx] * matToMultiply.m_values[Ty] + m_values[Kx] * matToMultiply.m_values[Tz] + m_values[Tx] * matToMultiply.m_values[Tw];
	retMatrix.m_values[Iy] = m_values[Iy] * matToMultiply.m_values[Ix] + m_values[Jy] * matToMultiply.m_values[Iy] + m_values[Ky] * matToMultiply.m_values[Iz] + m_values[Ty] * matToMultiply.m_values[Iw];
	retMatrix.m_values[Jy] = m_values[Iy] * matToMultiply.m_values[Jx] + m_values[Jy] * matToMultiply.m_values[Jy] + m_values[Ky] * matToMultiply.m_values[Jz] + m_values[Ty] * matToMultiply.m_values[Jw];
	retMatrix.m_values[Ky] = m_values[Iy] * matToMultiply.m_values[Kx] + m_values[Jy] * matToMultiply.m_values[Ky] + m_values[Ky] * matToMultiply.m_values[Kz] + m_values[Ty] * matToMultiply.m_values[Kw];
	retMatrix.m_values[Ty] = m_values[Iy] * matToMultiply.m_values[Tx] + m_values[Jy] * matToMultiply.m_values[Ty] + m_values[Ky] * matToMultiply.m_values[Tz] + m_values[Ty] * matToMultiply.m_values[Tw];
	retMatrix.m_values[Iz] = m_values[Iz] * matToMultiply.m_values[Ix] + m_values[Jz] * matToMultiply.m_values[Iy] + m_values[Kz] * matToMultiply.m_values[Iz] + m_values[Tz] * matToMultiply.m_values[Iw];
	retMatrix.m_values[Jz] = m_values[Iz] * matToMultiply.m_values[Jx] + m_values[Jz] * matToMultiply.m_values[Jy] + m_values[Kz] * matToMultiply.m_values[Jz] + m_values[Tz] * matToMultiply.m_values[Jw];
	retMatrix.m_values[Kz] = m_values[Iz] * matToMultiply.m_values[Kx] + m_values[Jz] * matToMultiply.m_values[Ky] + m_values[Kz] * matToMultiply.m_values[Kz] + m_values[Tz] * matToMultiply.m_values[Kw];
	retMatrix.m_values[Tz] = m_values[Iz] * matToMultiply.m_values[Tx] + m_values[Jz] * matToMultiply.m_values[Ty] + m_values[Kz] * matToMultiply.m_values[Tz] + m_values[Tz] * matToMultiply.m_values[Tw];
	retMatrix.m_values[Iw] = m_values[Iw] * matToMultiply.m_values[Ix] + m_values[Jw] * matToMultiply.m_values[Iy] + m_values[Kw] * matToMultiply.m_values[Iz] + m_values[Tw] * matToMultiply.m_values[Iw];
	retMatrix.m_values[Jw] = m_values[Iw] * matToMultiply.m_values[Jx] + m_values[Jw] * matToMultiply.m_values[Jy] + m_values[Kw] * matToMultiply.m_values[Jz] + m_values[Tw] * matToMultiply.m_values[Jw];
	retMatrix.m_values[Kw] = m_values[Iw] * matToMultiply.m_values[Kx] + m_values[Jw] * matToMultiply.m_values[Ky] + m_values[Kw] * matToMultiply.m_values[Kz] + m_values[Tw] * matToMultiply.m_values[Kw];
	retMatrix.m_values[Tw] = m_values[Iw] * matToMultiply.m_values[Tx] + m_values[Jw] * matToMultiply.m_values[Ty] + m_values[Kw] * matToMultiply.m_values[Tz] + m_values[Tw] * matToMultiply.m_values[Tw];
	return retMatrix;
}