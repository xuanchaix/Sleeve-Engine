#include "Entity/Entity.h"
#include "Graphics/Renderer.h"
#include <vector>
#include <chrono>

Entity3D::Entity3D( Vec3 const& position /*= Vec3()*/, Euler const& orientation /*= Euler() */, bool isTest )
	:m_position(position), m_orientation(orientation)
{
	if (isTest) {
		AddVertsForAABB2D( m_vertices, AABB2( Vec2( 60.f, 80.f ), Vec2( 400.f, 230.f ) ), Rgba8( 255, 255, 255 ) );
		m_useIndexBuffer = false;
	}
	else {
		m_vertices = { {Vec3{-0.5f, -0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 0.0f}},
						{Vec3{0.5f, -0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 0.0f}},
						{Vec3{0.5f, 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 1.0f}},
						{Vec3{-0.5f, 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 1.0f}}, };
		m_indices = { 0, 1, 2, 2, 3, 0, };
	}
}

Entity3D::~Entity3D()
{
	g_theRenderer->ReturnMemoryToSharedBuffer( m_vertexBufferBinding, m_indexBufferBinding, m_uniformBufferBinding );
}

void Entity3D::BeginPlay()
{
	// initialize rendering objects
	m_vertexBufferBinding = g_theRenderer->AddVertsDataToSharedVertexBuffer( (void*)m_vertices.data(), sizeof( m_vertices[0] ) * m_vertices.size(), (uint32_t)m_vertices.size() );
	if (m_useIndexBuffer) {
		m_indexBufferBinding = g_theRenderer->AddIndicesDataToSharedIndexBuffer( (void*)m_indices.data(), sizeof( m_indices[0] ) * m_indices.size(), (uint32_t)m_indices.size() );
	}

	//m_textureBinding.m_texture = g_theResourceManager->GetOrLoadTexture( "Data/Textures/texture.png" );
	m_textureBinding.m_texture = g_theResourceManager->GetWhiteTexture();

	// initialize uniform buffers
	m_uniformBufferBinding = g_theRenderer->AddDataToSharedUniformBuffer( UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2 );

	m_shader = g_theResourceManager->GetOrLoadShader( "shader" );
}

void Entity3D::Update( float deltaSeconds )
{
	//m_orientation.yaw += deltaSeconds * 90.0f;
	CalculateModelMatrix( m_modelMatrix );
}

void Entity3D::Render() const
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
}

void Entity3D::CalculateModelMatrix( Mat44& modelMat )
{
	modelMat = Mat44( Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), m_position );
	modelMat.Append( m_orientation.GetMatrix() );
}
