#pragma once
#include "Math/MathCommon.h"

template<FloatingPointType T>
struct Mat4;

/// Rotate a matrix around z-axis and get the result matrix
// template<FloatingPointType T>
// Mat4<T> GetRotatedMatrixZAxis( Mat4<T> const& mat, T degrees );
/// Add a rotation to the matrix
// template<FloatingPointType T>
// void RotateMatrixZAxis( Mat4<T>& mat, T degrees );

/// Get look at matrix(view matrix) from eye to target center position
template<FloatingPointType T>
Mat4<T> GetLookAtMatrix( Vector3<T> const& eyePosition, Vector3<T> const& targetPosition, Vector3<T> const& upNormal );
/// Get perspective matrix
template<FloatingPointType T>
Mat4<T> GetPerspectiveMatrix( T fov, T aspect, T zNear, T zFar );
/// Get orthographic matrix
//template<FloatingPointType T>
//Mat4<T> GetOrthographicMatrix( T fov, T aspect, T zNear, T zFar );
/// Get render matrix for Vulkan
// template<FloatingPointType T>
// Mat4<T> GetRenderMatrix();



#include "Math/MatrixUtils.inl"