#pragma once
#include "Core/EngineFwdMinor.h"
#include <vector>

class Entity2D {
public:
	Entity2D( Vec2 const& position = Vec2(), float orientation = 0.f );
	~Entity2D();
	void BeginPlay();
	void Update( float deltaSeconds );
	void Render() const;
protected:
	Vec2 m_position;
	float m_orientationDegrees;
	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;
	Shader* m_shader;
};

class Entity3D {
public:
	Entity3D( Vec3 const& position = Vec3(), Euler const& orientation = Euler() );
	~Entity3D();
	/// Begin play will be called right after an entity is created
	void BeginPlay();
	/// Update will be called each frame
	void Update( float deltaSeconds );
	/// Render will be called each frame
	void Render() const;
protected:
	void CalculateModelMatrix( Mat44& modelMat );
protected:
	// positions
	Vec3 m_position;
	Euler m_orientation;

	// render variables
	VertexBufferBinding m_vertexBufferBinding;
	IndexBufferBinding m_indexBufferBinding;
	UniformBufferBinding m_uniformBufferBinding;
	Shader* m_shader;
	Mat44 m_modelMatrix;
};