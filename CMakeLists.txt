cmake_minimum_required(VERSION 3.24.2)
project(WackyEngine VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(Vulkan REQUIRED)
find_package(glfw3 3.3 REQUIRED)

set(SRC_FILES
    src/Main.cpp
    src/Application.cpp

    src/Core/Window.cpp
    src/Core/Debugger.cpp
    src/Core/Device.cpp
    src/Core/Context.cpp
    src/Core/Buffer.cpp

    src/Graphics/RenderSystem.cpp
    src/Graphics/SwapChain.cpp
    src/Graphics/Pipeline.cpp
    src/Graphics/RenderPass.cpp
    src/Graphics/DescriptorUtil.cpp
    src/Graphics/GraphicsBuffers.cpp
    src/Graphics/Model.cpp
    src/Graphics/Texture.cpp
    
    src/Graphics/Renderers/Renderer2D.cpp

    src/Math/Calculator.cpp
    src/Math/Vector2.cpp
    src/Math/Vector3.cpp
    src/Math/Vector4.cpp
    src/Math/Matrix4.cpp
    src/Math/Rectangle.cpp
)

add_executable(WackyEngine ${SRC_FILES})
target_include_directories(WackyEngine PRIVATE include "${GLFW_INCLUDE_DIRS}" "${Vulkan_INCLUDE_DIRS}")
target_link_libraries(WackyEngine PRIVATE "${Vulkan_LIBRARIES}" glfw)