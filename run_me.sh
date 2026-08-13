sudo find / -wholename "*-vulkan-headers-*/include/vulkan/vulkan.h" 2>/dev/null
sudo find / -wholename "*-vulkan-loader-*/lib/libvulkan.so" 2>/dev/null

sudo find / -wholename "*/engine/glfw-3.5.1/include/GLFW/glfw3.h" 2>/dev/null
sudo find / -wholename "*/engine/glfw-3.5.1/include/GLFW/glfw3native.h" 2>/dev/null

sudo find / -wholename "*-glfw-*/lib/libglfw.so" 2>/dev/null

sudo find / -wholename "*-glm-*/include/glm/vec4.hpp" 2>/dev/null
sudo find / -wholename "*-glm-*/include/glm/mat4x4.hpp" 2>/dev/null

sudo find / -wholename "*/GL/gl.h" 2>/dev/null

find / -name "wayland.xml" 2>/dev/null

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

echo "end. don't look here, it's just testing for future"

# 1st - step
sed -e 's/@WAYLAND_VERSION_MAJOR@/1/g' \
    -e 's/@WAYLAND_VERSION_MINOR@/22/g' \
    -e 's/@WAYLAND_VERSION_MICRO@/0/g' \
    -e 's/@WAYLAND_VERSION@/1.22.0/g' \
    wayland/src/wayland-version.h.in > wayland/src/wayland-version.h
# 2nd - step
wayland-scanner client-header /nix/store/qk3csrmy9gmzar57mgy5shkqifzndngr-qtbase-6.11.1/share/qt6/wayland/protocols/wayland/wayland.xml /home/Gabimaru/engine/wayland/src/wayland-client-protocol.h
# 3rd - step
wayland-scanner client-header /nix/store/qk3csrmy9gmzar57mgy5shkqifzndngr-qtbase-6.11.1/share/qt6/wayland/protocols/wayland/wayland.xml /home/Gabimaru/engine/wayland/src/wayland-client-core.h
# 4th - step
sudo find / -name wayland-client.h
# 5th - step
sudo find / -name "libwayland*" 2>/dev/null