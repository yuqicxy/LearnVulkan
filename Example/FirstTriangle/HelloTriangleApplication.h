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

private:
	GLFWwindow*		mWindow;
	VkInstance		mInstance;
};
