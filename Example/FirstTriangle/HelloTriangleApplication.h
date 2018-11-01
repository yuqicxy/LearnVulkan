#pragma once

#define GLFW_INCLUDE_VULKAN

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>

class HelloTriangleApplication
{
public:
	void run();

private:
	void initWindow();
	
	void initVulkan();

	/*The instance is the connection between 
	your application and the Vulkan library and 
	creating it involves specifying some details 
	about your application to the driver.*/
	void createInstance();

	void mainLoop();

	void cleanUp();

	bool checkValidationLayerSupport();

	std::vector<const char*> getRequiredExtensions();

	void setupDebugCallback();
	
	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance
		, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo
		, const VkAllocationCallbacks* pAllocator
		, VkDebugUtilsMessengerEXT *pCallback);

	void DestroyDebugUtilsMessengerEXT(VkInstance instance
		,VkDebugUtilsMessengerEXT callback
		, const VkAllocationCallbacks* pAllocator);

private:
	GLFWwindow*							mWindow;
	VkInstance							mInstance;
	VkDebugUtilsMessengerEXT			mCallback;
};
