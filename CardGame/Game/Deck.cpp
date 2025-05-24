#include "Game/Deck.h"
#include "Game/Battle.h"
#include "Engine/Graphics/GraphicsFwd.h"
#include "Game/Frameworks/GameCommon.h"

Deck::Deck( Vec3 const& position, Euler const& orientation /*= Euler() */ )
	:Entity3D(position, orientation)
{

}

Deck::~Deck()
{
	g_theRenderer->ReturnMemoryToSharedBuffer( m_vertexBufferBinding, m_indexBufferBinding, m_uniformBufferBinding );
	g_theRenderer->DeferredDestroyBuffer( m_textVertexBufferBinding.m_vertexBuffer, false );
}

void Deck::BeginPlay()
{
	m_vertices = { {Vec3{-CardWidth * 0.5f, -CardHeight * 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 0.0f}},
				{Vec3{CardWidth * 0.5f, -CardHeight * 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 0.0f}},
				{Vec3{CardWidth * 0.5f, CardHeight * 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 1.0f}},
				{Vec3{-CardWidth * 0.5f, CardHeight * 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 1.0f}}, };
	m_indices = { 0, 1, 2, 2, 3, 0, };
	m_useIndexBuffer = true;
	// initialize rendering objects
	m_vertexBufferBinding = g_theRenderer->AddVertsDataToSharedVertexBuffer( (void*)m_vertices.data(), sizeof( m_vertices[0] ) * m_vertices.size(), (uint32_t)m_vertices.size() );
	if (m_useIndexBuffer) {
		m_indexBufferBinding = g_theRenderer->AddIndicesDataToSharedIndexBuffer( (void*)m_indices.data(), sizeof( m_indices[0] ) * m_indices.size(), (uint32_t)m_indices.size() );
	}

	m_textVertexBufferBinding.m_vertexBuffer = g_theRenderer->CreateDynamicVertexBuffer( PerFrameTextDataSize * MAX_FRAMES_IN_FLIGHT );
	m_textVertexBufferBinding.m_vertexBufferVertexCount = PerFrameTextVertexCount;

	//m_textureBinding.m_texture = g_theResourceManager->GetOrLoadTexture( "Data/Textures/texture.png" );
	m_textureBinding.m_texture = g_theResourceManager->GetWhiteTexture();
	m_fontTextureBinding.m_texture = g_defaultFont->GetTexture();

	// initialize uniform buffers
	m_uniformBufferBinding = g_theRenderer->AddDataToSharedUniformBuffer( UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2 );

	m_shader = g_theResourceManager->GetOrLoadShader( "shader" );
}

void Deck::Update( float deltaSeconds )
{
	CalculateModelMatrix( m_modelMatrix );
	m_textVerts.clear();
	if (m_isFriendly) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( (int)m_battle->m_myCardsInDeck.size() ), AABB2( Vec2( -33.f, -15.f ), Vec2( 33.f, 15.f ) ), Rgba8( 0, 0, 0 ), 30.f, Vec2( 0.5f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}
	else {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( (int)m_battle->m_enemyCardsInDeck.size() ), AABB2( Vec2( -33.f, -15.f ), Vec2( 33.f, 15.f ) ), Rgba8( 0, 0, 0 ), 30.f, Vec2( 0.5f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}

	m_textVertexBufferBinding.m_vertexBufferVertexCount = (uint32_t)m_textVerts.size();
	uint32_t frameIndex = g_theRenderer->GetCurFrameNumber();
	uint32_t offset = frameIndex * PerFrameTextDataSize;
	memcpy((char*)m_textVertexBufferBinding.m_vertexBuffer->m_mappedData + offset, m_textVerts.data(), m_textVertexBufferBinding.m_vertexBufferVertexCount * sizeof(VertexPCU3D));
	//g_theRenderer->CopyDataToVertexBufferThroughStagingBuffer( m_textVerts.data(), m_textVertexBufferBinding.m_vertexBufferVertexCount * sizeof( VertexPCU3D ), m_textVertexBufferBinding.m_vertexBuffer, offset );
	m_textVertexBufferBinding.m_vertexBufferOffset = offset;
	g_theRenderer->UpdateSharedModelUniformBuffer( m_uniformBufferBinding, (void*)&m_modelMatrix, sizeof( m_modelMatrix ) );
}

void Deck::Render() const
{
	// if the current pipeline is not my shader's pipeline, bind my shader's pipeline
	g_theRenderer->BindShader( m_shader );
	// -------------------------------Draw Card-----------------------------------
	// acquire and set the descriptor set for this specific entity
	g_theRenderer->BeginDrawCommands( m_uniformBufferBinding, m_textureBinding );
	if (m_useIndexBuffer) {
		// draw the entity
		g_theRenderer->DrawIndexed( m_vertexBufferBinding, m_indexBufferBinding );
	}
	else {
		// draw the entity
		g_theRenderer->Draw( m_vertexBufferBinding );
	}

	// ----------------------------------Draw texts--------------------------------
	// acquire and set the descriptor set for this specific entity
	g_theRenderer->BeginDrawCommands( m_uniformBufferBinding, m_fontTextureBinding );
	g_theRenderer->Draw( m_textVertexBufferBinding );
}
