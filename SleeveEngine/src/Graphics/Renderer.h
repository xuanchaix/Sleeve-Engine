#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <vector>
#include <optional>
#include <set>
#include <algorithm>
#include <array>
#include <unordered_map>

#include "Core/EngineCommon.h"
#include "Graphics/GraphicsCommon.h"

struct PerspectiveCamera;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
	std::optional<uint32_t> m_graphicsFamily;
	std::optional<uint32_t> m_presentFamily;

	bool isComplete() {
		return m_graphicsFamily.has_value() && m_presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR m_capabilities;
	std::vector<VkSurfaceFormatKHR> m_formats;
	std::vector<VkPresentModeKHR> m_presentModes;
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class Renderer {
	friend class Shader;
	friend class DescriptorPools;
public:
	void InitVulkan();
	void Cleanup();
	void WaitForCleanup();
	void BeginFrame();
	void EndFrame();

	void BeginCamera( PerspectiveCamera const& camera );
	void EndCamera( PerspectiveCamera const& camera );

	void DrawSingleBufferIndexed( VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint64_t vertexOffset = 0, uint64_t indexOffset = 0 );
	void Draw( VertexBufferBinding const& vertexBinding );
	void DrawIndexed( VertexBufferBinding const& vertexBinding, IndexBufferBinding const& indexBinding );

	void BindShader( Shader* shader );
	void BeginDrawCommands( EntityUniformBuffers const& uniformBuffers );
	void BeginDrawCommands( UniformBufferBinding const& binding );

	void UpdateUniformBuffer( UniformBuffer* uniformBuffer, void* newData, size_t dataSize );
	void UpdateSharedModelUniformBuffer( UniformBufferBinding const& binding, void* newData, size_t dataSize );
	uint32_t GetCurFrameNumber() const;

	Texture* CreateTextureFromFile( std::string const& fileName );
	//Texture* CreateTexture();
	IndexBuffer* CreateIndexBuffer( void* indexData, uint64_t size, uint32_t indexCount );
	VertexBuffer* CreateVertexBuffer( void* vertexData, uint64_t size, uint32_t vertexCount );
	UniformBuffer* CreateUniformBuffer( uint64_t size );
	Shader* CreateShader( std::string const& fileName );

	VertexBufferBinding AddVertsDataToSharedVertexBuffer( void* vertexData, uint64_t size, uint32_t vertexCount );
	IndexBufferBinding AddIndicesDataToSharedIndexBuffer( void* indexData, uint64_t size, uint32_t indexCount );
	UniformBufferBinding AddDataToSharedUniformBuffer( UniformBufferDataBindingFlags flags );
	void ReturnMemoryToSharedBuffer( VertexBufferBinding const& vBinding, IndexBufferBinding const& iBinding, UniformBufferBinding const& uBinding );
protected:
	void DeferredDestroyBuffer( UniformBuffer* buffer );
	void DeferredDestroyBuffer( VertexBuffer* buffer );
	void DeferredDestroyBuffer( IndexBuffer* buffer );
	void DeferredDestroyBuffer( VkBuffer buffer, VkDeviceMemory deviceMemory );
	void CreateInstance();

	void SetupDebugMessenger();

	void CreateSurface();

	void PickPhysicalDevice();

	void CreateLogicalDevice();

	void CreateSwapChain();

	void CreateSwapChainImageViews();

	void CreateRenderPass();

	void CreateFramebuffers();

	void CreateCommandPools();

	void CreateDepthResources();

	void CreateTextureSampler();

	VkImageView CreateImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags );

	void CreateImage( uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory );

	uint64_t GetDescriptorPoolKey( uint8_t numOfUniformBuffers, uint8_t numOfSamplers );

	DescriptorPools* GetOrCreateDescriptorPools( uint8_t numOfUniformBuffers, uint8_t numOfSamplers );

	void CreateCommandBuffers();

	void CreateStagingBuffer();

	void CreateSyncObjects();

	VkCommandBuffer BeginSingleTimeCommands();

	void EndSingleTimeCommands( VkCommandBuffer commandBuffer );

	void CleanupSwapChain();

	void RecreateSwapChain();

	VertexBuffer* CreateSharedVertexBuffer( uint64_t size, uint32_t stride );

	IndexBuffer* CreateSharedIndexBuffer( uint64_t size );
	
	UniformBuffer* CreateSharedUniformBuffer( uint64_t size, uint32_t stride );

	void CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory );

	void CopyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset = 0, VkDeviceSize dstOffset = 0 );

	void TransitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout );

	void CopyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height );

	QueueFamilyIndices FindQueueFamilies( VkPhysicalDevice device );

	SwapChainSupportDetails QuerySwapChainSupport( VkPhysicalDevice device );

	VkSurfaceFormatKHR ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats );

	VkPresentModeKHR ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes );

	VkExtent2D ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities );

	void PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo );

	VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger );

	static void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator );

	bool IsDeviceSuitable( VkPhysicalDevice device );

	bool CheckValidationLayerSupport();

	int GetTransferQueueFamily();

	VkFormat FindSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features );

	VkFormat FindDepthFormat();

	bool HasStencilComponent( VkFormat format );

	uint32_t FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties );

	std::vector<const char*> GetRequiredExtensions();

	bool CheckDeviceExtensionSupport( VkPhysicalDevice device );

	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData );

protected:
	VkInstance m_instance;
	VkDevice m_device;
	VkDebugUtilsMessengerEXT m_debugMessenger;
	VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
	VkQueue m_graphicsQueue;
	VkQueue m_presentQueue;
	VkQueue m_transferQueue;
	VkSurfaceKHR m_surface;
	VkSwapchainKHR m_swapChain;
	std::vector<VkImage> m_swapChainImages;
	VkFormat m_swapChainImageFormat;
	VkExtent2D m_swapChainExtent;
	std::vector<VkImageView> m_swapChainImageViews;

	VkRenderPass m_renderPass;
	VkCommandPool m_commandPool;
	Texture* m_depthTexture = nullptr;
	VkSampler m_textureSampler;
	std::unordered_map<uint64_t, DescriptorPools*> m_descriptorPoolsDictionary;
	Shader* m_currentShader = nullptr;
	std::vector<VkCommandPool> m_transferCommandPools;
	std::vector<VkCommandBuffer> m_transferCommandBuffers;

	std::vector<VkCommandBuffer> m_commandBuffers;
	std::vector<VkSemaphore> m_imageAvailableSemaphores;
	std::vector<VkSemaphore> m_renderFinishedSemaphores;
	std::vector<VkSemaphore> m_transferCompleteSemaphores;
	std::vector<VkFence> m_inFlightFences;
	std::vector<VkFence> m_transferFences;

	std::vector<VkFramebuffer> m_swapChainFramebuffers;
	uint32_t m_currentFrame = 0;

	uint32_t m_curImageIndex = 0;

	VertexBuffer* m_sharedMeshVertexBuffer = nullptr;
	IndexBuffer* m_sharedMeshIndexBuffer = nullptr;
	std::array<UniformBuffer*, MAX_FRAMES_IN_FLIGHT> m_sharedModelUniformBuffers;

	std::vector<BufferCopyCommand> m_copyCommands;
	std::vector<BufferPendingToDestroy> m_pendingDestroyBuffers;
	std::vector<StagingBuffer*> m_stagingBuffers;
};