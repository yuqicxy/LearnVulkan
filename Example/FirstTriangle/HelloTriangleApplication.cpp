#include <iostream>
#include <map>
#include <set>
#include <array>
#include <limits>
#include <algorithm>
#include "HelloTriangleApplication.h"
#include "ReadFile.h"
#include "glm/glm.hpp"

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

#ifdef NODEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

struct Vertex
{
	glm::vec2 pos;
	glm::vec3 color;

	/************************************************************************/
	/*		Binding descriptions
	/************************************************************************/
	static VkVertexInputBindingDescription getBindingDescription()
	{
		VkVertexInputBindingDescription bindingDescription = {};
		//The binding parameter specifies 
		//the index of the binding in the array of bindings
		bindingDescription.binding = 0;
		//The stride parameter specifies the number of bytes 
		//from one entry to the next
		bindingDescription.stride = sizeof(Vertex);
		//the inputRate parameter:
		//	VK_VERTEX_INPUT_RATE_VERTEX: Move to the next data entry 
		//		after each vertex
		//	VK_VERTEX_INPUT_RATE_INSTANCE : Move to the next data entry 
		//		after each instance
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	/************************************************************************/
	/*		Attribute descriptions
	/************************************************************************/
	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions()
	{
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		//An attribute description struct describes 
		//	how to extract a vertex attribute 
		//	from a chunk of vertex data 
		//	originating from a binding description
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(Vertex, color);
		
		//The format parameter describes
		//		the type of data for the attribute. 
		//
		//	float	: VK_FORMAT_R32_SFLOAT
		//	vec2	: VK_FORMAT_R32G32_SFLOAT
		//	vec3	: VK_FORMAT_R32G32B32_SFLOAT
		//	vec4	: VK_FORMAT_R32G32B32A32_SFLOAT
		//	ivec2	: VK_FORMAT_R32G32_SINT, a 2-component vector of 32-bit signed integers
		//	uvec4	: VK_FORMAT_R32G32B32A32_UINT, a 4-component vector of 32-bit unsigned integers
		//	double	: VK_FORMAT_R64_SFLOAT, a double-precision(64-bit) float
		//
		// the offset parameter specifies 
		//		the number of bytes 
		//		since the start of the per-vertex data to read from
		//
		//The binding is loading one Vertex at a time 
		//		and the position attribute(pos) 
		//		is at an offset of 0 bytes 
		//		from the beginning of this struct.
		//
		return attributeDescriptions;
	}
};

const std::vector<Vertex> vertices = {
	{{ 0.0f,-0.5f},{1.0f,0.0f,0.0f}},
	{{ 0.5f, 0.5f},{1.0f,1.0f,0.0f}},
	{{-0.5f, 0.5f},{1.0f,1.0f,1.0f}}
};

#undef max
#undef min
//@parameter messageSeverity
//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: Diagnostic message
//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: Informational message like the creation of a resource
//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: Message about behavior that is not necessarily an error, 
//			but very likely a bug in your application
//	VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: Message about behavior that is invalid and may cause crashes

//@parameter messageType
//	VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT: Some event has happened that is unrelated to the specification or performance
//	VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT: Something has happened that violates the specification or indicates a possible mistake
//	VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT: Potential non - optimal use of Vulkan

//@parameter pCallbackData 
//refers to a VkDebugUtilsMessengerCallbackDataEXT struct \
//containing the details of the message itself, \
//with the most important members being:
//	pMessage: The debug message as a null - terminated string
//	pObjects: Array of Vulkan object handles related to the message
//	objectCount: Number of objects in array

//@parameter pUserData 
//	contains a pointer that was specified 
//	during the setup of the callback 
//	and allows you to pass your own data to it

//@return Value
//indicates if the Vulkan call that 
//triggered the validation layer message should be aborted.

//If the callback returns true, 
//then the call is aborted with 
//the VK_ERROR_VALIDATION_FAILED_EXT error

//This is normally only used to test 
//the validation layers themselves, 
//so you should always return VK_FALSE.

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData) 
{
	std::cout << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
}



static void framebufferResizeCallback(GLFWwindow* window, int width, int height) 
{
	auto app = reinterpret_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
	app->mFramebufferResized = true;
}

void HelloTriangleApplication::run()
{
	initWindow();
	initVulkan();
	mainLoop();
	cleanUp();
}

void HelloTriangleApplication::initWindow()
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
	
	mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);

	glfwSetWindowUserPointer(mWindow, this);

	//Now to actually detect resizes 
	//		we can use the glfwSetFramebufferSizeCallback function 
	//		in the GLFW framework to set up a callback:
	glfwSetFramebufferSizeCallback(mWindow, framebufferResizeCallback);
}

void HelloTriangleApplication::initVulkan()
{
	createInstance();
	setupDebugCallback();
	CreateSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createFrameBuffers();
	createCommandPool();
	createVertexBuffer();
	createCommandBuffer();
	//createSemaphore();
	createSyncObjects();
}

void HelloTriangleApplication::createInstance()
{
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	//uint32_t glfwExtensionCount = 0;
	//const char** glfwExtensions;
	//glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	auto instanceExtensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();
	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensions) 
	{
		std::cout << "\t" << extension.extensionName << std::endl;
	}

	//Common operation in Validation Layer
	//Checking the values of parameters against the specification to detect misuse
	//Tracking creation and destruction of objects to find resource leaks
	//Checking thread safety by tracking the threads that calls originate from
	//Logging every call and its parameters to the standard output
	//Tracing Vulkan calls for profiling and replaying
	if (enableValidationLayers && !checkValidationLayerSupport())
	{
		throw std::runtime_error("validation layers requested,but not available!");
	}
}

void HelloTriangleApplication::CreateSurface()
{
	//The window surface needs to be created 
	//right after the instance creation, 
	//because it can actually influence 
	//the physical device selection.
	VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
	surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	surfaceCreateInfo.hwnd = glfwGetWin32Window(mWindow);
	surfaceCreateInfo.hinstance = GetModuleHandle(nullptr);

	auto CreateWin32SurfaceKHR = (PFN_vkCreateWin32SurfaceKHR)vkGetInstanceProcAddr(mInstance, "vkCreateWin32SurfaceKHR");
	if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(mInstance, &surfaceCreateInfo, nullptr, &mSurface) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create window surface!");
	}
}


void HelloTriangleApplication::mainLoop()
{
	while(!glfwWindowShouldClose(mWindow))
	{
		glfwPollEvents();
		//drawFrame();
		DrawFrame();
	}
	
	//Remember that all of the operations in drawFrame are asynchronous.
	//That means that when we exit the loop in mainLoop, 
	//drawing and presentation operations may still be going on.
	//Cleaning up resources while that is happening is a bad idea.
	vkDeviceWaitIdle(mDevice);
}

void HelloTriangleApplication::cleanupSwapChain()
{
	for (size_t i = 0; i < mSwapChainFrameBuffers.size(); ++i)
	{
		vkDestroyFramebuffer(mDevice, mSwapChainFrameBuffers[i], nullptr);
	}

	vkFreeCommandBuffers(mDevice,
		mCommandPool,
		static_cast<uint32_t>(mCommandBuffers.size()),
		mCommandBuffers.data());

	vkDestroyPipeline(mDevice, mGraphicsPipeline, nullptr);

	vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);

	vkDestroyRenderPass(mDevice, mRenderPass, nullptr);

	for (size_t i = 0;i < mSwapChainImageViews.size(); ++i)
	{
		vkDestroyImageView(mDevice, mSwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(mDevice, mSwapChain, nullptr);
}

void HelloTriangleApplication::cleanUp()
{
	cleanupSwapChain();
	

	vkDestroyBuffer(mDevice, mVertexBuffer, nullptr);
	//Memory that is bound to a buffer object 
	//	may be freed once the buffer is no longer used, 
	//so let's free it after the buffer has been destroyed:
	vkFreeMemory(mDevice, mVertexBufferMemory, nullptr);


	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		vkDestroySemaphore(mDevice, mImageAvailableSemaphores[i], nullptr);
		vkDestroySemaphore(mDevice, mRenderFinishedSemaphores[i], nullptr);
		vkDestroyFence(mDevice, mInFlightFences[i], nullptr);
	}

	vkDestroyCommandPool(mDevice, mCommandPool, nullptr);

	vkDestroyDevice(mDevice, nullptr);

	if (enableValidationLayers) 
	{
		DestroyDebugUtilsMessengerEXT(mInstance,mCallback, nullptr);
	}

	vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
	vkDestroyInstance(mInstance, nullptr);

	glfwDestroyWindow(mWindow);
	glfwTerminate();
}

bool HelloTriangleApplication::checkValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers)
	{
		bool layerFound = false;

		for (const auto &layerProperties : availableLayers)
		{
			if(strcmp(layerName,layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if(!layerFound)
		{
			return false;
		}
	}

	return true;
}

std::vector<const char*> HelloTriangleApplication::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char **glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidationLayers)
	{
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

void HelloTriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers)
		return;
	VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT \
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT\
		| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT \
		| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT \
		| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = debugCallback;
	createInfo.pUserData = nullptr; // Optional

	if (CreateDebugUtilsMessengerEXT(mInstance, &createInfo, nullptr, &mCallback) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to set up debug callback!");
	}
	std::cout << "create Debug Util" << std::endl;
}

VkResult HelloTriangleApplication::CreateDebugUtilsMessengerEXT(VkInstance instance
	, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
	, const VkAllocationCallbacks* pAllocator
	, VkDebugUtilsMessengerEXT *pCallback)
{
	auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance\
		, "vkCreateDebugUtilsMessengerEXT");
	if (func != nullptr)
	{
		return func(instance, pCreateInfo,\
			pAllocator, pCallback);
	}
	else
	{
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void HelloTriangleApplication::DestroyDebugUtilsMessengerEXT(VkInstance instance
	, VkDebugUtilsMessengerEXT callback
	, const VkAllocationCallbacks* pAllocator)
{
	auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
	if (func != nullptr) {
		func(instance, callback, pAllocator);
		std::cout << "Destroy Debug Util" << std::endl;
	}
}

void HelloTriangleApplication::pickPhysicalDevice()
{
	mPhysicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, nullptr);
	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan Support");
	}

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());

	for (const auto &device : devices)
	{
		if (isDeviceSuitable(device))
		{
			mPhysicalDevice = device;
			break;
		}
	}

	if(mPhysicalDevice == VK_NULL_HANDLE)
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

bool HelloTriangleApplication::isDeviceSuitable(const VkPhysicalDevice &device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	bool extensionSupported = checkDeviceExtensionSupport(device);

	bool swapChainAdequate = false;
	if (extensionSupported)
	{
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
	}
	return deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
		&& deviceFeatures.geometryShader
		&& findQueueFamilies(device).isComplete()
		&& extensionSupported
		&& swapChainAdequate;
}

bool HelloTriangleApplication::checkDeviceExtensionSupport(const VkPhysicalDevice &device)
{
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> availableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

	std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

	for (const auto &extension : availableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

void HelloTriangleApplication::pickPhysicalDevicebyScore()
{
	mPhysicalDevice = VK_NULL_HANDLE;
	uint32_t deviceCount;
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, NULL);
	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(mInstance, &deviceCount, devices.data());
	
	std::multimap<int, VkPhysicalDevice> candidates;
	for (const auto &device : devices)
	{
		int score = rateDeviceSuitability(device);
		candidates.insert(std::make_pair(score, device));
	}

	if (candidates.rbegin()->first > 0)
	{
		mPhysicalDevice = candidates.rbegin()->second;
	}
	else
	{
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

int HelloTriangleApplication::rateDeviceSuitability(const VkPhysicalDevice &device)
{
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(mPhysicalDevice, &deviceProperties);
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(mPhysicalDevice, &deviceFeatures);

	int score = 0;
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		score += 1000;

	//Maximum possible size of textures affects graphic quality
	score += deviceProperties.limits.maxImageDimension2D;

	//application can't function without geometry shader
	if (!deviceFeatures.geometryShader)
		return 0;
	
	if (findQueueFamilies(device).isComplete())
		score += 1000;

	return score;
}

HelloTriangleApplication::QueueFamily HelloTriangleApplication::findQueueFamilies(VkPhysicalDevice device)
{
	QueueFamily indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
	
	int i = 0;
	for (const auto &familyPropery : queueFamilies)
	{
		if (familyPropery.queueCount > 0 && familyPropery.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;
		}

		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, mSurface, &presentSupport);

		if (familyPropery.queueCount > 0 && presentSupport)
		{
			indices.presentFamily = i;
		}
		if (indices.isComplete())
		{
			break;
		}
		i++;
	}
	return indices;
}

void HelloTriangleApplication::createLogicalDevice()
{
	//Create Logical Device
	QueueFamily queueFam = findQueueFamilies(mPhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

	std::set<uint32_t> uniqueQueueFamilies = { queueFam.graphicsFamily.value()
		,queueFam.presentFamily.value() };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	//Specify Device Feature
	VkPhysicalDeviceFeatures deviceFeatures = {};
	
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();
	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	if (enableValidationLayers)
	{
		deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
		deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		deviceCreateInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(mDevice, queueFam.graphicsFamily.value(), 0, &mGraphicsQueue);
	vkGetDeviceQueue(mDevice, queueFam.presentFamily.value(), 0, &mPresentQueue);
}

HelloTriangleApplication::SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(VkPhysicalDevice device)
{
	SwapChainSupportDetails details;

	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, mSurface, &details.capabilities);

	/*querying the supported surface formats*/
	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, nullptr);
	if (formatCount != 0)
	{
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, mSurface, &formatCount, details.formats.data());
	}
	
	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &presentModeCount, nullptr);
	if (presentModeCount != 0)
	{
		details.presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, mSurface, &formatCount, details.presentModes.data());
	}

	return details;
}

VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats)
{
	/*VkSurfaceFormatKHR entry contains 
	a format and a colorSpace member.*/
	/*format member specifies the color channels 
	and types*/
	/*The colorSpace member indicates 
	if the SRGB color space is supported 
	or not using the VK_COLOR_SPACE_SRGB_NONLINEAR_KHR flag.*/
	if (availableFormats.size() == 1
		&& availableFormats[0].format == VK_FORMAT_UNDEFINED)
	{
		return	{ VK_FORMAT_B8G8R8A8_UNORM,
		VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			return availableFormat;
	}
	return availableFormats[0];
}

VkPresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;
	for (const auto &availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			return availablePresentMode;
		else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
			bestMode =  availablePresentMode;
	}
	return bestMode;
}

VkExtent2D HelloTriangleApplication::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites)
{
	if (capabilites.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilites.currentExtent;
	}
	else
	{
		//To handle window resizes properly, 
		//we also need to query 
		//the current size of the framebuffer 
		//to make sure that 
		//the swap chain images have the(new) right size.
		int width, height;
		glfwGetFramebufferSize(mWindow, &width, &height);

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		actualExtent.width  = std::max(capabilites.minImageExtent.width  ,std::min(capabilites.maxImageExtent.width, actualExtent.width));
		actualExtent.height = std::max(capabilites.minImageExtent.height,std::min(capabilites.maxImageExtent.height, actualExtent.height));
		return actualExtent;
	}

}

void HelloTriangleApplication::createSwapChain()
{
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(mPhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
	VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);
	
	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = mSurface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamily indices = findQueueFamilies(mPhysicalDevice);
	uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; // Optional
		createInfo.pQueueFamilyIndices = nullptr; // Optional
	}
	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;
	createInfo.oldSwapchain = VK_NULL_HANDLE;
	if (vkCreateSwapchainKHR(mDevice, &createInfo, nullptr, &mSwapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain!");
	}

	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, nullptr);
	mSwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(mDevice, mSwapChain, &imageCount, mSwapChainImages.data());

	mSwapChainFormat = surfaceFormat.format;
	mSwapChainExtent = extent;
}

void HelloTriangleApplication::createImageViews()
{
	mSwapChainImageViews.resize(mSwapChainImages.size());
	for (size_t i = 0;i < mSwapChainImages.size(); ++i)
	{
		VkImageViewCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = mSwapChainImages[i];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = mSwapChainFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;
		if (vkCreateImageView(mDevice, &createInfo, nullptr, &mSwapChainImageViews[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to create image views!");
		}
	}
}

void HelloTriangleApplication::createGraphicsPipeline()
{
	std::vector<char> vertShaderCode = readFile("Shaders/vert.spv");
	std::vector<char> fragShaderCode = readFile("Shaders/frag.spv");
	
	VkShaderModule vertShaderModule = createShaderModule(vertShaderCode);
	VkShaderModule fragShaderModule = createShaderModule(fragShaderCode);

	VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
	vertShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertShaderStageInfo.stage	= VK_SHADER_STAGE_VERTEX_BIT;
	vertShaderStageInfo.module	= vertShaderModule;
	vertShaderStageInfo.pName	= "main";

	VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
	fragShaderStageInfo.sType	= VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragShaderStageInfo.stage	= VK_SHADER_STAGE_FRAGMENT_BIT;
	fragShaderStageInfo.module	= fragShaderModule;
	fragShaderStageInfo.pName	= "main";

	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo,fragShaderStageInfo };

	//*************************************
	//		Pipeline Vertex Input
	//*************************************
	//
	//Bindings: spacing between data and 
	//			whether the data is per-vertex or per-instance(see instancing)
	//Attribute descriptions : 
	//			type of the attributes passed to the vertex shader, 
	//			which binding to load them from 
	//			and at which offset
	//
	auto bindingDescription = Vertex::getBindingDescription();
	auto attributeDescriptions = Vertex::getAttributeDescriptions();

	//The pipeline is now ready to accept vertex data 
	//		in the format of the vertices container 
	//		and pass it on to our vertex shader. 
	//

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType							= VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount	= 1;
	vertexInputInfo.pVertexBindingDescriptions		= &bindingDescription;
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputInfo.pVertexAttributeDescriptions	= attributeDescriptions.data();
	

	/************************************************************************/
	/*		Input Assembly                                                                      */
	/************************************************************************/
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType						= VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	//VK_PRIMITIVE_TOPOLOGY_POINT_LIST: points from vertices
	//VK_PRIMITIVE_TOPOLOGY_LINE_LIST : line from every 2 vertices without reuse
	//VK_PRIMITIVE_TOPOLOGY_LINE_STRIP : the end vertex of every line is used as start vertex for the next line
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST : triangle from every 3 vertices without reuse
	//VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP : the second and third vertex of every triangle are used as first two vertices of the next triangle
	inputAssembly.topology					= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

	//If you set the primitiveRestartEnable member to VK_TRUE, 
	//then it's possible to break up lines 
	//and triangles in the _STRIP topology modes 
	//by using a special index of 0xFFFF or 0xFFFFFFFF.
	inputAssembly.primitiveRestartEnable	= VK_FALSE;
	

	/************************************************************************/
	/*		Viewports and Scissors                                                                      */
	/************************************************************************/
	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)mSwapChainExtent.width;
	viewport.height = (float)mSwapChainExtent.height;
	/*
	 *	The minDepth and maxDepth values specify 
	 *	the range of depth values to use for the framebuffer
	 */
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	//While viewports define the transformation from the image to the framebuffer,
	//scissor rectangles define in which regions pixels will actually be stored.
	VkRect2D scissor = {};
	scissor.offset = { 0,0 };
	scissor.extent = mSwapChainExtent;

	//It is possible to use multiple viewports 
	//and scissor rectangles on some graphics cards, 
	//so its members reference an array of them.
	//Using multiple requires enabling a GPU feature(see logical device creation).
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	/************************************************************************/
	/*		Rasterizer                                                                      */
	/************************************************************************/
	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	//If depthClampEnable is set to VK_TRUE, then fragments that are beyond the near and far planes are clamped to them as opposed to discarding them. This is useful in some special cases like shadow maps. Using this requires enabling a GPU feature.
	rasterizer.depthClampEnable = VK_FALSE;
	//If rasterizerDiscardEnable is set to VK_TRUE, then geometry never passes through the rasterizer stage. This basically disables any output to the framebuffer.
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	//The polygonMode determines how fragments are generated for geometry.The following modes are available :
	//			VK_POLYGON_MODE_FILL	: fill the area of the polygon with fragments
	//			VK_POLYGON_MODE_LINE	: polygon edges are drawn as lines
	//			VK_POLYGON_MODE_POINT : polygon vertices are drawn as points
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	//The lineWidth member is straightforward, 
	//it describes the thickness of lines 
	//in terms of number of fragments.
	//The maximum line width that is supported depends on the hardware and any line thicker than 1.0f requires you to enable the wideLines GPU feature.
	rasterizer.lineWidth = 4.0f;
	//The cullMode variable determines the type of face culling to use.You can disable culling, cull the front faces, cull the back faces or both.
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	//The frontFace variable specifies the vertex order for faces to be considered front-facing and can be clockwise or counterclockwise.
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	//The rasterizer can alter the depth values by adding a constant value or biasing them based on a fragment's slope
	rasterizer.depthBiasEnable			= VK_FALSE;
	rasterizer.depthBiasConstantFactor	= 0.0f;//optional
	rasterizer.depthBiasClamp			= 0.0f;//optional
	rasterizer.depthBiasSlopeFactor		= 0.0f;//optional

	/************************************************************************/
	/*		MultiSampling                                                                     */
	/************************************************************************/
	//The VkPipelineMultisampleStateCreateInfo struct configures multisampling, 
	//which is one of the ways to perform anti-aliasing. 
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType					= VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable	= VK_FALSE;
	multisampling.rasterizationSamples	= VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading		= 1.0f;//optional
	multisampling.pSampleMask			= nullptr;//optional
	multisampling.alphaToCoverageEnable = VK_FALSE;//optional
	multisampling.alphaToOneEnable		= VK_FALSE;//optional

	/************************************************************************/
	/*		Depth and stencil testing
	/************************************************************************/
	//empty

	/************************************************************************/
	/*		Color Blending                                                                      */
	/************************************************************************/
	//After a fragment shader has returned a color, 
	//it needs to be combined with the color 
	//that is already in the framebuffer. 
	//This transformation is known as color blending and there are two ways to do it:
	//	1.Mix the old and new value to produce a final color
	//	2.Combine the old and new value using a bitwise operation
	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask			= VK_COLOR_COMPONENT_R_BIT |
		VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
		VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable			= VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstColorBlendFactor	= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.colorBlendOp			= VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor	= VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor	= VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp			= VK_BLEND_OP_ADD;

	//color blending follow pseudo-code:
	//	if (blendEnable)
	//	{
	//		finalColor.rgb = (srcColorBlendFactor * newColor.rgb) < colorBlendOp > (dstColorBlendFactor * oldColor.rgb);
	//		finalColor.a = (srcAlphaBlendFactor * newColor.a) < alphaBlendOp > (dstAlphaBlendFactor * oldColor.a);
	//	}
	//	else
	//	{
	//		finalColor = newColor;
	//	}
	//
	//	finalColor = finalColor & colorWriteMask;
	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	//If you want to use the second method of blending(bitwise combination), 
	//	then you should set logicOpEnable to VK_TRUE.
	//
	//The bitwise operation can then be specified 
	//	in the logicOp field
	//
	//Note that this will automatically disable the first method, 
	//	as if you had set blendEnable to VK_FALSE 
	//	for every attached framebuffer!
	//
	//The colorWriteMask will also be used in this mode 
	//	to determine which channels in the framebuffer will actually be affected. 
	colorBlending.logicOpEnable		= VK_FALSE;
	colorBlending.logicOp			= VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount	= 1;
	colorBlending.pAttachments		= &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f; //optional
	colorBlending.blendConstants[1] = 0.0f; //optional
	colorBlending.blendConstants[2] = 0.0f; //optional
	colorBlending.blendConstants[3] = 0.0f; //optional
	
	/************************************************************************/
	/*		Dynamic State                                                                     */
	/************************************************************************/
	VkDynamicState dynamicStates[] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineDynamicStateCreateInfo dynamicState = {};
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.dynamicStateCount = 2;
	dynamicState.pDynamicStates = dynamicStates;

	/************************************************************************/
	/*		Pipeline Layout                                                                     */
	/************************************************************************/
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 0;	//optional
	pipelineLayoutInfo.pSetLayouts = nullptr; //optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; //optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(mDevice, &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
 	pipelineInfo.pDepthStencilState = nullptr;//optional
	pipelineInfo.pColorBlendState = &colorBlending;//optional
	pipelineInfo.pDynamicState = nullptr;//optional
	pipelineInfo.layout = mPipelineLayout;
	pipelineInfo.renderPass = mRenderPass;
	pipelineInfo.subpass = 0;
	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;	//optional
	pipelineInfo.basePipelineIndex = -1;				//optional

	//The vkCreateGraphicsPipelines function 
	//		actually has more parameters than 
	//		the usual object creation functions in Vulkan

	//It is designed to take multiple VkGraphicsPipelineCreateInfo objects 
	//		and create multiple VkPipeline objects 
	//		in a single call.
	
	//The second parameter, for which 
	//		we've passed the VK_NULL_HANDLE argument, 
	//		references an optional VkPipelineCache object. 
	//A pipeline cache can be used to store and reuse data 
	//		relevant to pipeline creation across multiple calls to vkCreateGraphicsPipelines 
	//		and even across program executions 
	//		if the cache is stored to a file.
	// This makes it possible to significantly 
	//		speed up pipeline creation at a later time.
	if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mGraphicsPipeline) != VK_SUCCESS )
	{
		throw std::runtime_error("Failed to create graphics pipeline!");
	}

	vkDestroyShaderModule(mDevice, vertShaderModule, nullptr);
	vkDestroyShaderModule(mDevice, fragShaderModule, nullptr);
}

VkShaderModule HelloTriangleApplication::createShaderModule(const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo{};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
	VkShaderModule shaderModule;
	if (vkCreateShaderModule(mDevice, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create shader module!");
	}
	return shaderModule;
}

/************************************************************************/
/*  specify how many color and depth buffers there will be, 
/*	how many samples to use for each of them 
/*	and how their contents should be handled throughout the rendering operations                                                                     */
/************************************************************************/
void HelloTriangleApplication::createRenderPass()
{
	VkAttachmentDescription colorAttachment = {};
	colorAttachment.format = mSwapChainFormat;
	colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; //1 sample

	/************************************************************************/
	/*The loadOp and storeOp determine what to do with the data in the attachment before rendering and after rendering.

	/* LOAD_OP:
	/*		VK_ATTACHMENT_LOAD_OP_LOAD: Preserve the existing contents of the attachment
	/*		VK_ATTACHMENT_LOAD_OP_CLEAR: Clear the values to a constant at the start
	/*		VK_ATTACHMENT_LOAD_OP_DONT_CARE: Existing contents are undefined; we don't care about them
	/*
	/* STORE_OP:
	/*
	/*
	/************************************************************************/
	colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

	//storeOp
	//******************************************
	//VK_ATTACHMENT_STORE_OP_STORE: 
	//			Rendered contents will be stored 
	//			in memory and can be read later
	//VK_ATTACHMENT_STORE_OP_DONT_CARE : 
	//			Contents of the framebuffer 
	//			will be undefined 
	//			after the rendering operation
	//******************************************
	colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

	/************************************************************************/
	/*	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: 
	/*		Images used as color attachment
	/*	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: 
	/*		Images to be presented in the swap chain
	/*	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL: 
	/*		Images to be used as destination 
	/*		for a memory copy operation
	/************************************************************************/
	colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	//initialLayout specifies which layout the image will have before the render pass begins.
	//finalLayout specifies the layout to automatically transition to when the render pass finishes

	//A single render pass can consist of multiple subpasses
	//Subpasses are subsequent rendering operations 
	//that depend on the contents of framebuffers in previous passes,

	/************************************************************************/
	/*	VkAttchmentReference                                                                     */
	/************************************************************************/
	VkAttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	//The attachment parameter specifies 
	//		which attachment to reference 
	//		by its index in the attachment descriptions array.

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	//The index of the attachment in this array 
	//is directly referenced 
	//from the fragment shader 
	//with the layout(location = 0) out vec4 outColor 
	//directive!
	subpass.pColorAttachments = &colorAttachmentRef;

	//pInputAttachments			: Attachments that are read from a shader
	//pResolveAttachments		: Attachments used 
	//			for multisampling color attachments
	//pDepthStencilAttachment	: Attachments for depth and stencil data
	//pPreserveAttachments		: Attachments that are not used 
	//			by this subpass, 
	//			but for which the data must be preserved
	

	/************************************************************************/
	/*		Subpass dependencies
	/************************************************************************/
	//Remember that the subpasses in a render pass automatically take care of image layout transitions.
	//These transitions are controlled by subpass dependencies, 
	//		which specify memory and execution dependencies between subpasses.

	//There are two built-in dependencies that take care of 
	//		the transition at the start of the render pass and at the end of the render pass, 
	//		but the former does not occur at the right time.
	//It assumes that the transition occurs at the start of the pipeline, but we haven't acquired the image yet at that point!
	//There are two ways to deal with this problem:
	//		1.We could change the waitStages for the imageAvailableSemaphore to VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT to ensure that the render passes don't begin until the image is available, 
	//		2. or we can make the render pass wait for the VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT stage

	//The first two fields specify the indices of the dependency and the dependent subpass.
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
	dependency.dstSubpass = 0;

	//The special value VK_SUBPASS_EXTERNAL refers to the implicit subpass before or after the render pass depending on 
	//				whether it is specified in srcSubpass or dstSubpass.
	//The index 0 refers to our subpass, which is the first and only one.
	//The dstSubpass must always be higher than srcSubpass to prevent cycles in the dependency graph.
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	//The next two fields specify the operations to wait on and the stages in which these operations occur.
	//We need to wait for the swap chain to finish reading from the image before we can access it.
	//This can be accomplished by waiting on the color attachment output stage itself.
	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	//Render Pass
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = 1;
	renderPassInfo.pAttachments = &colorAttachment;
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	//The operations that should wait on this are in the color attachment stage and involve the reading and writing of the color attachment.
	//These settings will prevent the transition from happening until it's actually necessary (and allowed): 
	//				when we want to start writing colors to it.
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &mRenderPass) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create render pass!");
	}
}

void HelloTriangleApplication::createFrameBuffers()
{
	mSwapChainFrameBuffers.resize(mSwapChainImages.size());
	for (size_t i = 0;i < mSwapChainImageViews.size();++i)
	{
		VkImageView attachments[] = { mSwapChainImageViews[i] };
		VkFramebufferCreateInfo frameBufferInfo = {};
		frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		frameBufferInfo.renderPass = mRenderPass;
		frameBufferInfo.attachmentCount = 1;
		frameBufferInfo.pAttachments = attachments;
		frameBufferInfo.width = mSwapChainExtent.width;
		frameBufferInfo.height = mSwapChainExtent.height;
		frameBufferInfo.layers = 1;

		if (vkCreateFramebuffer(mDevice, &frameBufferInfo, nullptr, &mSwapChainFrameBuffers[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create Framebuffer");
		}
	}
}

void HelloTriangleApplication::createCommandPool()
{
	QueueFamily queueFamilyIndice = findQueueFamilies(mPhysicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndice.graphicsFamily.value();

	//Command buffers are executed 
	//by submitting them on one of the device queues

	//Each command pool can only allocate command buffers 
	//		that are submitted on a single type of queue.

	//Two flags for command pools:
	//		VK_COMMAND_POOL_CREATE_TRANSIENT_BIT: Hint that command buffers are rerecorded 
	//				with new commands very often
	//				(may change memory allocation behavior)
	//		VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : Allow command buffers to be rerecorded individually, 
	//				without this flag they all have to be reset together

	if (vkCreateCommandPool(mDevice, &poolInfo, nullptr, &mCommandPool) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create command pool!");
	}
}

void HelloTriangleApplication::createVertexBuffer()
{
	VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory	stagingBufferMemory;
	//stagingBuffer with stagingBufferMemory for mapping and copying the vertex data.
	//VK_BUFFER_USAGE_TRANSFER_SRC_BIT: 
	//		Buffer can be used as source in a memory transfer operation.
	//VK_BUFFER_USAGE_TRANSFER_DST_BIT:
	//		Buffer can be used as destination in a memory transfer operation.
	//
	//The vertexBuffer is now allocated from a memory type that is device local, which generally means that we're not able to use vkMapMemory. 
	//
	//However, we can copy data from the stagingBuffer to the vertexBuffer. 
	//We have to indicate that we intend to do that 
	//	by specifying the transfer source flag for the stagingBuffer 
	//	and the transfer destination flag for the vertexBuffer, 
	//	along with the vertex buffer usage flag.
	//
	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
		, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT 
		| VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		, stagingBuffer, stagingBufferMemory);

	void *data;
	vkMapMemory(mDevice, stagingBufferMemory, 0, bufferSize, 0, &data);
	//memcpy the vertex data to the mapped memory 
	//	and unmap it again using vkUnmapMemory
	memcpy(data, vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(mDevice, stagingBufferMemory);
	//Unfortunately the driver may not immediately 
	//	copy the data into the buffer memory, 
	//	for example because of caching.
	//It is also possible that 
	//	writes to the buffer 
	//	are not visible in the mapped memory yet.
	//There are two ways to deal with that problem :
	//	Use a memory heap that is host coherent, 
	//		indicated with VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
	//	Call vkFlushMappedMemoryRanges to 
	//		after writing to the mapped memory,
	//		and call vkInvalidateMappedMemoryRanges 
	//		before reading from the mapped memory

	createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, mVertexBuffer, mVertexBufferMemory);
	copyBuffer(stagingBuffer, mVertexBuffer, bufferSize);
	
	//After copying the data from the staging buffer to the device buffer, 
	//		we should clean it up :
	vkDestroyBuffer(mDevice, stagingBuffer, nullptr);
	vkFreeMemory(mDevice, stagingBufferMemory, nullptr);
}

uint32_t HelloTriangleApplication::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
{
	VkPhysicalDeviceMemoryProperties memProperties;
	//First we need to query info 
	//	about the available types of memory 
	//	using vkGetPhysicalDeviceMemoryProperties.
	vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &memProperties);
	//The VkPhysicalDeviceMemoryProperties structure 
	//	has two arrays memoryTypes and memoryHeaps.
	//
	//Memory heaps are distinct memory resources like dedicated VRAM 
	//	and swap space in RAM for when VRAM runs out.
	//The different types of memory exist within these heaps.
	//Right now we'll only concern ourselves with the type of memory 
	//	and not the heap it comes from, 
	//	but you can imagine that this can affect performance
	//
	//**********************************************
	//**********************************************
	//first find a memory type
	//that is suitable for the buffer itself:
	for (uint32_t i = 0; i < memProperties.memoryTypeCount; ++i)
	{
		if (typeFilter & (1 << i)
			&& (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	//The typeFilter parameter will be used 
	//	to specify the bit field of memory types 
	//	that are suitable.
	//That means that we can find 
	//	the index of a suitable memory type 
	//	by simply iterating over them 
	//	and checking if 
	//	the corresponding bit is set to 1.
	throw std::runtime_error("Failed to find suitable memory type!");
}

void HelloTriangleApplication::createCommandBuffer()
{
	mCommandBuffers.resize(mSwapChainFrameBuffers.size());

	// VkCommandBufferAllocateInfo specifies the command pool 
	//		and number of buffers to allocate:
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = mCommandPool;
	//The level parameter specifies if the allocated command buffers are primary or secondary command buffers.
	//	VK_COMMAND_BUFFER_LEVEL_PRIMARY: Can be submitted to a queue for execution, 
	//			but cannot be called from other command buffers.
	//	VK_COMMAND_BUFFER_LEVEL_SECONDARY : Cannot be submitted directly,
	//			but can be called from primary command buffers.
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = (uint32_t)mCommandBuffers.size();

	if (vkAllocateCommandBuffers(mDevice, &allocInfo, mCommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	for (size_t i = 0; i < mCommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		
		//The flags parameter specifies how we're going to use the command buffer. 
		//The following values are available:
		//		VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT:
		//				The command buffer will be rerecorded right 
		//				after executing it once.
		//		VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT : 
		//				This is a secondary command buffer that 
		//				will be entirely within a single render pass.
		//		VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT : 
		//				The command buffer can be resubmitted 
		//				while it is also already pending execution.
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

		if (vkBeginCommandBuffer(mCommandBuffers[i], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		//The first parameters are the render pass itself 
		//	and the attachments to bind.
		renderPassInfo.renderPass = mRenderPass;
		renderPassInfo.framebuffer = mSwapChainFrameBuffers[i];
		//The next two parameters define 
		//		the size of the render area.
		//The render area defines where 
		//		shader loads and stores will take place. 
		//
		//The pixels outside this region will have undefined values.
		//It should match the size of the attachments 
		//		for best performance.
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = mSwapChainExtent;

		//The last two parameters define the clear values 
		//	to use for VK_ATTACHMENT_LOAD_OP_CLEAR, 
		//	which we used as load operation for the color attachment.
		VkClearValue clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(mCommandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
		
			//The render pass can now begin.
			//All of the functions that 
			//		record commands can be recognized 
			//		by their vkCmd prefix
			//
			//The first parameter for every command 
			//		is always the command buffer 
			//		to record the command to.
			//
			//The second parameter specifies 
			//		the details of the render pass 
			//		we've just provided.
			//
			//The final parameter controls 
			//		how the drawing commands 
			//		within the render pass will be provided.
			//
			//It can have one of two values :
			//		VK_SUBPASS_CONTENTS_INLINE: The render pass commands will be embedded
			//			in the primary command buffer itself 
			//			and no secondary command buffers will be executed.
			//		VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS: The render pass commands 
			//			will be executed from secondary command buffers.
			//		
			//************************************************************************/
			//*		Basic drawing commands
			//************************************************************************/
			// bind the graphics pipeline:
			//	The second parameter specifies 
			//	if the pipeline object is a graphics or compute pipeline. 
			vkCmdBindPipeline(mCommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, mGraphicsPipeline);

			VkBuffer vertexBuffers[] = { mVertexBuffer };
			VkDeviceSize offsets[] = { 0 };
			vkCmdBindVertexBuffers(mCommandBuffers[i], 0, 1, vertexBuffers, offsets);
			//vertexCount: Even though we don't have a vertex buffer, 
			//			we technically still have 3 vertices to draw.
			//instanceCount : Used for instanced rendering, 
			//			use 1 if you're not doing that.
			//firstVertex : Used as an offset into the vertex buffer,
			//			defines the lowest value of gl_VertexIndex.
			//firstInstance : Used as an offset for instanced rendering, 
			//			defines the lowest value of gl_InstanceIndex.
			vkCmdDraw(mCommandBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);

		vkCmdEndRenderPass(mCommandBuffers[i]);

		if (vkEndCommandBuffer(mCommandBuffers[i]) != VK_SUCCESS) 
		{
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

void HelloTriangleApplication::drawFrame()
{
	//Each of these events is set in motion using a single function call, 
	//	but they are executed asynchronously.
	//
	//The function calls will return before the operations are actually finished 
	//	and the order of execution is also undefined.
	//That is unfortunate, because each of the operations 
	//	depends on the previous one finishing.
	//
	//There are two ways of synchronizing swap chain events: 
	//	fences and semaphores.
	//
	//They're both objects that 
	//	can be used for coordinating operations 
	//	by having one operation signal 
	//	and another operation wait for a fence or semaphore 
	//	to go from the unsignaled to signaled state.
	//
	//The difference is that the state of fences 
	//	can be accessed from your program 
	//	using calls like vkWaitForFences 
	//	and semaphores cannot be.
	//
	//Fences are mainly designed to synchronize your application itself with rendering operation, 
	//whereas semaphores are used to synchronize operations within or across command queues.
	//
	/************************************************************************/
	/*		Acquiring an image from the swap chain
	/************************************************************************/
	uint32_t imageIndex;

	//The first two parameters of vkAcquireNextImageKHR are the logical device and the swap chain from which we wish to acquire an image.
	//The third parameter specifies a timeout in nanoseconds for an image to become available. 
	//				Using the maximum value of a 64 bit unsigned integer disables the timeout.
	//The next two parameters specify synchronization objects that are to be signaled when the presentation engine is finished using the image.
	//				That's the point in time where we can start drawing to it.
	//The last parameter specifies a variable to output the index of the swap chain image that has become available. 
	//				The index refers to the VkImage in our swapChainImages array. 
	//				We're going to use that index to pick the right command buffer.
	vkAcquireNextImageKHR(mDevice, mSwapChain, 
		std::numeric_limits<uint64_t>::max(), 
		mImageAvailableSemaphore, 
		VK_NULL_HANDLE, 
		&imageIndex);
	
	/************************************************************************/
	/*		Submitting the command buffer
	/************************************************************************/
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphore[] = {mImageAvailableSemaphore};
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	//The first three parameters specify which semaphores to wait on before execution begins and in which stage(s) of the pipeline to wait.
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphore;
	submitInfo.pWaitDstStageMask = waitStages;
	//We want to wait with writing colors to the image until it's available, 
	//so we're specifying the stage of the graphics pipeline that writes to the color attachment.
	//That means that theoretically the implementation can already start executing our vertex shader and such while the image is not yet available
	//Each entry in the waitStages array corresponds to the semaphore with the same index in pWaitSemaphores.
	
	//The next two parameters specify which command buffers to actually submit for execution.
	//we should submit the command buffer that binds the swap chain image we just acquired as color attachment.
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphore };
	//The signalSemaphoreCount and pSignalSemaphores parameters specify which semaphores to signal once the command buffer(s) have finished execution.
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to submit draw command buffer");
	}
	
	/************************************************************************/
	/*		Presentation
	/************************************************************************/
	//The last step of drawing a frame is submitting the result back to the swap chain to have it eventually show up on the screen.
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	//The first two parameters specify which semaphores to wait on before presentation can happen, just like VkSubmitInfo.
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	
	VkSwapchainKHR swapChains[] = { mSwapChain };
	presentInfo.swapchainCount = 1;
	//The next two parameters specify the swap chains to present images to and the index of the image for each swap chain.
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;
	//pResults:It allows you to specify an array of VkResult values to check for every individual swap chain if presentation was successful
	presentInfo.pResults = nullptr; // Optional
	
	//The vkQueuePresentKHR function 
	//	submits the request to present an image to the swap chain.
	vkQueuePresentKHR(mPresentQueue, &presentInfo);

	//We'll add error handling for both vkAcquireNextImageKHR 
	//and vkQueuePresentKHR in the next chapter, 
	//because their failure does not necessarily mean that 
	//the program should terminate, 
	//unlike the functions we've seen so far.


	/************************************************************************/
	/*	Frames in flight
	/************************************************************************/


}

void HelloTriangleApplication::DrawFrame()
{
	//To perform CPU-GPU synchronization, 
	//
	//Vulkan offers a second type of synchronization primitive called fences.
	//Fences are similar to semaphores in the sense that they can be signaled and waited for, 
	//but this time we actually wait for them in our own code.

	//The vkWaitForFences function takes an array of fences 
	//		and waits for either any or all of them to be signaled before returning.
	//The VK_TRUE we pass here indicates that 
	//		we want to wait for all fences, 
	//		but in the case of a single one 
	//		it obviously doesn't matter.
	vkWaitForFences(mDevice, 1, &mInFlightFences[mCurrentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	
	//Unlike the semaphores, 
	//we manually need to restore the fence to the unsignaled state 
	//by resetting it with the vkResetFences call.
	//vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mDevice, 
		mSwapChain, 
		std::numeric_limits<uint64_t>::max(), 
		mImageAvailableSemaphores[mCurrentFrame],
		VK_NULL_HANDLE, 
		&imageIndex);

	//Now we just need to figure out 
	//		when swap chain recreation is necessary 
	//		and call our new recreateSwapChain function. 
	//
	//The swap chain is no longer adequate during presentation. 
	//The vkAcquireNextImageKHR and vkQueuePresentKHR functions 
	//		can return the following special values to indicate this.
	//
	//VK_ERROR_OUT_OF_DATE_KHR: 
	//		The swap chain has become incompatible 
	//		with the surface and can no longer be used for rendering.
	//		Usually happens after a window resize.
	//VK_SUBOPTIMAL_KHR:
	//		The swap chain can still be used to successfully present to the surface, 
	//		but the surface properties are no longer matched exactly.
	//
	if (result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		//If the swap chain turns out to be out of date 
		//		when attempting to acquire an image, 
		//		then it is no longer possible to present to it. 
		//Therefore we should immediately recreate the swap chain 
		//		and try again in the next drawFrame call.
		recreateSwapChain();
		return;
	}
	else if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}


	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
	VkSemaphore waitSemaphores[] = { mImageAvailableSemaphores[mCurrentFrame] };
	VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mCommandBuffers[imageIndex];

	VkSemaphore signalSemaphores[] = { mRenderFinishedSemaphores[mCurrentFrame] };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;


	//if we abort drawing at this point 
	//	then the fence will be never have been submitted with vkQueueSubmit 
	//	and it'll be in an unexpected state 
	//	when we try to wait for it later on.
	//We could recreate the fences as part of swap chain recreation, 
	//	but it's easier to move the vkResetFences call:
	vkResetFences(mDevice, 1, &mInFlightFences[mCurrentFrame]);

	if(vkQueueSubmit(mGraphicsQueue,1,&submitInfo,mInFlightFences[mCurrentFrame]) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit draw command buffer!");
	}

	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { mSwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	
	presentInfo.pImageIndices = &imageIndex;

	result = vkQueuePresentKHR(mPresentQueue, &presentInfo);
	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || mFramebufferResized)
	{
		mFramebufferResized = false;
		recreateSwapChain();
	}
	else if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to present swap chain image");
	}

	mCurrentFrame = (mCurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void HelloTriangleApplication::createSyncObjects()
{
	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mInFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	VkFenceCreateInfo fenceInfo = {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	
	//The problem here is that, by default, 
	//		fences are created in the unsignaled state.
	//That means that vkWaitForFences will wait forever 
	//		if we haven't used the fence before.
	//change the fence creation to initialize it in the signaled state 
	//		as if we had rendered an initial frame that finished:
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0;i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphores[i]) != VK_SUCCESS ||
			vkCreateFence(mDevice, &fenceInfo, nullptr, &mInFlightFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create synchronization objects for a frame!");
		}
	}
}

void HelloTriangleApplication::createSemaphore()
{
	//VkSemaphoreCreateInfo semaphoreInfo = {};
	//semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	//
	//if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphore) != VK_SUCCESS ||
	//	vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mRenderFinishedSemaphore) != VK_SUCCESS)
	//{
	//	throw std::runtime_error("Failed to create semaphores!");
	//}

	mImageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
	mRenderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
	for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
	{
		if (vkCreateSemaphore(mDevice, &semaphoreInfo, nullptr, &mImageAvailableSemaphores[i]) != VK_SUCCESS
			|| vkCreateSemaphore(mDevice,&semaphoreInfo,nullptr,&mRenderFinishedSemaphores[i]) != VK_SUCCESS )
		{
			throw std::runtime_error("Failed to create semaphore for a frame!");
		}
	}

}

void HelloTriangleApplication::recreateSwapChain()
{
	//There is another case 
	//	where a swap chain may become out of data 
	//	and that is a special kind of window resizing:
	//
	//
	int width = 0, height = 0;
	while (width == 0 || height == 0)
	{
		glfwGetFramebufferSize(mWindow, &width, &height);
		glfwWaitEvents();
	}

	//We first call vkDeviceWaitIdle, 
	//because just like in the last chapter, we shouldn't touch resources that may still be in use
	vkDeviceWaitIdle(mDevice);
	
	cleanupSwapChain();

	//recreate the swap chain itself.
	createSwapChain();
	
	//The image views need to be recreated 
	//because they are based directly on the swap chain images.
	createImageViews();
	
	//The render pass needs to be recreated 
	//because it depends on the format of the swap chain images
	createRenderPass();

	//Viewport and scissor rectangle size is specified 
	//during graphics pipeline creation, 
	//so the pipeline also needs to be rebuilt.
	//It is possible to avoid this by using dynamic state 
	//for the viewports and scissor rectangles
	createGraphicsPipeline();

	//framebuffers and command buffers also directly 
	//depend on the swap chain images.
	createFrameBuffers();
	createCommandBuffer();
}


//It should be noted that in a real world application,
//		you're not supposed to actually call vkAllocateMemory for every individual buffer.
//The maximum number of simultaneous memory allocations is limited by the maxMemoryAllocationCount physical device limit,
//		which may be as low as 4096 even on high end hardware like an NVIDIA GTX 1080.
//The right way to allocate memory for a large number of objects at the same time
//		is to create a custom allocator that splits up a single allocation
//		among many different objects by using the offset parameters
//		that we've seen in many functions.
void HelloTriangleApplication::createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer &buffer, VkDeviceMemory &bufferMemory)
{
	//Creating a buffer requires us to fill a VkBufferCreateInfo structure
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//size specifies the size of the buffer in bytes. 
	bufferInfo.size = size;
	//usage indicates for which purposes the data in the buffer 
	//	is going to be used.
	//It is possible to specify multiple purposes 
	//	using a bitwise or.
	bufferInfo.usage = usage;
	//Just like the images in the swap chain, 
	//	buffers can also be owned by a specific queue family 
	//	or be shared between multiple at the same time.
	//The buffer will only be used from the graphics queue, 
	//	so we can stick to exclusive access.
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//The flags parameter is used to configure sparse buffer memory, 
	//which is not relevant right now.
	//We'll leave it at the default value of 0.
	bufferInfo.flags = 0;

	//create the buffer with vkCreateBuffer. 
	//Define a class member to hold the buffer handle 
	//	and call it vertexBuffer.
	if (vkCreateBuffer(mDevice, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create vertex buffer!");
	}

	/************************************************************************/
	/*		Memory requirements
	/************************************************************************/
	//The buffer has been created, 
	//but it doesn't actually have any memory assigned to it yet. 
	VkMemoryRequirements memRequirements;
	//query its memory requirements 
	vkGetBufferMemoryRequirements(mDevice, buffer, &memRequirements);

	//The VkMemoryRequirements struct has three fields :
	//	size: 
	//		The size of the required amount of memory in bytes, 
	//		may differ from bufferInfo.size.
	//	alignment: 
	//		The offset in bytes where the buffer begins 
	//		in the allocated region of memory, 
	//		depends on bufferInfo.usage 
	//		and bufferInfo.flags.
	//	memoryTypeBits:
	//		Bit field of the memory types
	//		that are suitable for the buffer.


	VkMemoryAllocateInfo alloInfo = {};
	alloInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	alloInfo.allocationSize = memRequirements.size;
	alloInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

	if (vkAllocateMemory(mDevice, &alloInfo, nullptr, &bufferMemory) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to allocate vertex buffer memory£¡");
	}

	//If memory allocation was successful, then we can now associate this memory with the buffer using vkBindBufferMemory:
	//Since this memory is allocated specifically 
	//	for this the vertex buffer, 
	//the offset is simply 0. 
	//If the offset is non - zero, 
	//	then it is required to be divisible 
	//	by memRequirements.alignment.
	vkBindBufferMemory(mDevice, buffer, bufferMemory, 0);
}

void HelloTriangleApplication::copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size)
{
	//Memory transfer operations are executed using command buffers, 
	//just like drawing commands.
	//Therefore we must first allocate a temporary command buffer.
	//You may wish to create a separate command pool for these kinds of short-lived buffers, 
	//because the implementation may be able to 
	//apply memory allocation optimizations.
	//You should use the VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag 
	//during command pool generation in that case.

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = mCommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(mDevice, &allocInfo, &commandBuffer);

	//And immediately start recording the command buffer:
	//The VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT flag that 
	//	we used for the drawing command buffers is not necessary here, 
	//because we're only going to use the command buffer once 
	//	and wait with returning from the function 
	//	until the copy operation has finished executing. 
	//It's good practice to tell the driver 
	//	about our intent using VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT.

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	
	VkBufferCopy copyRegion = {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	//Contents of buffers are transferred using the vkCmdCopyBuffer command.
	//It takes the source and destination buffers as arguments, 
	//	and an array of regions to copy.
	//The regions are defined in VkBufferCopy structs 
	//	and consist of a source buffer offset,
	//	destination buffer offset and size.
	//It is not possible to specify VK_WHOLE_SIZE here, 
	//	unlike the vkMapMemory command.
	vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);
	//This command buffer only contains the copy command, so we can stop recording right after that.
	vkEndCommandBuffer(commandBuffer);
	
	//Now execute the command buffer to complete the transfer :
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(mGraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	
	//Unlike the draw commands, 
	//there are no events we need to wait on this time. 
	//We just want to execute the transfer on the buffers immediately. 
	//There are again two possible ways to wait on this transfer to complete. 
	//We could use a fence and wait with vkWaitForFences, 
	//		or simply wait for the transfer queue to become idle with vkQueueWaitIdle. 
	//A fence would allow you to schedule multiple transfers simultaneously 
	//		and wait for all of them complete, 
	///		instead of executing one at a time. 
	//That may give the driver more opportunities to optimize.
	vkQueueWaitIdle(mGraphicsQueue);

	//Don't forget to clean up the command buffer used for the transfer operation.
	vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);


}