#pragma once

#define GLFW_INCLUDE_VULKAN
#define VK_USE_PLATFORM_WIN32_KHR
#define GLFW_EXPOSE_NATIVE_WIN32

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#include <functional>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <optional>

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

	void CreateSurface();

	void createLogicalDevice();

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

	void pickPhysicalDevice();

	bool isDeviceSuitable(const VkPhysicalDevice &device);

	void pickPhysicalDevicebyScore();

	int rateDeviceSuitability(const VkPhysicalDevice &device);

	bool checkDeviceExtensionSupport(const VkPhysicalDevice &device);

	struct QueueFamily
	{
		std::optional<uint32_t> graphicsFamily;
		std::optional<uint32_t> presentFamily;
		bool isComplete()
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	struct SwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR			capabilities;
		std::vector<VkSurfaceFormatKHR>		formats;
		std::vector<VkPresentModeKHR>		presentModes;
	};

	QueueFamily findQueueFamilies(VkPhysicalDevice device);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);

	/*
	There are three types of settings to determine :
	1.Surface format(color depth)
	2.Presentation mode(conditions for "swapping" images to the screen)
	3.Swap extent(resolution of images in swap chain)
	*/
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats);
	
	/*
	VK_PRESENT_MODE_IMMEDIATE_KHR: 
		Images submitted by your application are transferred to the screen right away, 
		which may result in tearing.
	VK_PRESENT_MODE_FIFO_KHR : 
		The swap chain is a queue 
		where the display takes an image from the front of the queue when the display is refreshed 
		and the program inserts rendered images at the back of the queue.
		If the queue is full then the program has to wait.
		This is most similar to vertical sync as found in modern games.
		The moment that the display is refreshed is known as "vertical blank".
	VK_PRESENT_MODE_FIFO_RELAXED_KHR : This mode only differs from the previous one 
		if the application is late and the queue was empty at the last vertical blank.
		Instead of waiting for the next vertical blank, 
		the image is transferred right away when it finally arrives.
		This may result in visible tearing.
	VK_PRESENT_MODE_MAILBOX_KHR : 
		This is another variation of the second mode.
		Instead of blocking the application when the queue is full, 
		the images that are already queued are simply replaced with the newer ones.
		This mode can be used to implement triple buffering, 
		which allows you to avoid tearing with significantly less latency issues 
		than standard vertical sync that uses double buffering.
	*/
	VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	/*The swap extent is the resolution of the swap chain images and it's almost always exactly equal to the resolution of the window that we're drawing to.The range of the possible resolutions is defined in the VkSurfaceCapabilitiesKHR structure.*/
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilites);
	
	void createSwapChain();

	void createImageViews();

	void createGraphicsPipeline();

	VkShaderModule createShaderModule(const std::vector<char>& code);
private:
	GLFWwindow*							mWindow;
	VkQueue								mGraphicsQueue;
	VkQueue								mPresentQueue;
	VkDevice							mDevice;
	VkInstance							mInstance;
	VkSurfaceKHR						mSurface;
	VkPhysicalDevice					mPhysicalDevice;
	VkDebugUtilsMessengerEXT			mCallback;
	VkSwapchainKHR						mSwapChain;
	std::vector<VkImage>				mSwapChainImages;
	VkFormat							mSwapChainFormat;
	VkExtent2D							mSwapChainExtent;
	std::vector<VkImageView>			mSwapChainImageViews;
	VkPipelineLayout					mPipelineLayout;
};
