#pragma once
#include "Math/MathFwd.h"
#include <vector>

class UniformBuffer;

struct Camera {
	virtual void BeginPlay() = 0;
	virtual Mat44 GetViewMatrix() const = 0;
	virtual Mat44 GetProjectionMatrix() const = 0;
	std::vector<UniformBuffer*> m_cameraUniformBuffers;
};

struct PerspectiveCamera : public Camera {
	PerspectiveCamera();
	~PerspectiveCamera();

	virtual void BeginPlay();
	/// Get view matrix of the camera, including render matrix
	virtual Mat44 GetViewMatrix() const;
	/// Get perspective projection matrix of the camera
	virtual Mat44 GetProjectionMatrix() const;

	Vec3 TransferScreenPosToWorld( Vec2 const& screenPos ) const;
	Vec2 TransferScreenPosToZPlane( Vec2 const& normalizedScreenPos, float z ) const;

	Euler m_orientation;
	Vec3 m_position;

	float m_aspect = 1.f;
	float m_fov = 60.f;
	float m_zNear = 0.f;
	float m_zFar = 100.f;
};

struct OrthographicCamera : public Camera {
	OrthographicCamera();
	~OrthographicCamera();

	virtual void BeginPlay();
	/// Get view matrix of the camera, including render matrix
	virtual Mat44 GetViewMatrix() const;
	/// Get orthographic projection matrix of the camera
	virtual Mat44 GetProjectionMatrix() const;

	AABB2 m_bounds;
	float m_near = 0.f;
	float m_far = 1.f;
};