#include "HelloTriangleApplication.h"
#include <iostream>
const int WIDTH = 800;
const int HEIGHT = 600;

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

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions;
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	createInfo.enabledExtensionCount = glfwExtensionCount;
	createInfo.ppEnabledExtensionNames = glfwExtensions;

	createInfo.enabledLayerCount = 0;

	if (vkCreateInstance(&createInfo, nullptr, &mInstance) != VK_SUCCESS) {
		throw std::runtime_error("failed to create instance!");
	}

	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());
	std::cout << "available extensions:" << std::endl;

	for (const auto& extension : extensions) {
		std::cout << "\t" << extension.extensionName << std::endl;
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
	vkDestroyInstance(mInstance, nullptr);
	glfwDestroyWindow(mWindow);
	glfwTerminate();
}