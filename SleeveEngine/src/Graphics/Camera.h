#pragma once
#include "Math/MathFwd.h"
#include <vector>

class UniformBuffer;

struct PerspectiveCamera {
	PerspectiveCamera();
	~PerspectiveCamera();

	void BeginPlay();
	Mat44 GetViewMatrix() const;
	Mat44 GetPerspectiveProjectionMatrix() const;

	Euler m_orientation;
	Vec3 m_position;

	float m_aspect = 1.f;
	float m_fov = 60.f;
	float m_zNear = 0.f;
	float m_zFar = 100.f;
	std::vector<UniformBuffer*> m_cameraUniformBuffers;
};

struct OrthographicCamera {
	OrthographicCamera();
	~OrthographicCamera();

	Vec2 m_xyPos = Vec2( 0.f, 0.f );
	float m_near = 0.f;
	float m_far = 1.f;
	std::vector<UniformBuffer*> m_cameraUniformBuffers;
};