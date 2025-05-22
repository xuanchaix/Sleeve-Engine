#include "Graphics/Font.h"
#include "Core/Error.h"
#include "Graphics/PrimitiveUtils.h"
#include "Graphics/Renderer.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "Thirdparty/stb/stb_truetype.h"

Font::Font( std::string const& fontPath )
{
	unsigned char* ttfBuffer = NULL;
	FILE* fontFile;
	fopen_s( &fontFile, fontPath.c_str(), "rb" );
	if (fontFile) {
		fseek( fontFile, 0, SEEK_END );
		long size = ftell( fontFile );
		fseek( fontFile, 0, SEEK_SET );
		ttfBuffer = (unsigned char*)malloc( size );
		if (!ttfBuffer) {
			THROW_ERROR( "Cannot create ttf buffer" );
		}
		fread( ttfBuffer, size, 1, fontFile );
		fclose( fontFile );
	}
	else {
		THROW_ERROR( "Cannot find ttf file" );
	}

	stbtt_fontinfo font;
	if (!stbtt_InitFont( &font, ttfBuffer, stbtt_GetFontOffsetForIndex( ttfBuffer, 0 ) )) {
		THROW_ERROR( "Cannot initialize ttf font" );
	}

	//int ascent, descent, line_gap;
	//stbtt_GetFontVMetrics( &font, &ascent, &descent, &line_gap );

	float scale = stbtt_ScaleForPixelHeight( &font, m_fontSize ); // Scale for 24px tall font

	// Allocate a large texture (e.g., 512x512)
	constexpr int atlasWidth = 512, atlasHeight = 512;
	unsigned char atlas[atlasWidth * atlasHeight];
	stbtt_pack_context pc;
	stbtt_PackBegin( &pc, atlas, 512, 512, 0, 3, NULL );

	// Pack glyphs into the atlas
	m_fontPackRange = new stbtt_pack_range();
	((stbtt_pack_range*)m_fontPackRange)->font_size = m_fontSize;
	((stbtt_pack_range*)m_fontPackRange)->first_unicode_codepoint_in_range = 32; // ASCII starts at 32
	((stbtt_pack_range*)m_fontPackRange)->num_chars = 95; // ASCII 32-126
	((stbtt_pack_range*)m_fontPackRange)->chardata_for_range = (stbtt_packedchar*)malloc( sizeof( stbtt_packedchar ) * 95 );

	stbtt_PackFontRanges( &pc, ttfBuffer, 0, &*(stbtt_pack_range*)m_fontPackRange, 1 );

	stbtt_PackEnd( &pc );

	// Expand 1-channel atlas_data to RGBA (4 bytes per pixel)
	unsigned char* rgbaAtlas = (unsigned char*)malloc( atlasWidth * atlasHeight * 4 );
	if (!rgbaAtlas) {
		THROW_ERROR( "Cannot initialize RGBA atlas" );
	}
	for (int i = 0; i < atlasWidth * atlasHeight; i++) {
		rgbaAtlas[i * 4 + 0] = 255; // R
		rgbaAtlas[i * 4 + 1] = 255; // G
		rgbaAtlas[i * 4 + 2] = 255; // B
		rgbaAtlas[i * 4 + 3] = atlas[i]; // A
	}

	m_texture = g_theRenderer->CreateTextureFromBuffer( rgbaAtlas, atlasWidth * atlasHeight * 4, atlasWidth, atlasHeight );

	free( ttfBuffer );
	free( rgbaAtlas );
}

Font::~Font()
{
	delete m_texture;
	free( ((stbtt_pack_range*)m_fontPackRange)->chardata_for_range );
	delete m_fontPackRange;
}

void Font::AddVertsForText2D( std::vector<VertexPCU3D>& verts, std::string const& text, Vec2 const& leftBottomPos, Rgba8 const& color, float textSize, float height ) const
{
	size_t startPos = verts.size();
	float xPos = leftBottomPos.x, yPos = leftBottomPos.y;
	float ratio = textSize / m_fontSize;
	float prevX1 = xPos;
	float prevY0 = yPos;
	float prevQY0 = yPos;
	float startX0 = 0.f;
	float startY0 = 0.f;
	// q contains (s0,t0) to (s1,t1) texture coordinates and (x0,y0) to (x1,y1) screen coordinates
	for (int i = 0; i < text.size(); ++i) {
		char c = text[i] - 32;
		stbtt_aligned_quad q;
		float prevXPos = xPos;
		stbtt_GetPackedQuad( ((stbtt_pack_range*)m_fontPackRange)->chardata_for_range, 512, 512, c, &xPos, &yPos, &q, 0 );

		// Draw quad with texture coordinates (q.s0, q.t0) to (q.s1, q.t1)
		// and screen coordinates (q.x0, q.y0) to (q.x1, q.y1)
		float newX0, newY0;
		if (i == 0) {
			newX0 = q.x0;
			startX0 = q.x0;
			newY0 = q.y0;
			startY0 = q.y0;
		}
		else {
			newX0 = (q.x0 - prevX1) * ratio + prevX1;
			newY0 = (q.y0 - prevQY0) * ratio + prevY0;
		}
		float newX1 = newX0 + (q.x1 - q.x0) * ratio;
		float newY1 = newY0 + (q.y1 - q.y0) * ratio;
		AddVertsForAABB2D( verts, AABB2( Vec2( newX0, newY0 ), Vec2( newX1, newY1 ) ), color, Vec2( q.s0, q.t1 ), Vec2( q.s1, q.t0 ) );

		xPos = (xPos - prevXPos) * ratio + prevXPos;
		prevX1 = newX1;
		prevY0 = newY0;
		prevQY0 = q.y0;
	}
	size_t endPos = verts.size();
	for (size_t i = startPos; i < endPos; ++i) {
		verts[i].m_position.x += (leftBottomPos.x - startX0);
		verts[i].m_position.y += (leftBottomPos.y - startY0);
		verts[i].m_position.z += height;
	}
}

Texture* Font::GetTexture() const
{
	return m_texture;
}

