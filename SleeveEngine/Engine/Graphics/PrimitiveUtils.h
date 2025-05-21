#pragma once
#include <vector>
#include "Graphics/Vertex.h"

/// Add AABB2D vertexes to a vector
void AddVertsForAABB2D( std::vector<VertexPCU3D>& verts, AABB2 const& aabbBox, Rgba8 const& color, AABB2 const& uvs = AABB2::Identity );
/// Add AABB2D vertexes to a vector
void AddVertsForAABB2D( std::vector<VertexPCU3D>& verts, AABB2 const& aabbBox, Rgba8 const& color, Vec2 const& uvMins, Vec2 const& uvMaxs );