#pragma once
#include <string>
#include <vector>
#include "Graphics/Vertex.h"

class Texture;
enum class TextBoxMode {
	SHRINK_TO_FIT,
	OVERRUN,
	AUTO_NEW_LINE,
};

class Font {
public:
	Font( std::string const& fontPath );
	~Font();

	void AddVertsForTextInBox2D( std::vector<VertexPCU3D>& verts, std::string const& text, AABB2 const& box, Rgba8 const& color, float textSize, Vec2 const& alignment, TextBoxMode mode = TextBoxMode::SHRINK_TO_FIT, float zHeight = 0.f ) const;
	void AddVertsForText2D( std::vector<VertexPCU3D>& verts, std::string const& text, Vec2 const& leftBottomPos, Rgba8 const& color, float textSize, float zHeight = 0.f ) const;
	Texture* GetTexture() const;
protected:
	float GetTextWidth( float textSize, std::string const& string ) const;
protected:
	float m_fontSize = 64.f;
	float m_spaceAdvance = 0.f;
	void* m_fontPackRange = nullptr;
	void* m_fontInfo = nullptr;
	Texture* m_texture;
	std::string m_path;
	unsigned char* m_ttfBuffer = NULL;
};