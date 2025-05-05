#include "Graphics/Renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Core/App.h"

PerspectiveCamera* globalCamera = nullptr;

void Renderer::InitVulkan()
{
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface();
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateRenderPass();
	CreateCommandPool();
	CreateDepthResources();
	CreateFramebuffers();
	CreateTextureSampler();
	CreateCommandBuffers();
	CreateSyncObjects();

	m_sharedMeshVertexBuffer = CreateSharedVertexBuffer( INITIAL_SHARED_VERTEX_BUFFER_MAX_SIZE, sizeof( VertexPCU3D ) );
	m_sharedMeshIndexBuffer = CreateSharedIndexBuffer( INITIAL_SHARED_INDEX_BUFFER_MAX_SIZE );
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		m_sharedModelUniformBuffers[i] = CreateSharedUniformBuffer( INITIAL_MODEL_UNIFORM_BUFFER_MAX_SIZE, sizeof(ModelUniformBufferObject) );
	}

	globalCamera->m_aspect = m_swapChainExtent.width / (float)m_swapChainExtent.height;
	globalCamera->m_fov = 60.f;
	globalCamera->m_zNear = 0.1f;
	globalCamera->m_zFar = 100.f;
	globalCamera->m_position = Vec3( 0.f, -1.f, 1.73f );
	globalCamera->m_orientation = Euler( 90.f, 60.f, 0.f );
}

void Renderer::Cleanup()
{
	CleanupSwapChain();
	delete m_depthTexture;
	delete m_sharedMeshIndexBuffer;
	delete m_sharedMeshVertexBuffer;
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		delete m_sharedModelUniformBuffers[i];
	}
	vkDestroySampler( m_device, m_textureSampler, nullptr );
	for (auto& pair : m_descriptorPoolsDictionary) {
		delete pair.second;
	}
	vkDestroyRenderPass( m_device, m_renderPass, nullptr );

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroySemaphore( m_device, m_renderFinishedSemaphores[i], nullptr );
		vkDestroySemaphore( m_device, m_imageAvailableSemaphores[i], nullptr );
	}
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		vkDestroyFence( m_device, m_inFlightFences[i], nullptr );
	}

	vkDestroyCommandPool( m_device, m_commandPool, nullptr );

	vkDestroyDevice( m_device, nullptr );

	if (enableValidationLayers) {
		DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, nullptr );
	}

	vkDestroySurfaceKHR( m_instance, m_surface, nullptr );
	vkDestroyInstance( m_instance, nullptr );

}

void Renderer::WaitForCleanup()
{
	for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
		vkWaitForFences( m_device, 1, &m_inFlightFences[i], VK_TRUE, UINT64_MAX );
	}
	vkQueueWaitIdle( m_graphicsQueue );
	vkQueueWaitIdle( m_presentQueue );
}

void Renderer::BeginFrame()
{
	vkWaitForFences( m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX );

	VkResult result = vkAcquireNextImageKHR( m_device, m_swapChain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_curImageIndex );

	if (result == VK_ERROR_OUT_OF_DATE_KHR) {
		RecreateSwapChain();
		return;
	}
	else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
		throw std::runtime_error( "failed to acquire swap chain image!" );
	}

	// Only reset the fence if we are submitting work
	vkResetFences( m_device, 1, &m_inFlightFences[m_currentFrame] );

	vkResetCommandBuffer( m_commandBuffers[m_currentFrame], 0 );

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = 0; // Optional
	beginInfo.pInheritanceInfo = nullptr; // Optional

	if (vkBeginCommandBuffer( m_commandBuffers[m_currentFrame], &beginInfo ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to begin recording command buffer!" );
	}

	for (auto& pair : m_descriptorPoolsDictionary) {
		pair.second->BeginFrame();
	}

	VkRenderPassBeginInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass = m_renderPass;
	renderPassInfo.framebuffer = m_swapChainFramebuffers[m_curImageIndex];
	renderPassInfo.renderArea.offset = { 0, 0 };
	renderPassInfo.renderArea.extent = m_swapChainExtent;

	std::array<VkClearValue, 2> clearValues{};
	clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	clearValues[1].depthStencil = { 1.0f, 0 };

	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues = clearValues.data();

	vkCmdBeginRenderPass( m_commandBuffers[m_currentFrame], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );

	m_currentShader = nullptr;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_swapChainExtent.width);
	viewport.height = static_cast<float>(m_swapChainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vkCmdSetViewport( m_commandBuffers[m_currentFrame], 0, 1, &viewport );

	VkRect2D scissor{};
	scissor.offset = { 0, 0 };
	scissor.extent = m_swapChainExtent;
	vkCmdSetScissor( m_commandBuffers[m_currentFrame], 0, 1, &scissor );
}

void Renderer::EndFrame()
{
	vkCmdEndRenderPass( m_commandBuffers[m_currentFrame] );

	if (vkEndCommandBuffer( m_commandBuffers[m_currentFrame] ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to record command buffer!" );
	}

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[m_currentFrame];
	VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame] ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to submit draw command buffer!" );
	}

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	VkSwapchainKHR swapChains[] = { m_swapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &m_curImageIndex;
	presentInfo.pResults = nullptr; // Optional
	VkResult result = vkQueuePresentKHR( m_presentQueue, &presentInfo );

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || g_theApp->HasFrameBufferResized()) {
		g_theApp->SetFrameBufferResized( false );
		RecreateSwapChain();
	}
	else if (result != VK_SUCCESS) {
		throw std::runtime_error( "failed to present swap chain image!" );
	}

	m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void Renderer::BeginCamera( PerspectiveCamera const& camera )
{
	CameraUniformBufferObject cubo;
	cubo.m_projectionMatrix = camera.GetPerspectiveProjectionMatrix();
	cubo.m_viewMatrix = camera.GetViewMatrix();
	UpdateUniformBuffer( camera.m_cameraUniformBuffers[GetCurFrameNumber()], (void*)&cubo, sizeof(cubo));
}

void Renderer::EndCamera( PerspectiveCamera const& camera )
{

}

void Renderer::CreateInstance()
{
	if (enableValidationLayers && !CheckValidationLayerSupport()) {
		throw std::runtime_error( "validation layers requested, but not available!" );
	}

	VkApplicationInfo appInfo{};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION( 1, 0, 0 );
	appInfo.apiVersion = VK_API_VERSION_1_0;

	auto extensions = GetRequiredExtensions();

	VkInstanceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
	createInfo.ppEnabledExtensionNames = extensions.data();
	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
	if (enableValidationLayers) {
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		createInfo.ppEnabledLayerNames = validationLayers.data();

		PopulateDebugMessengerCreateInfo( debugCreateInfo );
		createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
	}
	else {
		createInfo.enabledLayerCount = 0;

		createInfo.pNext = nullptr;
	}

	if (vkCreateInstance( &createInfo, nullptr, &m_instance ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create instance!" );
	}
}

void Renderer::SetupDebugMessenger()
{
	if (!enableValidationLayers) return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo{};
	PopulateDebugMessengerCreateInfo( createInfo );
	if (CreateDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugMessenger ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to set up debug messenger!" );
	}
}

void Renderer::CreateSurface()
{
	if (glfwCreateWindowSurface( m_instance, g_theApp->m_window, nullptr, &m_surface ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create window surface!" );
	}
}

void Renderer::PickPhysicalDevice()
{
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, nullptr );
	if (deviceCount == 0) {
		throw std::runtime_error( "failed to find GPUs with Vulkan support!" );
	}
	std::vector<VkPhysicalDevice> devices( deviceCount );
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, devices.data() );

	for (const auto& device : devices) {
		if (IsDeviceSuitable( device )) {
			m_physicalDevice = device;
			break;
		}
	}

	if (m_physicalDevice == VK_NULL_HANDLE) {
		throw std::runtime_error( "failed to find a suitable GPU!" );
	}
}

void Renderer::CreateLogicalDevice()
{
	QueueFamilyIndices indices = FindQueueFamilies( m_physicalDevice );

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.m_graphicsFamily.value(), indices.m_presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo{};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back( queueCreateInfo );
	}

	VkPhysicalDeviceFeatures deviceFeatures{};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	VkDeviceCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	createInfo.ppEnabledExtensionNames = deviceExtensions.data();
	createInfo.pEnabledFeatures = &deviceFeatures;

	if (vkCreateDevice( m_physicalDevice, &createInfo, nullptr, &m_device ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create logical device!" );
	}

	vkGetDeviceQueue( m_device, indices.m_graphicsFamily.value(), 0, &m_graphicsQueue );
	vkGetDeviceQueue( m_device, indices.m_presentFamily.value(), 0, &m_presentQueue );
}

void Renderer::CreateSwapChain()
{
	SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( m_physicalDevice );

	VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat( swapChainSupport.m_formats );
	VkPresentModeKHR presentMode = ChooseSwapPresentMode( swapChainSupport.m_presentModes );
	VkExtent2D extent = ChooseSwapExtent( swapChainSupport.m_capabilities );
	uint32_t imageCount = swapChainSupport.m_capabilities.minImageCount + 1;
	if (swapChainSupport.m_capabilities.maxImageCount > 0 && imageCount > swapChainSupport.m_capabilities.maxImageCount) {
		imageCount = swapChainSupport.m_capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	QueueFamilyIndices indices = FindQueueFamilies( m_physicalDevice );
	uint32_t queueFamilyIndices[] = { indices.m_graphicsFamily.value(), indices.m_presentFamily.value() };

	if (indices.m_graphicsFamily != indices.m_presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.m_capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR( m_device, &createInfo, nullptr, &m_swapChain ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create swap chain!" );
	}

	vkGetSwapchainImagesKHR( m_device, m_swapChain, &imageCount, nullptr );
	m_swapChainImages.resize( imageCount );
	vkGetSwapchainImagesKHR( m_device, m_swapChain, &imageCount, m_swapChainImages.data() );

	m_swapChainImageFormat = surfaceFormat.format;
	m_swapChainExtent = extent;
}

void Renderer::CreateSwapChainImageViews()
{
	m_swapChainImageViews.resize( m_swapChainImages.size() );

	for (uint32_t i = 0; i < m_swapChainImages.size(); i++) {
		m_swapChainImageViews[i] = CreateImageView( m_swapChainImages[i], m_swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT );
	}
}

void Renderer::CreateRenderPass()
{
	VkAttachmentDescription colorAttachment{};
	colorAttachment.format = m_swapChainImageFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	VkAttachmentReference colorAttachmentRef{};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkAttachmentDescription depthAttachment{};
	depthAttachment.format = FindDepthFormat();
	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkAttachmentReference depthAttachmentRef{};
	depthAttachmentRef.attachment = 1;
	depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass{};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;
	subpass.pDepthStencilAttachment = &depthAttachmentRef;
	VkSubpassDependency dependency{};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
	dependency.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
	std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
	VkRenderPassCreateInfo renderPassInfo{};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass( m_device, &renderPassInfo, nullptr, &m_renderPass ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create render pass!" );
	}
}




void Renderer::CreateFramebuffers()
{
	m_swapChainFramebuffers.resize( m_swapChainImageViews.size() );
	for (size_t i = 0; i < m_swapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			m_swapChainImageViews[i],
			m_depthTexture->m_textureImageView
		};

		VkFramebufferCreateInfo framebufferInfo{};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_renderPass;
		framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_swapChainExtent.width;
		framebufferInfo.height = m_swapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer( m_device, &framebufferInfo, nullptr, &m_swapChainFramebuffers[i] ) != VK_SUCCESS) {
			throw std::runtime_error( "failed to create framebuffer!" );
		}
	}
}

void Renderer::CreateCommandPool()
{
	QueueFamilyIndices queueFamilyIndices = FindQueueFamilies( m_physicalDevice );

	VkCommandPoolCreateInfo poolInfo{};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.m_graphicsFamily.value();

	if (vkCreateCommandPool( m_device, &poolInfo, nullptr, &m_commandPool ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create command pool!" );
	}
}

void Renderer::CreateDepthResources()
{
	VkFormat depthFormat = FindDepthFormat();
	m_depthTexture = new Texture( m_device );
	CreateImage( m_swapChainExtent.width, m_swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthTexture->m_textureImage, m_depthTexture->m_textureDeviceMemory );
	m_depthTexture->m_textureImageView = CreateImageView( m_depthTexture->m_textureImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT );

	TransitionImageLayout( m_depthTexture->m_textureImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );
}

void Renderer::CreateTextureSampler()
{
	VkSamplerCreateInfo samplerInfo{};
	samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter = VK_FILTER_LINEAR;
	samplerInfo.minFilter = VK_FILTER_LINEAR;
	samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.anisotropyEnable = VK_TRUE;

	VkPhysicalDeviceProperties properties{};
	vkGetPhysicalDeviceProperties( m_physicalDevice, &properties );
	samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable = VK_FALSE;
	samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
	samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias = 0.0f;
	samplerInfo.minLod = 0.0f;
	samplerInfo.maxLod = 0.0f;
	if (vkCreateSampler( m_device, &samplerInfo, nullptr, &m_textureSampler ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create texture sampler!" );
	}
}

VkImageView Renderer::CreateImageView( VkImage image, VkFormat format, VkImageAspectFlags aspectFlags )
{
	VkImageViewCreateInfo viewInfo{};
	viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image = image;
	viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format = format;
	//viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.aspectMask = aspectFlags;
	viewInfo.subresourceRange.baseMipLevel = 0;
	viewInfo.subresourceRange.levelCount = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount = 1;

	VkImageView imageView;
	if (vkCreateImageView( m_device, &viewInfo, nullptr, &imageView ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create image view!" );
	}

	return imageView;
}

void Renderer::CreateImage( uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory )
{
	VkImageCreateInfo imageInfo{};
	imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageInfo.imageType = VK_IMAGE_TYPE_2D;
	imageInfo.extent.width = width;
	imageInfo.extent.height = height;
	imageInfo.extent.depth = 1;
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.format = format;
	imageInfo.tiling = tiling;
	imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageInfo.usage = usage;
	imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateImage( m_device, &imageInfo, nullptr, &image ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create image!" );
	}

	VkMemoryRequirements memRequirements;
	vkGetImageMemoryRequirements( m_device, image, &memRequirements );

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, properties );

	if (vkAllocateMemory( m_device, &allocInfo, nullptr, &imageMemory ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to allocate image memory!" );
	}

	vkBindImageMemory( m_device, image, imageMemory, 0 );
}

uint64_t Renderer::GetDescriptorPoolKey( uint8_t numOfUniformBuffers, uint8_t numOfSamplers )
{
	return ((numOfUniformBuffers & 0xff) << 8) | ((numOfSamplers & 0xff) << 0);
}

DescriptorPools* Renderer::GetOrCreateDescriptorPools( uint8_t numOfUniformBuffers, uint8_t numOfSamplers )
{
	uint64_t key = GetDescriptorPoolKey( numOfUniformBuffers, numOfSamplers );
	auto iter = m_descriptorPoolsDictionary.find( key );
	if (iter == m_descriptorPoolsDictionary.end()) {
		DescriptorPools* newPools = new DescriptorPools( m_device, numOfUniformBuffers, numOfSamplers );
		m_descriptorPoolsDictionary[key] = newPools;
		return newPools;
	}
	else {
		return iter->second;
	}
}

void Renderer::CreateCommandBuffers()
{
	m_commandBuffers.resize( MAX_FRAMES_IN_FLIGHT );
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)m_commandBuffers.size();

	if (vkAllocateCommandBuffers( m_device, &allocInfo, m_commandBuffers.data() ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to allocate command buffers!" );
	}

// 	VkMemoryRequirements memRequirements;
// 	vkGetBufferMemoryRequirements( m_device, m_vertexBuffer->m_buffer, &memRequirements );
}

void Renderer::CreateSyncObjects()
{
	m_imageAvailableSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	m_renderFinishedSemaphores.resize( MAX_FRAMES_IN_FLIGHT );
	m_inFlightFences.resize( MAX_FRAMES_IN_FLIGHT );

	VkSemaphoreCreateInfo semaphoreInfo{};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo{};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i] ) != VK_SUCCESS ||
			vkCreateSemaphore( m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i] ) != VK_SUCCESS ||
			vkCreateFence( m_device, &fenceInfo, nullptr, &m_inFlightFences[i] ) != VK_SUCCESS) {

			throw std::runtime_error( "failed to create synchronization objects for a frame!" );
		}
	}
}

void Renderer::DrawSingleBufferIndexed( VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer, uint64_t vertexOffset, uint64_t indexOffset )
{
	VkBuffer vertexBuffers[] = { vertexBuffer->m_buffer };
	VkDeviceSize offsets[] = { vertexOffset };
	vkCmdBindVertexBuffers( m_commandBuffers[m_currentFrame], 0, 1, vertexBuffers, offsets );
	vkCmdBindIndexBuffer( m_commandBuffers[m_currentFrame], indexBuffer->m_buffer, indexOffset, VK_INDEX_TYPE_UINT16 );
	vkCmdDrawIndexed( m_commandBuffers[m_currentFrame], indexBuffer->m_indexCount, 1, 0, 0, 0 );
}

void Renderer::Draw( VertexBufferBinding const& vertexBinding )
{
	VkBuffer vertexBuffers[] = { vertexBinding.m_vertexBuffer->m_buffer };
	VkDeviceSize offsets[] = { vertexBinding.m_vertexBufferOffset };
	vkCmdBindVertexBuffers( m_commandBuffers[m_currentFrame], 0, 1, vertexBuffers, offsets );
	vkCmdDraw( m_commandBuffers[m_currentFrame], vertexBinding.m_vertexBufferVertexCount, 1, 0, 0 );
}

void Renderer::DrawIndexed( VertexBufferBinding const& vertexBinding, IndexBufferBinding const& indexBinding )
{
	VkBuffer vertexBuffers[] = { vertexBinding.m_vertexBuffer->m_buffer };
	VkDeviceSize offsets[] = { vertexBinding.m_vertexBufferOffset };
	vkCmdBindVertexBuffers( m_commandBuffers[m_currentFrame], 0, 1, vertexBuffers, offsets );
	vkCmdBindIndexBuffer( m_commandBuffers[m_currentFrame], indexBinding.m_indexBuffer->m_buffer, indexBinding.m_indexBufferOffset, VK_INDEX_TYPE_UINT16 );
	vkCmdDrawIndexed( m_commandBuffers[m_currentFrame], indexBinding.m_indexBufferIndexCount, 1, 0, 0, 0 );
}

void Renderer::BindShader( Shader* shader )
{
	if (m_currentShader != shader) {
		m_currentShader = shader;
		vkCmdBindPipeline( m_commandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_currentShader->m_graphicsPipeline );
	}
}

void Renderer::BeginDrawCommands( EntityUniformBuffers const& uniformBuffers )
{
	m_currentShader->UpdateDescriptorSets( uniformBuffers );
}

void Renderer::BeginDrawCommands( UniformBufferBinding const& binding )
{
	m_currentShader->UpdateDescriptorSets( binding );
}

VkCommandBuffer Renderer::BeginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_commandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers( m_device, &allocInfo, &commandBuffer );

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer( commandBuffer, &beginInfo );

	return commandBuffer;
}

void Renderer::EndSingleTimeCommands( VkCommandBuffer commandBuffer )
{
	vkEndCommandBuffer( commandBuffer );

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
	vkQueueWaitIdle( m_graphicsQueue );

	vkFreeCommandBuffers( m_device, m_commandPool, 1, &commandBuffer );
}

void Renderer::UpdateUniformBuffer( UniformBuffer* uniformBuffer, void* newData, size_t dataSize)
{
	memcpy( uniformBuffer->m_uniformBufferMapped, newData, dataSize );
}

void Renderer::UpdateSharedModelUniformBuffer( UniformBufferBinding const& binding, void* newData, size_t dataSize )
{
	if (binding.m_flags & UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2) {
		memcpy( (void*)((uint64_t)(m_sharedModelUniformBuffers[m_currentFrame])->m_uniformBufferMapped + binding.m_modelUniformBufferOffset), newData, dataSize );
	}
}

uint32_t Renderer::GetCurFrameNumber() const
{
	return m_currentFrame;
}

Texture* Renderer::CreateTextureFromFile( std::string const& fileName )
{
	int texWidth, texHeight, texChannels;
	stbi_set_flip_vertically_on_load( 1 );
	stbi_uc* pixels = stbi_load( fileName.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha );
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	if (!pixels) {
		throw std::runtime_error( "failed to load texture image!" );
	}

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data;
	vkMapMemory( m_device, stagingBufferMemory, 0, imageSize, 0, &data );
	memcpy( data, pixels, static_cast<size_t>(imageSize) );
	vkUnmapMemory( m_device, stagingBufferMemory );

	stbi_image_free( pixels );

	Texture* texture = new Texture( m_device );
	CreateImage( texWidth, texHeight, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, texture->m_textureImage, texture->m_textureDeviceMemory );
	TransitionImageLayout( texture->m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
	CopyBufferToImage( stagingBuffer, texture->m_textureImage, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight) );
	TransitionImageLayout( texture->m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	vkDestroyBuffer( m_device, stagingBuffer, nullptr );
	vkFreeMemory( m_device, stagingBufferMemory, nullptr );

	// create texture image view
	texture->m_textureImageView = CreateImageView( texture->m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT );

	return texture;
}

IndexBuffer* Renderer::CreateIndexBuffer( void* indexData, uint64_t size, uint32_t indexCount )
{
	VkDeviceSize bufferSize = size;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, indexData, (size_t)bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	IndexBuffer* indexBuffer = new IndexBuffer( m_device, indexCount );
	indexBuffer->m_maxSize = size;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer->m_buffer, indexBuffer->m_deviceMemory );

	CopyBuffer( stagingBuffer, indexBuffer->m_buffer, bufferSize );

	vkDestroyBuffer( m_device, stagingBuffer, nullptr );
	vkFreeMemory( m_device, stagingBufferMemory, nullptr );

	return indexBuffer;
}

VertexBuffer* Renderer::CreateVertexBuffer( void* vertexData, uint64_t size, uint32_t vertexCount )
{
	VkDeviceSize bufferSize = size;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, vertexData, (size_t)bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	VertexBuffer* vertexBuffer = new VertexBuffer( m_device, vertexCount );
	vertexBuffer->m_stride = (uint32_t)(size / vertexCount);
	vertexBuffer->m_maxSize = size;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer->m_buffer, vertexBuffer->m_deviceMemory );

	CopyBuffer( stagingBuffer, vertexBuffer->m_buffer, bufferSize );
	vkDestroyBuffer( m_device, stagingBuffer, nullptr );
	vkFreeMemory( m_device, stagingBufferMemory, nullptr );

	return vertexBuffer;
}

UniformBuffer* Renderer::CreateUniformBuffer( uint64_t size )
{
	UniformBuffer* uniformBuffer = new UniformBuffer( m_device );
	uniformBuffer->m_stride = size;
	uniformBuffer->m_maxSize = size;
	CreateBuffer( size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer->m_buffer, uniformBuffer->m_uniformBufferMemory );

	vkMapMemory( m_device, uniformBuffer->m_uniformBufferMemory, 0, size, 0, &uniformBuffer->m_uniformBufferMapped );

	return uniformBuffer;
}

Shader* Renderer::CreateShader( std::string const& fileName )
{
	Shader* shader = new Shader( m_device, this );
	shader->LoadShader( fileName );
	return shader;
}

VertexBufferBinding Renderer::AddVertsDataToSharedVertexBuffer( void* vertexData, uint64_t size, uint32_t vertexCount )
{
	VkDeviceSize bufferSize = size;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, vertexData, (size_t)bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	VkDeviceSize dstOffset = (VkDeviceSize)m_sharedMeshVertexBuffer->m_vertexCount * (VkDeviceSize)m_sharedMeshVertexBuffer->m_stride;

	// if the shared buffer is full, create a bigger one
	if (dstOffset + vertexCount * m_sharedMeshVertexBuffer->m_stride > m_sharedMeshVertexBuffer->m_maxSize) {
		VkDeviceSize newSize = m_sharedMeshVertexBuffer->m_maxSize;
		do { // calculate the new size: multiply the size by 2 until it reaches the requirement
			newSize *= 2;
		} while (dstOffset + vertexCount * m_sharedMeshVertexBuffer->m_stride >= newSize);

		// create a new staging buffer
		VkBuffer newStagingBuffer;
		VkDeviceMemory newStagingBufferMemory;
		CreateBuffer( newSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, newStagingBuffer, newStagingBufferMemory );

		// copy the data from old shared buffer to staging buffer
		if (dstOffset != 0) {
			CopyBuffer( m_sharedMeshVertexBuffer->m_buffer, newStagingBuffer, dstOffset, 0, 0 );
		}

		// destroy the old shared buffer
		vkDestroyBuffer( m_device, m_sharedMeshVertexBuffer->m_buffer, nullptr );
		vkFreeMemory( m_device, m_sharedMeshVertexBuffer->m_deviceMemory, nullptr );

		// create the new shared buffer
		m_sharedMeshVertexBuffer->m_maxSize = newSize;
		CreateBuffer( newSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_sharedMeshVertexBuffer->m_buffer, m_sharedMeshVertexBuffer->m_deviceMemory );

		// copy the data from staging buffer to new shared buffer
		if (dstOffset != 0) {
			CopyBuffer( newStagingBuffer, m_sharedMeshVertexBuffer->m_buffer, dstOffset, 0, 0 );
		}

		// destroy staging buffer
		vkDestroyBuffer( m_device, newStagingBuffer, nullptr );
		vkFreeMemory( m_device, newStagingBufferMemory, nullptr );
	}

	m_sharedMeshVertexBuffer->m_vertexCount += vertexCount;
	CopyBuffer( stagingBuffer, m_sharedMeshVertexBuffer->m_buffer, bufferSize, 0, dstOffset );
	vkDestroyBuffer( m_device, stagingBuffer, nullptr );
	vkFreeMemory( m_device, stagingBufferMemory, nullptr );

	VertexBufferBinding binding;
	binding.m_vertexBuffer = m_sharedMeshVertexBuffer;
	binding.m_vertexBufferOffset = dstOffset;
	binding.m_vertexBufferVertexCount = vertexCount;
	return binding;
}

IndexBufferBinding Renderer::AddIndicesDataToSharedIndexBuffer( void* indexData, uint64_t size, uint32_t indexCount )
{
	constexpr VkDeviceSize intStride = sizeof( uint16_t );
	VkDeviceSize bufferSize = size;

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void* data;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, indexData, (size_t)bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	VkDeviceSize dstOffset = (VkDeviceSize)m_sharedMeshIndexBuffer->m_indexCount * intStride;

	// if the shared buffer is full, create a bigger one
	if (dstOffset + indexCount * intStride > m_sharedMeshIndexBuffer->m_maxSize) {
		VkDeviceSize newSize = m_sharedMeshIndexBuffer->m_maxSize;
		do { // calculate the new size: multiply the size by 2 until it reaches the requirement
			newSize *= 2;
		} while (dstOffset + indexCount * intStride >= newSize);

		// create a new staging buffer
		VkBuffer newStagingBuffer;
		VkDeviceMemory newStagingBufferMemory;
		CreateBuffer( newSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, newStagingBuffer, newStagingBufferMemory );

		// copy the data from old shared buffer to staging buffer
		if (dstOffset != 0) {
			CopyBuffer( m_sharedMeshIndexBuffer->m_buffer, newStagingBuffer, dstOffset, 0, 0 );
		}

		// destroy the old shared buffer
		vkDestroyBuffer( m_device, m_sharedMeshIndexBuffer->m_buffer, nullptr );
		vkFreeMemory( m_device, m_sharedMeshIndexBuffer->m_deviceMemory, nullptr );

		// create the new shared buffer
		m_sharedMeshIndexBuffer->m_maxSize = newSize;
		CreateBuffer( newSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_sharedMeshIndexBuffer->m_buffer, m_sharedMeshIndexBuffer->m_deviceMemory );

		// copy the data from staging buffer to new shared buffer
		if (dstOffset != 0) {
			CopyBuffer( newStagingBuffer, m_sharedMeshIndexBuffer->m_buffer, dstOffset, 0, 0 );
		}

		// destroy staging buffer
		vkDestroyBuffer( m_device, newStagingBuffer, nullptr );
		vkFreeMemory( m_device, newStagingBufferMemory, nullptr );
	}

	m_sharedMeshIndexBuffer->m_indexCount += indexCount;
	CopyBuffer( stagingBuffer, m_sharedMeshIndexBuffer->m_buffer, bufferSize, 0, dstOffset );

	vkDestroyBuffer( m_device, stagingBuffer, nullptr );
	vkFreeMemory( m_device, stagingBufferMemory, nullptr );

	IndexBufferBinding binding;
	binding.m_indexBuffer = m_sharedMeshIndexBuffer;
	binding.m_indexBufferOffset = dstOffset;
	binding.m_indexBufferIndexCount = indexCount;
	return binding;
}

UniformBufferBinding Renderer::AddDataToSharedUniformBuffer( UniformBufferDataBindingFlags flags )
{
	UniformBufferBinding binding;
	binding.m_flags = flags;
	if (flags & UNIFORM_BUFFER_USE_MODEL_CONSTANTS_BINDING_2) {
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
			VkDeviceSize dstOffset = m_sharedModelUniformBuffers[i]->m_currentOffset;
// 			VkDeviceSize const maxSize = m_sharedModelUniformBuffers[i]->m_maxSize;
// 			if (dstOffset + sizeof( ModelUniformBufferObject ) >= maxSize) {
// 				VkDeviceSize newSize = maxSize;
// 				do {
// 					newSize *= 2;
// 				} while (dstOffset + sizeof( ModelUniformBufferObject ) >= newSize);
// 
// 			}
			m_sharedModelUniformBuffers[i]->m_currentOffset += sizeof( ModelUniformBufferObject );
			binding.m_modelUniformBufferOffset = m_sharedModelUniformBuffers[i]->m_currentOffset;
		}
	}
	return binding;
}

void Renderer::CleanupSwapChain()
{
	for (auto framebuffer : m_swapChainFramebuffers) {
		vkDestroyFramebuffer( m_device, framebuffer, nullptr );
	}

	for (auto imageView : m_swapChainImageViews) {
		vkDestroyImageView( m_device, imageView, nullptr );
	}

	vkDestroySwapchainKHR( m_device, m_swapChain, nullptr );
}

void Renderer::RecreateSwapChain()
{
	int width = 0, height = 0;
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize( g_theApp->m_window, &width, &height );
		glfwWaitEvents();
	}

	vkDeviceWaitIdle( m_device );

	CleanupSwapChain();

	CreateSwapChain();
	CreateSwapChainImageViews();
	CreateDepthResources();
	CreateFramebuffers();
}

VertexBuffer* Renderer::CreateSharedVertexBuffer( uint64_t size, uint32_t stride )
{
	VkDeviceSize bufferSize = size;

	VertexBuffer* vertexBuffer = new VertexBuffer( m_device, 0 );
	vertexBuffer->m_stride = stride;
	vertexBuffer->m_maxSize = size;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer->m_buffer, vertexBuffer->m_deviceMemory );

	return vertexBuffer;
}

IndexBuffer* Renderer::CreateSharedIndexBuffer( uint64_t size )
{
	VkDeviceSize bufferSize = size;

	IndexBuffer* indexBuffer = new IndexBuffer( m_device, 0 );
	indexBuffer->m_maxSize = size;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer->m_buffer, indexBuffer->m_deviceMemory );

	return indexBuffer;
}

UniformBuffer* Renderer::CreateSharedUniformBuffer( uint64_t size, uint32_t stride )
{
	UniformBuffer* uniformBuffer = new UniformBuffer( m_device );
	uniformBuffer->m_stride = stride;
	uniformBuffer->m_maxSize = size;
	CreateBuffer( size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffer->m_buffer, uniformBuffer->m_uniformBufferMemory );

	vkMapMemory( m_device, uniformBuffer->m_uniformBufferMemory, 0, size, 0, &uniformBuffer->m_uniformBufferMapped );

	return uniformBuffer;
}

void Renderer::CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory )
{
	static int count = 0;
	++count;
	VkBufferCreateInfo bufferInfo{};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = size;
	bufferInfo.usage = usage;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer( m_device, &bufferInfo, nullptr, &buffer ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to create buffer!" );
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements( m_device, buffer, &memRequirements );

	VkMemoryAllocateInfo allocInfo{};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, properties );

	if (vkAllocateMemory( m_device, &allocInfo, nullptr, &bufferMemory ) != VK_SUCCESS) {
		throw std::runtime_error( "failed to allocate buffer memory!" );
	}

	vkBindBufferMemory( m_device, buffer, bufferMemory, 0 );
}

void Renderer::CopyBuffer( VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size, VkDeviceSize srcOffset, VkDeviceSize dstOffset )
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

	VkBufferCopy copyRegion{};
	copyRegion.size = size;
	copyRegion.dstOffset = dstOffset;
	copyRegion.srcOffset = srcOffset;
	vkCmdCopyBuffer( commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion );

	EndSingleTimeCommands( commandBuffer );
}

void Renderer::TransitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout )
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	VkImageMemoryBarrier barrier{};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument( "unsupported layout transition!" );
	}

	if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

		if (HasStencilComponent( format )) {
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	else {
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage, destinationStage,
		0,
		0, nullptr,
		0, nullptr,
		1, &barrier
	);
	EndSingleTimeCommands( commandBuffer );
}

void Renderer::CopyBufferToImage( VkBuffer buffer, VkImage image, uint32_t width, uint32_t height )
{
	VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	VkBufferImageCopy region{};
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;

	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;

	region.imageOffset = { 0, 0, 0 };
	region.imageExtent = {
		width,
		height,
		1
	};
	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&region
	);
	EndSingleTimeCommands( commandBuffer );
}

QueueFamilyIndices Renderer::FindQueueFamilies( VkPhysicalDevice device )
{
	QueueFamilyIndices indices;
	// Logic to find queue family indices to populate struct with
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, nullptr );

	std::vector<VkQueueFamilyProperties> queueFamilies( queueFamilyCount );
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.data() );

	int i = 0;
	for (const auto& queueFamily : queueFamilies) {
		if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.m_graphicsFamily = i;
		}
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR( device, i, m_surface, &presentSupport );
		if (presentSupport) {
			indices.m_presentFamily = i;
		}

		if (indices.isComplete()) {
			break;
		}
		i++;
	}


	return indices;
}

SwapChainSupportDetails Renderer::QuerySwapChainSupport( VkPhysicalDevice device )
{
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, m_surface, &details.m_capabilities );

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &formatCount, nullptr );
	if (formatCount != 0) {
		details.m_formats.resize( formatCount );
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &formatCount, details.m_formats.data() );
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &presentModeCount, nullptr );
	if (presentModeCount != 0) {
		details.m_presentModes.resize( presentModeCount );
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &presentModeCount, details.m_presentModes.data() );
	}
	return details;
}

VkSurfaceFormatKHR Renderer::ChooseSwapSurfaceFormat( const std::vector<VkSurfaceFormatKHR>& availableFormats )
{
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
			return availableFormat;
		}
	}
	return availableFormats[0];
}

VkPresentModeKHR Renderer::ChooseSwapPresentMode( const std::vector<VkPresentModeKHR>& availablePresentModes )
{
	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapExtent( const VkSurfaceCapabilitiesKHR& capabilities )
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}
	else {
		int width, height;
		glfwGetFramebufferSize( g_theApp->m_window, &width, &height );

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width = std::clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
		actualExtent.height = std::clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

		return actualExtent;
	}
}

void Renderer::PopulateDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT& createInfo )
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

VkResult Renderer::CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger )
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" );
	if (func != nullptr) {
		return func( instance, pCreateInfo, pAllocator, pDebugMessenger );
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void Renderer::DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator )
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" );
	if (func != nullptr) {
		func( instance, debugMessenger, pAllocator );
	}
}

bool Renderer::IsDeviceSuitable( VkPhysicalDevice device )
{
	QueueFamilyIndices indices = FindQueueFamilies( device );

	bool extensionsSupported = CheckDeviceExtensionSupport( device );

	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport( device );
		swapChainAdequate = !swapChainSupport.m_formats.empty() && !swapChainSupport.m_presentModes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures( device, &supportedFeatures );

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

bool Renderer::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties( &layerCount, nullptr );

	std::vector<VkLayerProperties> availableLayers( layerCount );
	vkEnumerateInstanceLayerProperties( &layerCount, availableLayers.data() );

	for (const char* layerName : validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp( layerName, layerProperties.layerName ) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

VkFormat Renderer::FindSupportedFormat( const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features )
{
	for (VkFormat format : candidates) {
		VkFormatProperties props;
		vkGetPhysicalDeviceFormatProperties( m_physicalDevice, format, &props );
		if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
			return format;
		}
		else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
			return format;
		}
	}
	throw std::runtime_error( "failed to find supported format!" );
}

VkFormat Renderer::FindDepthFormat()
{
	return FindSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

bool Renderer::HasStencilComponent( VkFormat format )
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

uint32_t Renderer::FindMemoryType( uint32_t typeFilter, VkMemoryPropertyFlags properties )
{
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &memProperties );
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error( "failed to find suitable memory type!" );
}

std::vector<const char*> Renderer::GetRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions( &glfwExtensionCount );

	std::vector<const char*> extensions( glfwExtensions, glfwExtensions + glfwExtensionCount );

	if (enableValidationLayers) {
		extensions.push_back( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	}

	return extensions;
}

bool Renderer::CheckDeviceExtensionSupport( VkPhysicalDevice device )
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, nullptr );

	std::vector<VkExtensionProperties> availableExtensions( extensionCount );
	vkEnumerateDeviceExtensionProperties( device, nullptr, &extensionCount, availableExtensions.data() );

	std::set<std::string> requiredExtensions( deviceExtensions.begin(), deviceExtensions.end() );

	for (const auto& extension : availableExtensions) {
		requiredExtensions.erase( extension.extensionName );
	}

	return requiredExtensions.empty();
}

VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData )
{
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	
	return VK_FALSE;
}