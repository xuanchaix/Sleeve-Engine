#pragma once
#include <vulkan/vulkan.h>
#include "Math/MathFwd.h"
#include "Core/Color.h"
#include <array>

// ToDo: 2D version of vertex
struct VertexPCU2D {
	Vec2 m_position;
	Rgba8 m_color;
	Vec2 m_uvCoord;
};

// 3D version of vertex without normal
struct VertexPCU3D {
	Vec3 m_position; // position of the vertex
	Rgba8 m_color; // color of the vertex, uses 8 bit RGBA from 0 - 255
	Vec2 m_uvCoord; // uv in texture
};