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
    VkResult result;
    VkInstance instance;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
    VkSurfaceKHR surface;
    VkDebugUtilsMessengerEXT debugMessenger;
    uint32_t i = 0;
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    VkQueue presentQueue;

    VkSwapchainKHR     swapChain; 
    std::vector<VkImage>       swapChainImages; 
    std::vector<VkImageView>   swapChainImageViews; 
    VkFormat           swapChainImageFormat; 
    VkExtent2D         swapChainExtent;

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
        std::cout << "5. createLogicalDevice: ";

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
            
            const char* deviceExtensions[] = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

            VkDeviceCreateInfo createInfo{};

            createInfo.enabledExtensionCount = 1;
            createInfo.ppEnabledExtensionNames = deviceExtensions;

            result = vkCreateDevice(physicalDevice, &createInfo, nullptr, &device);

            if (result != VK_SUCCESS) {
                throw std::runtime_error("failed to create logical device!");
            } else {
                std::cout << "Success - result = " << result << std::endl;
            }
            
            createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
            createInfo.pQueueCreateInfos = queueCreateInfos.data();

            vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);  
        }
        
        isDone.push_back(true); nameFunction.push_back("createLogicalDevice");
    }

    void createSwapChain() {
        std::cout << "6. createSwapChain: try" << std::endl;
        PFN_vkCreateSwapchainKHR pfnCreate = (PFN_vkCreateSwapchainKHR)vkGetDeviceProcAddr(device, "vkCreateSwapchainKHR");     std::cout << "vkCreateSwapchainKHR address: " << (void*)pfnCreate << std::endl;         if (pfnCreate == nullptr) {         std::cout << "FATAL: VK_KHR_swapchain not loaded!" << std::endl;         return;     }

        std::cout << "physicalDevice: " << physicalDevice << std::endl;     std::cout << "device: " << device << std::endl;     std::cout << "surface: " << surface << std::endl;

        std::cout << "  Test, step 1" << std::endl;
        VkSurfaceCapabilitiesKHR caps; vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &caps); uint32_t fN = 0; vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fN, nullptr);
        std::vector<VkSurfaceFormatKHR> formats(fN); vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &fN, formats.data()); uint32_t pN = 0; vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &pN, nullptr); std::vector<VkPresentModeKHR> presentModes(pN);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &pN, presentModes.data());

        std::cout << "    Step 2" << std::endl; VkSurfaceFormatKHR fmt = formats[0]; for (const auto& f : formats) if (f.format == VK_FORMAT_B8G8R8A8_SRGB && f.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) fmt = f;

        std::cout << "    Step 3" << std::endl; VkPresentModeKHR pm = VK_PRESENT_MODE_FIFO_KHR; for (const auto& m : presentModes) if (m == VK_PRESENT_MODE_MAILBOX_KHR) { pm = m; break; }

        std::cout << "    Step 4" << std::endl; VkExtent2D ext = caps.currentExtent; if (ext.width == 0xFFFFFFFF) { ext.width  = std::clamp((uint32_t)WIDTH,  caps.minImageExtent.width,  caps.maxImageExtent.width); ext.height = std::clamp((uint32_t)HEIGHT, caps.minImageExtent.height, caps.maxImageExtent.height); }

        std::cout << "    Step 5" << std::endl; uint32_t imgCount = caps.minImageCount + 1; if (caps.maxImageCount > 0 && imgCount > caps.maxImageCount) imgCount = caps.maxImageCount;

        std::cout << "    Step 6" << std::endl; VkSwapchainCreateInfoKHR ci{}; ci.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR; ci.surface = surface; ci.minImageCount    = imgCount; ci.imageFormat      = fmt.format; ci.imageColorSpace  = fmt.colorSpace; ci.imageExtent      = ext; ci.imageArrayLayers = 1; ci.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; ci.preTransform     = caps.currentTransform; ci.compositeAlpha   = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; ci.presentMode      = pm; ci.clipped          = VK_TRUE; ci.oldSwapchain     = VK_NULL_HANDLE;

        daemonDebbugerSwapChain(ci);

        std::cout << "    Step 7" << std::endl; if (vkCreateSwapchainKHR(device, &ci, nullptr, &swapChain) != VK_SUCCESS) throw std::runtime_error("failed to create swap chain!");
        
        std::cout << "    Step 8" << std::endl; vkGetSwapchainImagesKHR(device, swapChain, &imgCount, nullptr); swapChainImages.resize(imgCount); vkGetSwapchainImagesKHR(device, swapChain, &imgCount, swapChainImages.data());

        std::cout << "    Step 9" << std::endl; swapChainImageViews.resize(swapChainImages.size()); for (size_t i = 0; i < swapChainImages.size(); i++) { VkImageViewCreateInfo vi{}; vi.sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO; vi.image    = swapChainImages[i]; vi.viewType = VK_IMAGE_VIEW_TYPE_2D; vi.format   = fmt.format; vi.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }; vkCreateImageView(device, &vi, nullptr, &swapChainImageViews[i]); }

        std::cout << "    Step 10" << std::endl; swapChainImageFormat = fmt.format; swapChainExtent      = ext;

        std::cout << "  createSwapChain: SUCCESS" << std::endl;
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

        std::cout << "Main Loop: run" << std::endl;

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            // drawFrame();
            if (glfwWindowShouldClose(window)) break;
        }

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
        std::cout << std::endl;
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
        std::cout << "Hi, I'am master daemon debbuger, result:" << std::endl;
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
        for(int i = 0; i < isDone.size(); i++) {
            std::cout << "    id: " << i << " " << nameFunction[i] << " " << isDone[i] << std::endl; 
        }
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
        std::cout << "Everything is runnable fine, but run doesn't mean you hasn't bugs!!!" << std::endl;
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
    }
    void daemonDebbugerSwapChain(VkSwapchainCreateInfoKHR ci) {
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
        std::cout << "ci.sType: " << ci.sType << " (expected " << VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR << ")" << std::endl;
        std::cout << "ci.surface: " << ci.surface << std::endl;
        std::cout << "ci.minImageCount: " << ci.minImageCount << std::endl;
        std::cout << "ci.imageFormat: " << ci.imageFormat << std::endl;
        std::cout << "ci.imageColorSpace: " << ci.imageColorSpace << std::endl;
        std::cout << "ci.imageExtent.width: " << ci.imageExtent.width << std::endl;
        std::cout << "ci.imageExtent.height: " << ci.imageExtent.height << std::endl;
        std::cout << "ci.imageArrayLayers: " << ci.imageArrayLayers << std::endl;
        std::cout << "ci.imageUsage: " << ci.imageUsage << std::endl;
        std::cout << "ci.preTransform: " << ci.preTransform << std::endl;
        std::cout << "ci.compositeAlpha: " << ci.compositeAlpha << std::endl;
        std::cout << "ci.presentMode: " << ci.presentMode << std::endl;
        std::cout << "ci.clipped: " << ci.clipped << std::endl;
        std::cout << "ci.oldSwapchain: " << ci.oldSwapchain << std::endl;
        std::cout << "swapChain (out param): " << &swapChain << std::endl;
        std::cout << "\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\" << std::endl;
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