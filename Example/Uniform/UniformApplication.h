#pragma once

#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <vulkan/vulkan.h>

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include <vector>
#include <cstring>
#include <cstdlib>
#include <array>
#include <optional>
#include <set>

const int WIDTH = 800;
const int HEIGHT = 600;

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

const std::vector<const char*> deviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif



struct QueueFamilyIndices {
	std::optional<uint32_t> graphicsFamily;
	std::optional<uint32_t> presentFamily;

	bool isComplete() {
		return graphicsFamily.has_value() && presentFamily.has_value();
	}
};

struct SwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct Vertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(Vertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.5f , -0.5f },{ 0.0f, 1.0f, 0.0f } },
	{ { 0.5f ,  0.5f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.5f,  0.5f },{ 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};

class UniformApplication 
{
public:
	void run();

private:
	void initWindow();
		
	static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
	
	void initVulkan();
	
	void mainLoop();
	
	void cleanupSwapChain();
	
	void cleanup();
	
	void recreateSwapChain();
	
	void createInstance();
	
	void setupDebugCallback();
	
	void createSurface();
	
	void pickPhysicalDevice();

	void createLogicalDevice();
	
	void createImageViews();

	void createSwapChain();
	
	void createRenderPass();
	
	void createGraphicsPipeline();
	
	void createFramebuffers();
	
	void createCommandPool();
	
	void createVertexBuffer();
	
	void createIndexBuffer();
	
	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	
	void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
	
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
	void createCommandBuffers();
	
	void createSyncObjects();
	
	void drawFrame();
	
	VkShaderModule createShaderModule(const std::vector<char>& code);
	
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);
	
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
	
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
	
	bool isDeviceSuitable(VkPhysicalDevice device);
	
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	
	QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
	
	std::vector<const char*> getRequiredExtensions();
	
	bool checkValidationLayerSupport();
	
	static std::vector<char> readFile(const std::string& filename);
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

private:
	bool							framebufferResized = false;

	GLFWwindow						*window;

	VkInstance						instance;
	VkDebugUtilsMessengerEXT		callback;
	VkSurfaceKHR					surface;

	VkPhysicalDevice				physicalDevice = VK_NULL_HANDLE;
	VkDevice						device;

	VkQueue							graphicsQueue;
	VkQueue							presentQueue;

	VkSwapchainKHR					swapChain;
	std::vector<VkImage>			swapChainImages;
	VkFormat						swapChainImageFormat;
	VkExtent2D						swapChainExtent;
	std::vector<VkImageView>		swapChainImageViews;
	std::vector<VkFramebuffer>		swapChainFramebuffers;

	VkRenderPass					renderPass;
	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;

	VkCommandPool					commandPool;

	VkBuffer						vertexBuffer;
	VkDeviceMemory					vertexBufferMemory;
	VkBuffer						indexBuffer;
	VkDeviceMemory					indexBufferMemory;

	std::vector<VkCommandBuffer>	commandBuffers;

	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	size_t							currentFrame = 0;
};