#include "Graphics/PrimitiveUtils.h"

void AddVertsForAABB2D( std::vector<VertexPCU3D>& verts, AABB2 const& aabbBox, Rgba8 const& color, AABB2 const& uvs /*= AABB2::Identity */ )
{
	AddVertsForAABB2D( verts, aabbBox, color, uvs.m_mins, uvs.m_maxs );
}

void AddVertsForAABB2D( std::vector<VertexPCU3D>& verts, AABB2 const& aabbBox, Rgba8 const& color, Vec2 const& uvMins, Vec2 const& uvMaxs )
{
	verts.emplace_back( aabbBox.m_mins, color, uvMins );
	verts.emplace_back( Vec2( aabbBox.m_maxs.x, aabbBox.m_mins.y ), color, Vec2( uvMaxs.x, uvMins.y ) );
	verts.emplace_back( Vec2( aabbBox.m_mins.x, aabbBox.m_maxs.y ), color, Vec2( uvMins.x, uvMaxs.y ) );
	verts.emplace_back( aabbBox.m_maxs, color, uvMaxs );
	verts.emplace_back( Vec2( aabbBox.m_mins.x, aabbBox.m_maxs.y ), color, Vec2( uvMins.x, uvMaxs.y ) );
	verts.emplace_back( Vec2( aabbBox.m_maxs.x, aabbBox.m_mins.y ), color, Vec2( uvMaxs.x, uvMins.y ) );
}

