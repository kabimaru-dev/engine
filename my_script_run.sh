clang++ main.cpp -o main -std=c++20 \
-I/nix/store/qi1iiwfg6xhxkkcflw3b11zwj55kdkcj-vulkan-headers-1.4.341.0/include \
/nix/store/dszk3hvfj3qkbb1kd2907m3blg5iz587-vulkan-loader-1.4.341.0/lib/libvulkan.so \
-I/home/Gabimaru/engine/glfw-3.5.1/include \
/nix/store/47695rdjvj7k1n5ikmsslvj4na523scz-glfw-3.4/lib/libglfw.so \
-I/nix/store/v14b8w50ascyiq4l4mvqm5nhnc26azrg-glm-1.0.2/include \
-I/home/Gabimaru/mesa/include && ./main