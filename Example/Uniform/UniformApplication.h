#pragma once

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

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
#include <chrono>

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

//resource descriptors. 
//A descriptor is a way for shaders to 
//	freely access resources like buffers and images. 
//We're going to set up a buffer 
//	that contains the transformation matrices 
//	and have the vertex shader access them through a descriptor. 
//
//Usage of descriptors consists of three parts :
//	1.Specify a descriptor layout during pipeline creation
//	2.Allocate a descriptor set from a descriptor pool
//	3.Bind the descriptor set during rendering
//
//The descriptor layout specifies the types of resources that are going to be accessed by the pipeline,
//
//A descriptor set specifies the actual buffer or image resources that will be bound to the descriptors.
//The descriptor set is then bound for the drawing commands just like the vertex buffers and framebuffer.
struct UniformBufferObject
{
	glm::mat4 mModel;
	glm::mat4 mView;
	glm::mat4 mProj;
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
	
	void createDescriptorSetLayout();
	
	//create a descriptor set for each VkBuffer resource 
	//		to bind it to the uniform buffer descriptor.
	//
	//Descriptor sets can't be created directly, 
	//		they must be allocated from a pool like command buffers. 
	void createDescriptorPool();
	
	//A descriptor set allocation is described with a VkDescriptorSetAllocateInfo struct.
	//You need to specify the descriptor pool to allocate from, 
	//	the number of descriptor sets to allocate, 
	//	and the descriptor layout to base them on:
	//You need to specify the descriptor pool to allocate from, 
	//	the number of descriptor sets to allocate, 
	//	and the descriptor layout to base them on:
	void createDescriptorSets();

	void createUniformBuffers();

	void drawFrame();
	
	void updateUniformBuffer(uint32_t imageIndex);

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
	VkDescriptorSetLayout			mDescriptorSetLayout;
	VkDescriptorPool				mDescriptorPool;
	std::vector<VkDescriptorSet>	mDescriptorSets;

	VkPipelineLayout				pipelineLayout;
	VkPipeline						graphicsPipeline;

	VkCommandPool					commandPool;

	VkBuffer						vertexBuffer;
	VkDeviceMemory					vertexBufferMemory;
	VkBuffer						indexBuffer;
	VkDeviceMemory					indexBufferMemory;

	//Uniform buffer
	//We're going to copy new data to the uniform buffer every frame, 
	//	so it doesn't really make any sense to have a staging buffer.
	//
	//It would just add extra overhead in this case 
	//	and likely degrade performance instead of improving it.
	//
	//We should have multiple buffers, 
	//	because multiple frames may be in flight at the same time 
	//	and we don't want to update the buffer 
	//	in preparation of the next frame while a previous one is still reading from it! 
	std::vector<VkBuffer>			mUniformBuffers;
	std::vector<VkDeviceMemory>		mUniformBuffersMemory;

	std::vector<VkCommandBuffer>	commandBuffers;

	std::vector<VkSemaphore>		imageAvailableSemaphores;
	std::vector<VkSemaphore>		renderFinishedSemaphores;
	std::vector<VkFence>			inFlightFences;
	size_t							currentFrame = 0;
};