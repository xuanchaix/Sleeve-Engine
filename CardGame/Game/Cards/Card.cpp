#include "Game/Cards/Card.h"
#include "Engine/Graphics/GraphicsFwd.h"
constexpr uint32_t PerFrameTextVertexCount = 36;
constexpr uint32_t PerFrameTextDataSize = 36 * sizeof( VertexPCU3D );

Card::Card( CardDefinition const& def, Vec3 const& position /*= Vec3()*/, Euler const& orientation /*= Euler()*/ )
	:Entity3D(position, orientation, false), m_def(def)
{
}

Card::~Card()
{
	g_theRenderer->ReturnMemoryToSharedBuffer( m_vertexBufferBinding, m_indexBufferBinding, m_uniformBufferBinding );
	g_theRenderer->DeferredDestroyBuffer( m_textVertexBufferBinding.m_vertexBuffer, false );
}

void Card::BeginPlay()
{
	m_vertices = { {Vec3{-33.f, -45.f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 0.0f}},
				{Vec3{33.f, -45.f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 0.0f}},
				{Vec3{33.f, 45.f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 1.0f}},
				{Vec3{-33.f, 45.f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 1.0f}}, };
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

	m_curCoolDown = m_def.m_coolDown;
	m_curHealth = m_def.m_health;
	m_curDamage = m_def.m_damage;
}

void Card::Update( float deltaSeconds )
{
	CalculateModelMatrix( m_modelMatrix );

	m_textVerts.clear();
	if (!m_inBattleLine) {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curCoolDown ), Vec2( -32.f, 31.f ), Rgba8( 0, 0, 0 ), 20.f, 1.f );
	}
	if (m_curDamage < m_def.m_damage) {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curDamage ), Vec2( -32.f, -43.f ), Rgba8( 255, 0, 0 ), 20.f, 1.f );
	}
	else if (m_curDamage > m_def.m_damage) {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curDamage ), Vec2( -32.f, -43.f ), Rgba8( 0, 255, 0 ), 20.f, 1.f );
	}
	else {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curDamage ), Vec2( -32.f, -43.f ), Rgba8( 0, 0, 0 ), 20.f, 1.f );
	}

	if (m_curHealth < m_def.m_health) {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curHealth ), Vec2( 15.f, -43.f ), Rgba8( 255, 0, 0 ), 20.f, 1.f );
	}
	else if (m_curHealth > m_def.m_health) {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curHealth ), Vec2( 15.f, -43.f ), Rgba8( 0, 255, 0 ), 20.f, 1.f );
	}
	else {
		g_defaultFont->AddVertsForText2D( m_textVerts, std::to_string( m_curHealth ), Vec2( 15.f, -43.f ), Rgba8( 0, 0, 0 ), 20.f, 1.f );
	}
	m_textVertexBufferBinding.m_vertexBufferVertexCount = (uint32_t)m_textVerts.size();
	uint32_t frameIndex = g_theRenderer->GetCurFrameNumber();
	uint32_t offset = frameIndex * PerFrameTextDataSize;
	memcpy((char*)m_textVertexBufferBinding.m_vertexBuffer->m_mappedData + offset, m_textVerts.data(), m_textVertexBufferBinding.m_vertexBufferVertexCount * sizeof(VertexPCU3D));
	m_textVertexBufferBinding.m_vertexBufferOffset = offset;
}

void Card::Render() const
{
	// if the current pipeline is not my shader's pipeline, bind my shader's pipeline
	g_theRenderer->BindShader( m_shader );
	// acquire and set the descriptor set for this specific entity
	g_theRenderer->BeginDrawCommands( m_uniformBufferBinding, m_textureBinding );
	// copy the ubo data to the graphics card
	g_theRenderer->UpdateSharedModelUniformBuffer( m_uniformBufferBinding, (void*)&m_modelMatrix, sizeof( m_modelMatrix ) );
	if (m_useIndexBuffer) {
		// draw the entity
		g_theRenderer->DrawIndexed( m_vertexBufferBinding, m_indexBufferBinding );
	}
	else {
		// draw the entity
		g_theRenderer->Draw( m_vertexBufferBinding );
	}

	// ----------------------------------Draw texts--------------------------------
	// if the current pipeline is not my shader's pipeline, bind my shader's pipeline
	g_theRenderer->BindShader( m_shader );
	// acquire and set the descriptor set for this specific entity
	g_theRenderer->BeginDrawCommands( m_uniformBufferBinding, m_fontTextureBinding );
	// copy the ubo data to the graphics card
	g_theRenderer->UpdateSharedModelUniformBuffer( m_uniformBufferBinding, (void*)&m_modelMatrix, sizeof( m_modelMatrix ) );
	g_theRenderer->Draw( m_textVertexBufferBinding );
}

uint32_t Card::GetDamage( int damage )
{
	m_curHealth -= damage;
	if (m_curHealth < 0) {
		m_curHealth = 0;
	}
	return 0;
}

CardDefinition::CardDefinition()
{

}

CardDefinition::CardDefinition( XmlElement* xmlIter )
{
	m_health = ParseXmlAttribute( xmlIter, "health", m_health );
	m_damage = ParseXmlAttribute( xmlIter, "damage", m_damage );
	m_coolDown = ParseXmlAttribute( xmlIter, "coolDown", m_coolDown );
	m_name = ParseXmlAttribute( xmlIter, "name", m_name );
	m_description = ParseXmlAttribute( xmlIter, "description", m_description );
	m_effect = ParseXmlAttribute( xmlIter, "effect", m_effect );
	std::string rarity = ParseXmlAttribute( xmlIter, "rarity", "Beginning" );
	if (rarity == "Beginning") {
		m_rarity = CardRarity::Beginning;
	}
	else if (rarity == "Common") {
		m_rarity = CardRarity::Common;
	}
	else if (rarity == "Rare") {
		m_rarity = CardRarity::Rare;
	}
	else if (rarity == "Legendary") {
		m_rarity = CardRarity::Legendary;
	}
}

void CardDefinition::SetUpCardDefinitions()
{
	CardDefinition::s_definitions.reserve( 64 );
	XmlDocument xmlDocument;
	XmlError errorCode = xmlDocument.LoadFile( "Data/Definitions/CardDefinitions.xml" );
	ASSERT_OR_ERROR( errorCode == tinyxml2::XMLError::XML_SUCCESS, "Error! Load Xml Document CardDefinitions.xml error" );
	XmlElement* root = xmlDocument.FirstChildElement();
	ASSERT_OR_ERROR( !strcmp( root->Name(), "CardDefinitions" ), "Syntax Error! Name of the root of CardDefinitions.xml should be \"CardDefinitions\" " );
	XmlElement* xmlIter = root->FirstChildElement();
	while (xmlIter != nullptr) {
		ASSERT_OR_ERROR( !strcmp( xmlIter->Name(), "CardDefinition" ), "Syntax Error! Names of the elements of CardDefinitions.xml should be \"CardDefinition\" " );
		CardDefinition::s_definitions.emplace_back( xmlIter );
		xmlIter = xmlIter->NextSiblingElement();
	}
}

std::vector<CardDefinition> CardDefinition::s_definitions;

CardDefinition const& CardDefinition::GetDefinition( std::string const& name )
{
	for (auto const& def : s_definitions) {
		if (def.m_name == name) {
			return def;
		}
	}
	THROW_ERROR( std::string( "No Such Entity Definition " ) + name );
}
