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

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

class HelloTriangleApplication {
public:
    void run() {
        extensionsInfo();

        initWindow();
        initVulkan();
        mainLoop();
        cleanup();

        daemonDebbuger();
    }

private:
    GLFWwindow* window;
    VkDevice device;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;
    uint32_t i = 0;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkQueue presentQueue;

    std::vector<bool> isDone;
    std::vector<std::string> nameFunction;

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

        isDone.push_back(true); nameFunction.push_back("initWindow");
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();
        createSwapChain();
    }
    
    void extensionsInfo() {
        uint32_t extension{};
        vkEnumerateInstanceExtensionProperties(nullptr, &extension, nullptr);
        std::cout << "0. extensionsInfo: " << std::endl <<
            "  Count: " << extension << std::endl <<
            "    Names: " << std::endl;
        std::vector<VkExtensionProperties> extensions(extension);
        vkEnumerateInstanceExtensionProperties(nullptr, &extension, extensions.data());
        for (const auto& count : extensions) {
            std::cout << "      " << count.extensionName << std::endl;
        }
        
        isDone.push_back(true); nameFunction.push_back("extensionsInfo");
    }

    void createInstance() {
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        VkInstanceCreateInfo instanceInfo{};
        instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instanceInfo.pApplicationInfo = &appInfo;
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        instanceInfo.enabledExtensionCount = glfwExtensionCount;
        instanceInfo.ppEnabledExtensionNames = glfwExtensions;
        instanceInfo.enabledLayerCount = 0;
        if (vkCreateInstance(&instanceInfo, nullptr, &instance) != VK_SUCCESS) {
            std::cout << "failed to create instance!" << std::endl;
        }

        std::cout << "1. createInstance: " << "enable" << std::endl;
        isDone.push_back(true); nameFunction.push_back("createInstance");
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;
        std::cout << "2. setupDebugMessenger: " << "custom" << std::endl;
        isDone.push_back(true); nameFunction.push_back("setupDebugMessenger");
    }

    void createSurface() {
        std::cout << "3. createSurface: ";
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }
        std::cout << "ok" << std::endl;
        isDone.push_back(true); nameFunction.push_back("createSurface");
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
        isDone.push_back(true); nameFunction.push_back("pickPhysicalDevice");
    }

    void createLogicalDevice() {
        QueueFamilyIndices indices = findQueueFamilies(physicalDevice);


        std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
        std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

        float queuePriority = 1.0f;
        for (uint32_t queueFamily : uniqueQueueFamilies) {
            VkDeviceQueueCreateInfo queueCreateInfo{};
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = queueFamily;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &queuePriority;
            queueCreateInfos.push_back(queueCreateInfo);
            
            VkDeviceCreateInfo createInfo{};

            if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS) {
                throw std::runtime_error("failed to create logical device!");
            }
            
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            createInfo.pQueueCreateInfos = queueCreateInfos.data();
            
            // Swap Chain
            const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME }; 
            createInfo.enabledExtensionCount = 1; 
            createInfo.ppEnabledExtensionNames = deviceExtensions;
            // Swap Chain

            vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);  
        }

        std::cout << "5. createLogicalDevice: unknown" << std::endl;
        isDone.push_back(true); nameFunction.push_back("createLogicalDevice");
    }

    void createSwapChain() {
        // SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physicalDevice);

        // VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        // VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        // VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

        std::cout << "6. createSwapChain: undefined" << std::endl;
        isDone.push_back(true); nameFunction.push_back("createSwapChain");
    }

    bool isDeviceSuitable(VkPhysicalDevice device) {
        QueueFamilyIndices indices = findQueueFamilies(device);

        bool extensionsSupported = checkDeviceExtensionSupport(device);

        isDone.push_back(true); nameFunction.push_back("isDeviceSuitable");
        return indices.isComplete() && extensionsSupported;
    }

    bool checkDeviceExtensionSupport(VkPhysicalDevice device) {
        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

        std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

        for (const auto& extension : availableExtensions) {
            requiredExtensions.erase(extension.extensionName);
        }

        isDone.push_back(true); nameFunction.push_back("checkDeviceExtensionSupport");
        return requiredExtensions.empty();
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

        std::cout << "Main Loop: skip" << std::endl;

        // while (!glfwWindowShouldClose(window)) {
        //     glfwPollEvents();
        // }

        isDone.push_back(true); nameFunction.push_back("mainLoop");
    }

    void cleanup() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
        isDone.push_back(true); nameFunction.push_back("cleanup");
    }

    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

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

        if (presentSupport) {
            indices.presentFamily = i;
        }
        
        isDone.push_back(true); nameFunction.push_back("findQueueFamilies");
        return indices;
    }

    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device) {
        SwapChainSupportDetails details;

        isDone.push_back(true); nameFunction.push_back("querySwapChainSupport");
        return details;
    }

    void daemonDebbuger() {
        std::cout << "Hi, I'am master daemon debbuger, result:" << std::endl;
        for(int i = 0; i < isDone.size(); i++) {
            std::cout << "index(" << i << ")" << "bool(" << isDone[i] << ")" << "Function Name: " << nameFunction[i] << std::endl; 
        }
        std::cout << "Everything is runnable fine, but run doesn't mean you hasn't bugs!!!" << std::endl;
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