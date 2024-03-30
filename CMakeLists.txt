cmake_minimum_required(VERSION 3.12)
project(CW)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(assimp REQUIRED)

# find_library(SOIL2_LIB soil2 REQUIRED PATHS external/SOIL2/lib/macosx)
include_directories(external Engine)

# Add executable
add_executable(CW main.cpp
        Engine/utils/Shader.cpp
        Engine/graphics/Texture.cpp
        Engine/utils/Camera.cpp
        Engine/utils/Vertex.cpp
        Engine/graphics/Mesh.cpp
        Engine/graphics/Model.cpp
        Engine/helpers/AssimpGLMHelpers.cpp
        Engine/utils/Objects/Skybox.cpp
        Engine/utils/Objects/Sun.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imgui_impl_glfw.cpp
        external/imgui/imgui_impl_opengl3.cpp
        Engine/App.cpp
        Engine/View.cpp
        Engine/graphics/Color.cpp
        Engine/utils/Buffer.cpp
        Engine/utils/BoundingBox.cpp
        Engine/physics/ModelAttributes.cpp
        Engine/physics/Gravity.cpp
        Engine/physics/Collisions.cpp
        Engine/utils/PlayerManager.cpp
        Engine/utils/Objects/Player.cpp
        Engine/graphics/PostProcessor.cpp
        Engine/utils/ShaderManager.cpp
        Engine/graphics/DepthBuffer.cpp
        Engine/graphics/ShadowBuffer.cpp
        Engine/graphics/Renderable.cpp
        Engine/graphics/FrameBuffer.cpp
        Engine/physics/Spline.cpp
        Engine/Entity.cpp
        Engine/Entity.h
        Engine/ImmovableObject.cpp
        Engine/ImmovableObject.h
)

# Link libraries
target_link_libraries(CW PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm assimp::assimp) #${SOIL2_LIB})

# Set C++ standard
set_target_properties(CW PROPERTIES CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
