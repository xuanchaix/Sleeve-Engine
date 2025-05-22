#pragma once
#include "Core/EngineFwdMinor.h"
#include <vector>

class EntityBase {
public:
	/// Begin play will be called right after an entity is created
	virtual void BeginPlay() = 0;
	/// Update will be called each frame
	virtual void Update( float deltaSeconds ) = 0;
	/// Render will be called each frame
	virtual void Render() const = 0;

	bool m_useIndexBuffer = true;
};

class Entity2D : public EntityBase {
public:
	Entity2D( Vec2 const& position = Vec2(), float orientation = 0.f );
	~Entity2D();
	/// Begin play will be called right after an entity is created
	virtual void BeginPlay() override;
	/// Update will be called each frame
	virtual void Update( float deltaSeconds ) override;
	/// Render will be called each frame
	virtual void Render() const override;
protected:
	// positions
	Vec2 m_position;
	float m_orientationDegrees;

	// render variables
	VertexBufferBinding m_vertexBufferBinding;
	IndexBufferBinding m_indexBufferBinding;
	UniformBufferBinding m_uniformBufferBinding;
	Shader* m_shader;
	Mat44 m_modelMatrix;
	std::vector<VertexPCU3D> m_vertices;
	std::vector<uint16_t> m_indices;
};

class Entity3D : public EntityBase {
public:
	Entity3D( Vec3 const& position = Vec3(), Euler const& orientation = Euler(), bool isTest = false );
	~Entity3D();
	/// Begin play will be called right after an entity is created
	virtual void BeginPlay() override;
	/// Update will be called each frame
	virtual void Update( float deltaSeconds ) override;
	/// Render will be called each frame
	virtual void Render() const override;
protected:
	virtual void CalculateModelMatrix( Mat44& modelMat );

public:
	// positions
	Vec3 m_position;
	Euler m_orientation;
protected:
	// render variables
	VertexBufferBinding m_vertexBufferBinding;
	IndexBufferBinding m_indexBufferBinding;
	UniformBufferBinding m_uniformBufferBinding;
	TextureBinding m_textureBinding;
	Shader* m_shader;
	Mat44 m_modelMatrix;
	std::vector<VertexPCU3D> m_vertices;
	std::vector<uint16_t> m_indices;
};