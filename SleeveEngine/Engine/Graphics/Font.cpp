#include "Graphics/Font.h"
#include "Core/Error.h"
#include "Graphics/PrimitiveUtils.h"
#include "Graphics/Renderer.h"
#include "Core/StringUtils.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include "Thirdparty/stb/stb_truetype.h"

Font::Font( std::string const& fontPath )
{
	FILE* fontFile;
	fopen_s( &fontFile, fontPath.c_str(), "rb" );
	if (fontFile) {
		fseek( fontFile, 0, SEEK_END );
		long size = ftell( fontFile );
		fseek( fontFile, 0, SEEK_SET );
		m_ttfBuffer = (unsigned char*)malloc( size );
		if (!m_ttfBuffer) {
			THROW_ERROR( "Cannot create ttf buffer" );
		}
		fread( m_ttfBuffer, size, 1, fontFile );
		fclose( fontFile );
	}
	else {
		THROW_ERROR( "Cannot find ttf file" );
	}

	m_fontInfo = new stbtt_fontinfo();
	if (!stbtt_InitFont( (stbtt_fontinfo*)m_fontInfo, m_ttfBuffer, stbtt_GetFontOffsetForIndex( m_ttfBuffer, 0 ) )) {
		THROW_ERROR( "Cannot initialize ttf font" );
	}

	//int ascent, descent, line_gap;
	//stbtt_GetFontVMetrics( &font, &ascent, &descent, &line_gap );

	stbtt_ScaleForPixelHeight( (stbtt_fontinfo*)m_fontInfo, m_fontSize );

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

	stbtt_PackFontRanges( &pc, m_ttfBuffer, 0, &*(stbtt_pack_range*)m_fontPackRange, 1 );

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

	int advance, lsb;
	float scale = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*)m_fontInfo, m_fontSize );
	stbtt_GetCodepointHMetrics( (stbtt_fontinfo*)m_fontInfo, ' ', &advance, &lsb );
	m_spaceAdvance = (float)advance * scale;

	free( rgbaAtlas );
}

Font::~Font()
{
	delete m_texture;
	free( ((stbtt_pack_range*)m_fontPackRange)->chardata_for_range );
	delete m_fontPackRange;
	delete m_fontInfo;
	free( m_ttfBuffer );
}

void Font::AddVertsForTextInBox2D( std::vector<VertexPCU3D>& verts, std::string const& text, AABB2 const& box, Rgba8 const& color, float textSize, Vec2 const& alignment, TextBoxMode mode /*= TextBoxMode::SHRINK_TO_FIT*/, float zHeight /*= 0.f */ ) const
{
	// split string by \n
	Strings strings;
	int numOfLines;
	float boxWidth = box.m_maxs.x - box.m_mins.x;
	// #Todo: Auto new line
// 	if (mode == TextBoxMode::AUTO_NEW_LINE) {
// 		std::string newStr = text;
// 		float widthSum = 0.f;
// 		for (int i = 0; i < (int)text.size(); i++) {
// 			if (text[i] == '\n') {
// 				widthSum = 0.f;
// 				continue;
// 			}
// 			widthSum += GetGlyphAspectWithSpacing( text[i] ) * cellHeight * cellAspect;
// 			if (widthSum > boxWidth) {
// 				newStr = newStr.substr( 0, i ) + "\n" + newStr.substr( i );
// 				widthSum = GetGlyphAspectWithSpacing( text[i] ) * cellHeight * cellAspect;
// 			}
// 		}
// 		numOfLines = SplitStringOnDelimiter( strings, newStr, '\n' );
// 	}
// 	else {
	numOfLines = SplitStringOnDelimiter( strings, text, '\n' );
/*	}*/
	// calculate text box height and width
	float boxHeight = box.m_maxs.y - box.m_mins.y;
	float allTextHeight = textSize * numOfLines;
	float scalingFactor = 1.f;
	if (mode == TextBoxMode::SHRINK_TO_FIT) {
		float allTextMaxWidth = -1;
		// find max width of all lines of string
		for (std::string const& str : strings) {
			float textWidth = GetTextWidth( textSize, str );
			if (textWidth > allTextMaxWidth) {
				allTextMaxWidth = textWidth;
			}
		}
		// find the smallest scaling factor
		float heightScalingFactor = 1.f;
		if (allTextHeight > boxHeight) {
			heightScalingFactor = boxHeight / allTextHeight;
		}
		float widthScalingFactor = 1.f;
		if (allTextMaxWidth > boxWidth) {
			widthScalingFactor = boxWidth / allTextMaxWidth;
		}
		scalingFactor = std::min( heightScalingFactor, widthScalingFactor );
	}

	int drawGlyphsCount = 0;
	for (int i = 0; i < (int)strings.size(); i++) {
		std::string const& str = strings[i];
		float stringWidth = GetTextWidth( textSize, str ) * scalingFactor;
		// xOffset: offset from left
		float remainSpaceX = boxWidth - stringWidth;
		float xOffset = remainSpaceX * alignment.x;
		// yOffset: offset from top because text is start from top (but AABB starts from bottom)
		float remainSpaceY = boxHeight - allTextHeight * scalingFactor;
		float yOffset = remainSpaceY * (1 - alignment.y);
		// check if reach max glyph to draw
/*		if (drawGlyphsCount + (int)str.size() <= maxGlyphsToDraw) {*/
		AddVertsForText2D( verts, str, Vec2( box.m_mins.x + xOffset, box.m_maxs.y - yOffset - textSize * scalingFactor * (i + 1) ), color, textSize * scalingFactor, zHeight );
		drawGlyphsCount += (int)str.size();
/*		}*/
// 		else {
// 			AddVertsForText2D( verts, Vec2( box.m_mins.x + xOffset, box.m_maxs.y - yOffset - cellHeight * scalingFactor * (i + 1) ), cellHeight * scalingFactor, str.substr( 0, (size_t)maxGlyphsToDraw - drawGlyphsCount ), tint, cellAspect );
// 			return;
// 		}
	}
}

void Font::AddVertsForText2D( std::vector<VertexPCU3D>& verts, std::string const& text, Vec2 const& leftBottomPos, Rgba8 const& color, float textSize, float zHeight ) const
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
		if (c == 0) {
			q.x1 += m_spaceAdvance;
			xPos += m_spaceAdvance;
		}

		float qy1 = q.y1;
		q.y1 = m_fontSize - q.y0;
		q.y0 = m_fontSize - qy1;
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
		verts[i].m_position.z += zHeight;
	}
}

Texture* Font::GetTexture() const
{
	return m_texture;
}

static int utf8Decode( const char* s, int* codepoint ) {
	unsigned char* c = (unsigned char*)s;
	if (c[0] < 0x80) {
		*codepoint = c[0];
		return 1;
	}
	else if ((c[0] & 0xE0) == 0xC0) {
		*codepoint = ((c[0] & 0x1F) << 6) | (c[1] & 0x3F);
		return 2;
	}
	else if ((c[0] & 0xF0) == 0xE0) {
		*codepoint = ((c[0] & 0x0F) << 12) | ((c[1] & 0x3F) << 6) | (c[2] & 0x3F);
		return 3;
	}
	else if ((c[0] & 0xF8) == 0xF0) {
		*codepoint = ((c[0] & 0x07) << 18) | ((c[1] & 0x3F) << 12) | ((c[2] & 0x3F) << 6) | (c[3] & 0x3F);
		return 4;
	}
	*codepoint = 0;
	return 1;
}

float Font::GetTextWidth( float textSize, std::string const& string ) const
{
	//int ascent, descent, line_gap;
	//stbtt_GetFontVMetrics( (stbtt_fontinfo*)m_fontInfo, &ascent, &descent, &line_gap );

	float scale = stbtt_ScaleForPixelHeight( (stbtt_fontinfo*)m_fontInfo, textSize );// */ m_fontSize;

	//scale = stbtt_ScaleForMappingEmToPixels( (stbtt_fontinfo*)m_fontInfo, m_fontSize ) * textSize / m_fontSize;
	float x = 0.f;
	int prevCodepoint = 0;

	for (const char* c = string.c_str(); *c; ) {
		int codepoint;
		c += utf8Decode( c, &codepoint );

		int advance, lsb;
		stbtt_GetCodepointHMetrics( (stbtt_fontinfo*)m_fontInfo, codepoint, &advance, &lsb );

		if (prevCodepoint) {
			x += stbtt_GetCodepointKernAdvance( (stbtt_fontinfo*)m_fontInfo, prevCodepoint, codepoint ) * scale;
		}

		x += advance * scale;
		prevCodepoint = codepoint;
	}

	return x;
}

