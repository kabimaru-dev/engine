#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <cstdlib>

const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

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
    VkInstance instance;
    VkInstanceCreateInfo createInfo{};
    uint32_t i = 0;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
    }

    void displayDevices(VkInstance* instance) {

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);

        if (!deviceCount) return;
        std::vector<VkPhysicalDevice> devices(deviceCount);

        vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;  // для получения поддерживаемых функциональностей

        for (const auto& device : devices) {
            vkGetPhysicalDeviceProperties(device, &deviceProperties);

            // для каждого устройства получаем поддерживаемую функциональность
            vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

            std::cout << deviceProperties.deviceName << std::endl;
            // проверяем поддержку геометрических шейдеров
            std::cout << "Support for geometry shader: " << (deviceFeatures.geometryShader ? "Yes" : "No") << std::endl;
        }
    }

    VkPhysicalDevice selectDevice(VkInstance* instance) {

        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(*instance, &deviceCount, nullptr);
        // Если устройств с поддержкой Vulkan 0, то нет смысла что-то еще делать
        if (!deviceCount) {
            std::cout << "No available devices" << std::endl;
            return VK_NULL_HANDLE;
        }
        // определяем вектор для хранения всех дескрипторов VkPhysicalDevice.
        std::vector<VkPhysicalDevice> devices(deviceCount);
        // получаем устройства
        vkEnumeratePhysicalDevices(*instance, &deviceCount, devices.data());

        VkPhysicalDeviceProperties deviceProperties;
        // перебираем полученные устройства
        for (const auto& device : devices) {
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            // определяем дискретную видеокарту
            if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                return device;
        }
        return VK_NULL_HANDLE;
    }

    void initVulkan() {
        createInstance();
        setupDebugMessenger();
    }

    void setupDebugMessenger() {
        if (!enableValidationLayers) return;
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
            throw std::runtime_error("failed to create instance!");
        }
    }

    void mainLoop() {
        std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();

        while (i != 2'250'000'000) {
            i++;
        }

        std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();

        auto elapsed = duration_cast<std::chrono::milliseconds>(end - start);

        std::cout << "Elapsed Time: " << elapsed.count() / 1000 << "." << elapsed.count() % 1000 << "s";

        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);

        glfwTerminate();
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