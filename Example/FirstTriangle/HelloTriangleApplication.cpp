#include "HelloTriangleApplication.h"
#include <iostream>
const int WIDTH = 800;
const int HEIGHT = 600;
const std::vector<const char*> validationLayers = \
{"VK_LAYER_LUNARG_standard_validation"};

#ifdef NODEBUG
	const bool enableValidationLayers = false;
#else
	const bool enableValidationLayers = true;
#endif

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
	std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
	return VK_FALSE;
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
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	mWindow = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", nullptr, nullptr);
}

void HelloTriangleApplication::initVulkan()
{
	createInstance();
	setupDebugCallback();
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

void HelloTriangleApplication::mainLoop()
{
	while(!glfwWindowShouldClose(mWindow))
	{
		glfwPollEvents();
	}
}

void HelloTriangleApplication::cleanUp()
{
	if (enableValidationLayers) 
	{
		DestroyDebugUtilsMessengerEXT(mInstance,mCallback, nullptr);
	}
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