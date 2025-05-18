#pragma once
#include "Math/MathCommon.h"
#include "Math/Vec3.h"
#include "Math/Matrix44.h"

template<FloatingPointType T>
struct EulerAngles {
	T yaw = T{};
	T pitch = T{};
	T roll = T{};

	/// Get the rotation matrix that this Euler angle is pointing to
	Mat4<T> GetMatrix() const;
	/// Get the inversed rotation matrix that this Euler angle is pointing to
	Mat4<T> GetMatrixInversed() const;
	/// Get the forward vector
	Vector3<T> GetForwardVector() const;
};


template<FloatingPointType T>
Vector3<T> EulerAngles<T>::GetForwardVector() const
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
Mat4<T> EulerAngles<T>::GetMatrix() const
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
Mat4<T> EulerAngles<T>::GetMatrixInversed() const
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