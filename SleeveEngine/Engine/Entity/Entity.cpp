#include "Entity/Entity.h"
#include "Graphics/Renderer.h"
#include <vector>
#include <chrono>

const std::vector<VertexPCU3D> vertices = {
	{Vec3{-0.5f, -0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 0.0f}},
	{Vec3{0.5f, -0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 0.0f}},
	{Vec3{0.5f, 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{1.0f, 1.0f}},
	{Vec3{-0.5f, 0.5f, 0.0f}, Rgba8{255, 255, 255, 255}, Vec2{0.0f, 1.0f}},
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
};

Entity3D::Entity3D( Vec3 const& position /*= Vec3()*/, Euler const& orientation /*= Euler() */ )
	:m_position(position), m_orientation(orientation)
{

}

Entity3D::~Entity3D()
{
	g_theRenderer->ReturnMemoryToSharedBuffer( m_vertexBufferBinding, m_indexBufferBinding, m_uniformBufferBinding );
}

void Entity3D::BeginPlay()
{
	// initialize rendering objects
	m_vertexBufferBinding = g_theRenderer->AddVertsDataToSharedVertexBuffer( (void*)vertices.data(), sizeof( vertices[0] ) * vertices.size(), (uint32_t)vertices.size() );
	m_indexBufferBinding = g_theRenderer->AddIndicesDataToSharedIndexBuffer( (void*)indices.data(), sizeof( indices[0] ) * indices.size(), (uint32_t)indices.size() );

	// initialize uniform buffers
	m_uniformBufferBinding = g_theRenderer->AddDataToSharedUniformBuffer( UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2 );

	m_shader = g_theResourceManager->GetOrLoadShader( "shader" );
}

void Entity3D::Update( float deltaSeconds )
{
	m_orientation.yaw += deltaSeconds * 90.0f;
	CalculateModelMatrix( m_modelMatrix );
}

void Entity3D::Render() const
{
	// if the current pipeline is not my shader's pipeline, bind my shader's pipeline
	g_theRenderer->BindShader( m_shader );
	// acquire and set the descriptor set for this specific entity
	g_theRenderer->BeginDrawCommands( m_uniformBufferBinding );
	// copy the ubo data to the graphics card
	g_theRenderer->UpdateSharedModelUniformBuffer( m_uniformBufferBinding, (void*)&m_modelMatrix, sizeof( m_modelMatrix ) );
	// draw the entity
	g_theRenderer->DrawIndexed( m_vertexBufferBinding, m_indexBufferBinding );
}

void Entity3D::CalculateModelMatrix( Mat44& modelMat )
{
	modelMat = Mat44( Vec3( 1.f, 0.f, 0.f ), Vec3( 0.f, 1.f, 0.f ), Vec3( 0.f, 0.f, 1.f ), m_position );
	modelMat.Append( m_orientation.GetMatrix() );
}
