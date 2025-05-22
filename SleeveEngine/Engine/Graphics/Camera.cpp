#include "Graphics/Camera.h"
#include "Graphics/GraphicsFwd.h"
#include "Graphics/Renderer.h"
#include "Core/EngineCommon.h"

PerspectiveCamera::PerspectiveCamera()
{
}

PerspectiveCamera::~PerspectiveCamera()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		delete m_cameraUniformBuffers[i];
	}
}

void PerspectiveCamera::BeginPlay()
{
	m_cameraUniformBuffers.reserve( MAX_FRAMES_IN_FLIGHT );
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		m_cameraUniformBuffers.push_back( g_theRenderer->CreateUniformBuffer( sizeof( CameraUniformBufferObject ) ) );
	}
}

Mat44 PerspectiveCamera::GetViewMatrix() const
{
	Mat44 rotationMatrix = m_orientation.GetMatrix();
	Vec4 tempVector = rotationMatrix.m_cols[0];
	rotationMatrix.m_cols[0] = -rotationMatrix.m_cols[1];
	rotationMatrix.m_cols[1] = rotationMatrix.m_cols[2];
	rotationMatrix.m_cols[2] = -tempVector;
	rotationMatrix.Transpose();
	Mat44 translationMatrix = Mat44();
	translationMatrix.m_cols[3] = Vec4( -m_position.x, -m_position.y, -m_position.z, 1.f );
	rotationMatrix.Append( translationMatrix );
	return rotationMatrix;
}

Mat44 PerspectiveCamera::GetProjectionMatrix() const
{
	return ::GetPerspectiveMatrix( m_fov, m_aspect, m_zNear, m_zFar );
}

Vec3 PerspectiveCamera::TransferScreenPosToWorld( Vec2 const& screenPos ) const
{
	// make sure this is normalized to 0-1
	Vec3 iBasis, jBasis, kBasis;
	m_orientation.GetForwardAndLeftAndUpVector( iBasis, jBasis, kBasis );
	Vec3 worldPos = m_position + iBasis * m_zNear;
	float h = 2.f * m_zNear * SinDegrees( m_fov * 0.5f ) / CosDegrees( m_fov * 0.5f );
	float w = h * m_aspect;
	worldPos -= jBasis * (screenPos.x - 0.5f) * w;
	worldPos += kBasis * (screenPos.y - 0.5f) * h;
	return worldPos;
}

Vec2 PerspectiveCamera::TransferScreenPosToZPlane( Vec2 const& normalizedScreenPos, float z ) const
{
	Vec3 worldPos = TransferScreenPosToWorld( normalizedScreenPos );
	float zeroPlaneFraction = GetFractionWithinRange( z, m_position.z, worldPos.z );
	return Interpolate( m_position, worldPos, zeroPlaneFraction );
}

OrthographicCamera::OrthographicCamera()
{

}

OrthographicCamera::~OrthographicCamera()
{
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		delete m_cameraUniformBuffers[i];
	}
}

void OrthographicCamera::BeginPlay()
{
	m_cameraUniformBuffers.reserve( MAX_FRAMES_IN_FLIGHT );
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		m_cameraUniformBuffers.push_back( g_theRenderer->CreateUniformBuffer( sizeof( CameraUniformBufferObject ) ) );
	}
}

Mat44 OrthographicCamera::GetViewMatrix() const
{
	return Mat44();
}

Mat44 OrthographicCamera::GetProjectionMatrix() const
{
	float right = m_bounds.m_maxs.x;
	float left = m_bounds.m_mins.x;
	float top = m_bounds.m_maxs.y;
	float bottom = m_bounds.m_mins.y;
	Mat44 retMat;

 	retMat.m_values[Mat44::Ix] = 2.f / (right - left);
 	retMat.m_values[Mat44::Jy] = -2.f / (top - bottom);
 	retMat.m_values[Mat44::Kz] = -1.f / (m_near - m_far);
 	retMat.m_values[Mat44::Tx] = (left + right) / (left - right);
 	retMat.m_values[Mat44::Ty] = (top + bottom) / (top - bottom);
 	retMat.m_values[Mat44::Tz] = m_near / (m_near - m_far);
	return retMat;
}
