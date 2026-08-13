#include <wayland-client.h>
#include <vulkan/vulkan.h> 
#include <vulkan/vulkan_wayland.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>
#include <cstdio>

#include <map>
#include <set>
#include <optional>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const char* platformName(int p) {
    switch (p) {
        case GLFW_PLATFORM_WIN32:   return "Win32";
        case GLFW_PLATFORM_COCOA:   return "Cocoa";
        case GLFW_PLATFORM_WAYLAND: return "Wayland";
        case GLFW_PLATFORM_X11:     return "X11";
        case GLFW_PLATFORM_NULL:    return "Null";
        default:                    return "unknown";
    }
}

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:
    GLFWwindow* window;
    VkDevice device;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstanceCreateInfo createInfo{};
    uint32_t i = 0;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;

    struct QueueFamilyIndices {
        std::optional<uint32_t> graphicsFamily;
    };

    void initWindow() {
        wl_display* display = wl_display_connect(nullptr);

        glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);

        if (!glfwInit()) {
            std::cout << "glfwInit FAILED" << std::endl;
        }
        
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        
        int platform = glfwGetPlatform();
        std::cout << "GLFW using: " << platformName(platform) << std::endl;
        
        int monitorCount;     
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);     
        std::cout << "Monitors found: " << monitorCount << std::endl;
        for (int i = 0; i < monitorCount; i++) {         
            const GLFWvidmode* mode = glfwGetVideoMode(monitors[i]);         
            std::cout << "  Monitor "<< i << ": " << mode->width << "x" << mode->height << std::endl;     
        }

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);

        if (!window) {
            std::cout << "glfwCreateWindow FAILED" << std::endl;
            glfwTerminate();
        }

        int w,h; glfwGetWindowSize(window,&w,&h); 
        std::cout << "size: " << w << "x" << h << " visible: " << glfwGetWindowAttrib(window, GLFW_VISIBLE) << std::endl;
    }

    void initVulkan() {   
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
    }

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;

        createInfo.enabledLayerCount = 0;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            std::cout << "failed to create instance!" << std::endl;
        }

        std::cout << "1. createInstance: " << "enable" << std::endl;
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;
        std::cout << "2. setupDebugMessenger: " << "disable" << std::endl;
    }

    void createSurface() {
        std::cout << "3. createSurface: in-process-coding-please-wait" << std::endl;
        // VkWin32SurfaceCreateInfoKHR createInfo{};
        // createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
        // createInfo.hwnd = glfwGetWin32Window(window);
        // createInfo.hinstance = GetModuleHandle(nullptr);

        // if (vkCreateWin32SurfaceKHR(instance, &createInfo, nullptr, &surface) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to create window surface!");
        // }
        // if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
        //     throw std::runtime_error("failed to create window surface!");
        // }
    }

    void pickPhysicalDevice() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

        if (deviceCount == 0) throw std::runtime_error("failed to find GPUs with Vulkan support!");

        std::vector<VkPhysicalDevice> devices(deviceCount);

        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        VkPhysicalDeviceProperties deviceProperties;
        for (const auto& device : devices)
            if (isDeviceSuitable(device)) {
                vkGetPhysicalDeviceProperties(device, &deviceProperties);

                if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
                    VkPhysicalDevice selectDevice = device;
                    physicalDevice = device;
                    break;
                }
            }

        std::cout << "4. pickPhysicalDevice: " << deviceProperties.deviceName << std::endl;

        if (physicalDevice == VK_NULL_HANDLE) throw std::runtime_error("failed to find a suitable GPU!");
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);

        VkDeviceQueueCreateInfo queueCreateInfo{};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = indices.graphicsFamily.value();
        queueCreateInfo.queueCount = 1;
        
        std::cout << "5. createLogicalDevice: only QueueFamilyIndices is run" << std::endl;
        // VkPhysicalDeviceFeatures deviceFeatures{};
        // VkDeviceCreateInfo createInfo{};
        // createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        return indices.graphicsFamily.has_value();
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        return true;
    }

    void mainLoop() {
        std::cout << "Window OK, wait 5 seconds: ";
        
        double elapse = glfwGetTime() + 5.0;
        while (glfwGetTime() < elapse) {
            glfwPollEvents();
            if (glfwWindowShouldClose(window)) break;
        }
        
        std::cout << "Done" << std::endl;
        



        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
        
        while (i != 200'000'000) {
            i++;
        }
        
        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();

        auto elapsed = duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Run a task: " << elapsed.count() / 1000 << "." << elapsed.count() % 1000 << "s (elapsed)" << std::endl;




        std::cout << "Main Loop: run" << std::endl;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        QueueFamilyIndices indices;
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

        int i = 0;
        for (const auto& queueFamily : queueFamilies) {
            if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                indices.graphicsFamily = i;
            }

            i++;
        }

        return indices;
    }
};

int main() {
    HelloTriangleApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}