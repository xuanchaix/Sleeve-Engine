#include "Game/Cards/Card.h"
#include "Engine/Graphics/GraphicsFwd.h"
constexpr uint32_t PerFrameTextVertexCount = 600;
constexpr uint32_t PerFrameTextDataSize = PerFrameTextVertexCount * sizeof( VertexPCU3D );



Card::Card( CardDefinition const& def, Vec3 const& position /*= Vec3()*/, Euler const& orientation /*= Euler()*/ )
	:Entity3D(position, orientation, false), m_def(def)
{
}

Card::~Card()
{
	g_theRenderer->ReturnMemoryToSharedBuffer( m_UIBinding );
	g_theRenderer->ReturnMemoryToSharedBuffer( m_vertexBufferBinding, m_indexBufferBinding, m_uniformBufferBinding );
	g_theRenderer->DeferredDestroyBuffer( m_textVertexBufferBinding.m_vertexBuffer, false );
}

void Card::BeginPlay()
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
	m_UIBinding = g_theRenderer->AddDataToSharedUniformBuffer( UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2 );

	m_shader = g_theResourceManager->GetOrLoadShader( "shader" );

	m_curCoolDown = m_def.m_coolDown;
	m_curHealth = m_def.m_health;
	m_curDamage = m_def.m_damage;
}

void Card::Update( float deltaSeconds )
{
	if (m_isHovering) {
		m_hoveringTimer += deltaSeconds;
	}
	else {
		m_hoveringTimer = 0.f;
	}

	CalculateModelMatrix( m_modelMatrix );

	// update the text(health, damage, cool down)
	m_textVerts.clear();
	if (!m_inBattleLine) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curCoolDown ), AABB2( Vec2( -32.f, 30.f ), Vec2( -15.f, 50.f ) ), Rgba8( 0, 0, 0 ), 20.f, Vec2( 0.f, 1.f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}
	if (m_curDamage < m_def.m_damage) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curDamage ), AABB2( Vec2( -32.f, -43.f ), Vec2(-15.f, -23.f) ), Rgba8( 255, 0, 0 ), 20.f, Vec2( 0.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}
	else if (m_curDamage > m_def.m_damage) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curDamage ), AABB2( Vec2( -32.f, -43.f ), Vec2(-15.f, -23.f)), Rgba8( 0, 255, 0 ), 20.f, Vec2( 0.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}
	else {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curDamage ), AABB2( Vec2( -32.f, -43.f ), Vec2( -15.f, -23.f ) ), Rgba8( 0, 0, 0 ), 20.f, Vec2( 0.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}

	if (m_curHealth < m_def.m_health) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curHealth ), AABB2(Vec2( 15.f, -43.f ), Vec2(32.f, -23.f)), Rgba8(255, 0, 0), 20.f, Vec2( 1.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f);
	}
	else if (m_curHealth > m_def.m_health) {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curHealth ), AABB2(Vec2( 15.f, -43.f ), Vec2(32.f, -23.f)), Rgba8( 0, 255, 0 ), 20.f, Vec2( 1.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}
	else {
		g_defaultFont->AddVertsForTextInBox2D( m_textVerts, std::to_string( m_curHealth ), AABB2(Vec2( 15.f, -43.f ), Vec2(32.f, -23.f)), Rgba8( 0, 0, 0 ), 20.f, Vec2( 1.f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );
	}

	g_defaultFont->AddVertsForTextInBox2D( m_textVerts, m_def.m_name, AABB2( Vec2( -20.f, 31.f ), Vec2( 20.f, 45.f ) ), Rgba8( 0, 0, 0 ), 12.f, Vec2( 0.5f, 0.5f ), TextBoxMode::SHRINK_TO_FIT, 1.f );

	m_textVertexBufferBinding.m_vertexBufferVertexCount = (uint32_t)m_textVerts.size();
	uint32_t frameIndex = g_theRenderer->GetCurFrameNumber();
	uint32_t offset = frameIndex * PerFrameTextDataSize;
	g_theRenderer->CopyDataToVertexBufferThroughStagingBuffer( m_textVerts.data(), m_textVertexBufferBinding.m_vertexBufferVertexCount * sizeof( VertexPCU3D ), m_textVertexBufferBinding.m_vertexBuffer, offset );
	//memcpy((char*)m_textVertexBufferBinding.m_vertexBuffer->m_mappedData + offset, m_textVerts.data(), m_textVertexBufferBinding.m_vertexBufferVertexCount * sizeof(VertexPCU3D));
	m_textVertexBufferBinding.m_vertexBufferOffset = offset;
}

void Card::Render() const
{
	// if the current pipeline is not my shader's pipeline, bind my shader's pipeline
	g_theRenderer->BindShader( m_shader );
	if (!m_notShowCard) {
		// -------------------------------Draw Card-----------------------------------
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
		// acquire and set the descriptor set for this specific entity
		g_theRenderer->BeginDrawCommands( m_uniformBufferBinding, m_fontTextureBinding );
		g_theRenderer->Draw( m_textVertexBufferBinding );
	}
	
	//-----------------------------------Hovering: show as UI----------------------
	if (m_isHovering || m_showDetail) {
		// acquire and set the descriptor set for this specific entity
		g_theRenderer->BeginDrawCommands( m_UIBinding, m_textureBinding );
		// copy the ubo data to the graphics card
		g_theRenderer->UpdateSharedModelUniformBuffer( m_UIBinding, (void*)&m_UIMatrix, sizeof( m_UIMatrix ) );
		if (m_useIndexBuffer) {
			// draw the entity
			g_theRenderer->DrawIndexed( m_vertexBufferBinding, m_indexBufferBinding );
		}
		else {
			// draw the entity
			g_theRenderer->Draw( m_vertexBufferBinding );
		}
		g_theRenderer->BeginDrawCommands( m_UIBinding, m_fontTextureBinding );
		g_theRenderer->Draw( m_textVertexBufferBinding );
	}
}

uint32_t Card::GetDamage( int damage )
{
	m_curHealth -= damage;
	if (m_curHealth < 0) {
		m_curHealth = 0;
	}
	return 0;
}

void Card::CalculateModelMatrix( Mat44& modelMat )
{
	modelMat = Mat44( Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), m_position );
	modelMat.Append( m_orientation.GetMatrix() );
	if (m_isHovering) {
		modelMat.Append( Mat44( Vec3( 1.06f, 0.f, 0.f ), Vec3( 0.f, 1.06f, 0.f ), Vec3( 0.f, 0.f, 1.06f ), Vec3( 0.f, 0.f, 0.f ) ) );
	}

	if (m_isHovering) {
		constexpr float startAngle = -35.f;
		constexpr float maxShowingTime = 0.35f;
		float normalizedTime = SmoothStop2( RangeMapClamped( m_hoveringTimer, 0.f, maxShowingTime, 0.f, 1.f ) );
		float rollAngle = RangeMap( normalizedTime, 0.f, 1.f, startAngle, 0.f );
		Vec3 centerPos = Vec3( 160.f, 0.f, 200.f ) + Vec3( -CosDegrees( rollAngle ), 0.f, SinDegrees( rollAngle ) ) * CardWidth * 0.5f;
		m_UIMatrix = Mat44( Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), centerPos );
		m_UIMatrix.Append( Euler( 0.f, rollAngle, 0.f ).GetMatrix() );
	}
	else if(m_showDetail){
		Vec3 centerPos = Vec3( 160.f - CardWidth * 0.5f, 0.f, 200.f );
		m_UIMatrix = Mat44( Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), centerPos );
	}
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
