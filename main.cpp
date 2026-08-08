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
    int i = 0;

    void initWindow() {
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan window", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();

        //// main code
        //glm::mat4 matrix;
        //glm::vec4 vec;
        //auto test = matrix * vec;
        //// main code
        //printf("Hello, World!");
    }

    int mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            std::cout << std::chrono::system_clock::now() << std::endl;
            while (i != 2'000'000'000) {
                i++;
            }
            std::cout << std::chrono::system_clock::now() << std::endl;

            return EXIT_SUCCESS;
        }
    }

    void cleanup() {
        glfwDestroyWindow(window);

        glfwTerminate();
    }

    void createInstance() {
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;

        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
            std::cout << "Unable to create VKInstance" << std::endl;
            return;
        }

        // получаем устройство
        VkPhysicalDevice physicalDevice = selectDevice(&instance);

        // проверяем, что устройство успешно получено
        if (physicalDevice == VK_NULL_HANDLE) {
            std::cout << "Failed to find a suitable GPU" << std::endl;
        }
        else {
            // если устройство найдено, выводим его название
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
            std::cout << "Selected device: " << deviceProperties.deviceName << std::endl;
        }

        vkDestroyInstance(instance, nullptr);
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

    // функция выбора устройства
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