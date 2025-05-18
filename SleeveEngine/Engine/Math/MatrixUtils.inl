#pragma once
#include "Math/Matrix44.h"
// 
// template<FloatingPointType T>
// Mat4<T> GetRotatedMatrixZAxis( Mat4<T> const& mat, T degrees )
// {
// 	mat.Append( Mat4::MakeZRotationDegrees( degrees ) );
// }
// 
// template<FloatingPointType T>
// void RotateMatrixZAxis( Mat4<T>& mat, T degrees )
// {
// 
// }

template<FloatingPointType T>
Mat4<T> GetLookAtMatrix( Vector3<T> const& eyePosition, Vector3<T> const& targetPosition, Vector3<T> const& upNormal )
{
	Vector3<T> forward = (targetPosition - eyePosition).GetNormalized();
	Vector3<T> side = CrossProduct3D( forward, upNormal ).GetNormalized();
	Vector3<T> up = CrossProduct3D( side, forward );

	Mat4<T> retValue;
	retValue.m_values[Mat4<float>::Ix] = side.x;
	retValue.m_values[Mat4<float>::Jx] = side.y;
	retValue.m_values[Mat4<float>::Kx] = side.z;
	retValue.m_values[Mat4<float>::Iy] = up.x;
	retValue.m_values[Mat4<float>::Jy] = up.y;
	retValue.m_values[Mat4<float>::Ky] = up.z;
	retValue.m_values[Mat4<float>::Iz] = -forward.x;
	retValue.m_values[Mat4<float>::Jz] = -forward.y;
	retValue.m_values[Mat4<float>::Kz] = -forward.z;
	retValue.m_values[Mat4<float>::Tx] = -DotProduct3D( eyePosition, side );
	retValue.m_values[Mat4<float>::Ty] = -DotProduct3D( eyePosition, up );
	retValue.m_values[Mat4<float>::Tz] = DotProduct3D( eyePosition, forward );

	return retValue;
}

template<FloatingPointType T>
Mat4<T> GetPerspectiveMatrix( T fov, T aspect, T zNear, T zFar )
{
	Mat4<T> retMat;
	T s = CosDegrees( fov * static_cast<T>(0.5f) ) / SinDegrees( fov * static_cast<T>(0.5f) );
	retMat.m_values[Mat4<float>::Ix] = s / aspect;
	retMat.m_values[Mat4<float>::Jy] = -s;
	retMat.m_values[Mat4<float>::Kz] = zFar / (zNear - zFar);
	retMat.m_values[Mat4<float>::Kw] = -static_cast<T>(1.f);
	retMat.m_values[Mat4<float>::Tz] = -zFar * zNear / (zFar - zNear);
	retMat.m_values[Mat4<float>::Tw] = static_cast<T>(0.f);
	return retMat;
}

// template<FloatingPointType T>
// Mat4<T> GetRenderMatrix()
// {
// 	return Mat4<T>( Vector4<T>((T)0, (T)0, (T)1, (T)0), Vector4<T>((T)1, (T)0, (T)0, (T)0), Vector4<T>((T)0, (T)1, (T)0, (T)0), Vector4<T>((T)0, (T)0, (T)0, (T)1) );
// }