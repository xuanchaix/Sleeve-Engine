#pragma once
#include "Math/MathCommon.h"
#include "Math/Vec3.h"
#include "Math/Matrix44.h"

template<FloatingPointType T>
struct EulerAngles {
	T yaw = T{};
	T pitch = T{};
	T roll = T{};

	EulerAngles() = default;
	explicit EulerAngles( T in_yaw, T in_pitch, T in_roll ) noexcept;

	/// Get the rotation matrix that this Euler angle is pointing to
	Mat4<T> GetMatrix() const noexcept;
	/// Get the inversed rotation matrix that this Euler angle is pointing to
	Mat4<T> GetMatrixInversed() const noexcept;
	/// Get the forward vector
	Vector3<T> GetForwardVector() const noexcept;
	/// Get the forward and left vector
	void GetForwardAndLeftVector( Vector3<T>& forward, Vector3<T>& left ) const noexcept;
	/// Get the forward, left and up vector
	void GetForwardAndLeftAndUpVector( Vector3<T>& forward, Vector3<T>& left, Vector3<T>& up ) const noexcept;

};

template<FloatingPointType T>
EulerAngles<T>::EulerAngles( T in_yaw, T in_pitch, T in_roll ) noexcept
	:yaw(in_yaw), pitch(in_pitch), roll(in_roll)
{}

template<FloatingPointType T>
Vector3<T> EulerAngles<T>::GetForwardVector() const noexcept
{
	Vector3<T> retVec;
	float sy = SinDegrees( yaw );
	float cy = CosDegrees( yaw );
	float sp = SinDegrees( pitch );
	float cp = CosDegrees( pitch );

	retVec.x = cy * cp;
	retVec.y = sy * cp;
	retVec.z = -sp;
	return retVec;
}

template<FloatingPointType T>
void EulerAngles<T>::GetForwardAndLeftVector( Vector3<T>& forward, Vector3<T>& left ) const noexcept
{
	float sy = SinDegrees( yaw );
	float cy = CosDegrees( yaw );
	float sp = SinDegrees( pitch );
	float cp = CosDegrees( pitch );
	float sr = SinDegrees( roll );
	float cr = CosDegrees( roll );

	forward.x = cy * cp;
	forward.y = sy * cp;
	forward.z = -sp;

	left.x = -sy * cr + cy * sp * sr;
	left.y = cy * cr + sr * sy * sp;
	left.z = cp * sr;
}

template<FloatingPointType T>
void EulerAngles<T>::GetForwardAndLeftAndUpVector( Vector3<T>& forward, Vector3<T>& left, Vector3<T>& up ) const noexcept
{
	float sy = SinDegrees( yaw );
	float cy = CosDegrees( yaw );
	float sp = SinDegrees( pitch );
	float cp = CosDegrees( pitch );
	float sr = SinDegrees( roll );
	float cr = CosDegrees( roll );

	forward.x = cy * cp;
	forward.y = sy * cp;
	forward.z = -sp;

	left.x = -sy * cr + cy * sp * sr;
	left.y = cy * cr + sr * sy * sp;
	left.z = cp * sr;

	up.x = sr * sy + cy * sp * cr;
	up.y = -cy * sr + cr * sy * sp;
	up.z = cp * cr;
}

template<FloatingPointType T>
Mat4<T> EulerAngles<T>::GetMatrix() const noexcept
{
	Mat4<T> retMat = Mat4<T>();
	float sy = SinDegrees( yaw );
	float cy = CosDegrees( yaw );
	float sp = SinDegrees( pitch );
	float cp = CosDegrees( pitch );
	float sr = SinDegrees( roll );
	float cr = CosDegrees( roll );

	retMat.m_values[Mat4<float>::Ix] = cy * cp;
	retMat.m_values[Mat4<float>::Iy] = sy * cp;
	retMat.m_values[Mat4<float>::Iz] = -sp;
						
	retMat.m_values[Mat4<float>::Jx] = -sy * cr + cy * sp * sr;
	retMat.m_values[Mat4<float>::Jy] = cy * cr + sr * sy * sp;
	retMat.m_values[Mat4<float>::Jz] = cp * sr;
						
	retMat.m_values[Mat4<float>::Kx] = sr * sy + cy * sp * cr;
	retMat.m_values[Mat4<float>::Ky] = -cy * sr + cr * sy * sp;
	retMat.m_values[Mat4<float>::Kz] = cp * cr;
	return retMat;
}

template<FloatingPointType T>
Mat4<T> EulerAngles<T>::GetMatrixInversed() const noexcept
{
	Mat4<T> retMat = Mat4<T>();
	float sy = SinDegrees( yaw );
	float cy = CosDegrees( yaw );
	float sp = SinDegrees( pitch );
	float cp = CosDegrees( pitch );
	float sr = SinDegrees( roll );
	float cr = CosDegrees( roll );

	retMat.m_values[Mat4<float>::Ix] = cy * cp;
	retMat.m_values[Mat4<float>::Jx] = sy * cp;
	retMat.m_values[Mat4<float>::Kx] = -sp;

	retMat.m_values[Mat4<float>::Iy] = -sy * cr + cy * sp * sr;
	retMat.m_values[Mat4<float>::Jy] = cy * cr + sr * sy * sp;
	retMat.m_values[Mat4<float>::Ky] = cp * sr;

	retMat.m_values[Mat4<float>::Iz] = sr * sy + cy * sp * cr;
	retMat.m_values[Mat4<float>::Jz] = -cy * sr + cr * sy * sp;
	retMat.m_values[Mat4<float>::Kz] = cp * cr;
	return retMat;
}