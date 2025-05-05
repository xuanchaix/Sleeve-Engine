#pragma once
#include <vector>
#include "Math/MathFwd.h"
class VertexBuffer;
class IndexBuffer;
class UniformBuffer;

struct EntityUniformBuffers {
	std::vector<UniformBuffer*> m_uniformBuffersModel;
};

struct CameraUniformBufferObject {
	Mat44 m_viewMatrix;
	Mat44 m_projectionMatrix;
};

struct ModelUniformBufferObject {
	Mat44 m_modelMatrix;
};

struct VertexBufferBinding {
	VertexBuffer* m_vertexBuffer = nullptr;
	uint64_t m_vertexBufferOffset = 0;
	uint32_t m_vertexBufferVertexCount = 0;
};

struct IndexBufferBinding {
	IndexBuffer* m_indexBuffer = nullptr;
	uint64_t m_indexBufferOffset = 0;
	uint32_t m_indexBufferIndexCount = 0;
};

typedef uint32_t UniformBufferDataBindingFlags;

constexpr UniformBufferDataBindingFlags UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2 = 0x00000001;

struct UniformBufferBinding {
	uint32_t m_flags = 0;
	uint64_t m_modelUniformBufferOffset = 0;
};

constexpr uint32_t WINDOW_WIDTH = 1600;
constexpr uint32_t WINDOW_HEIGHT = 800;
constexpr int MAX_FRAMES_IN_FLIGHT = 2;
constexpr uint64_t INITIAL_SHARED_VERTEX_BUFFER_MAX_SIZE = 1000000; // around 1MB
constexpr uint64_t INITIAL_SHARED_INDEX_BUFFER_MAX_SIZE = 100000; // around 100KB
constexpr uint64_t INITIAL_MODEL_UNIFORM_BUFFER_MAX_COUNT = 4096;
constexpr uint64_t INITIAL_MODEL_UNIFORM_BUFFER_MAX_SIZE = sizeof( ModelUniformBufferObject ) * INITIAL_MODEL_UNIFORM_BUFFER_MAX_COUNT;