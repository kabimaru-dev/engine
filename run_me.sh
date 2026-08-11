sudo find / -wholename "*-vulkan-headers-*/include/vulkan/vulkan.h" 2>/dev/null
sudo find / -wholename "*-vulkan-loader-*/lib/libvulkan.so" 2>/dev/null

sudo find / -wholename "*/engine/glfw-3.5.1/include/GLFW/glfw3.h" 2>/dev/null
sudo find / -wholename "*/engine/glfw-3.5.1/include/GLFW/glfw3native.h" 2>/dev/null

sudo find / -wholename "*-glfw-*/lib/libglfw.so" 2>/dev/null

sudo find / -wholename "*-glm-*/include/glm/vec4.hpp" 2>/dev/null
sudo find / -wholename "*-glm-*/include/glm/mat4x4.hpp" 2>/dev/null

sudo find / -wholename "*/GL/gl.h" 2>/dev/null


echo ""
echo "write here your path to library, for example:"

echo ""
echo "clang++ main.cpp -o main -std=c++20 \\"
echo "-I.../vulkan-headers-1.4.341.0/include \\"
echo ".../vulkan-loader-1.4.341.0/lib/libvulkan.so \\"
echo "-I.../glfw/include \\"
echo ".../glfw/lib/libglfw.so \\"
echo "-I.../glm && ./main"
echo ""
echo "if you find your libraries path, then put it insted of this, and run it."