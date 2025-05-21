#pragma once

#include "Math/MathCommon.h"
#include "Math/Vec2.h"
#include "Math/Vec3.h"
#include "Math/Vec4.h"
#include "Math/MatrixUtils.h"
#include "Math/Matrix44.h"
#include "Math/EulerAngles.h"
#include "Math/AABB2D.h"

using Vector2f = Vector2<float>;
using Vector2d = Vector2<double>;
using Vec2 = Vector2f;

using Vector3f = Vector3<float>;
using Vector3d = Vector3<double>;
using Vec3 = Vector3f;

using Vector4f = Vector4<float>;
using Vector4d = Vector4<double>;
using Vec4 = Vector4f;

using Mat44f = Mat4<float>;
using Mat44d = Mat4<double>;
using Mat44 = Mat44f;

using Eulerf = EulerAngles<float>;
using Eulerd = EulerAngles<double>;
using Euler = Eulerf;

using AABB2d = AABB2D<double>;
using AABB2f = AABB2D<float>;
using AABB2 = AABB2f;
