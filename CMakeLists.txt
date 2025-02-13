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
        Engine/graphics/Shader.cpp
        Engine/graphics/Texture.cpp
        Engine/utils/Camera.cpp
        Engine/graphics/Vertex.cpp
        Engine/graphics/Mesh.cpp
        Engine/graphics/Model.cpp
        Engine/helpers/AssimpGLMHelpers.cpp
        Engine/renderables/objects/Skybox.cpp
        Engine/renderables/objects/Sun.cpp
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
        Engine/graphics/buffers/VertexBuffer.cpp
        Engine/utils/BoundingBox.cpp
        Engine/physics/ModelAttributes.cpp
        Engine/physics/Gravity.cpp
        Engine/physics/Collisions.cpp
        Engine/utils/PlayerManager.cpp
        Engine/renderables/objects/Player.cpp
        Engine/graphics/PostProcessor.cpp
        Engine/utils/ShaderManager.cpp
        Engine/graphics/buffers/DepthBuffer.cpp
        Engine/graphics/buffers/ShadowBuffer.cpp
        Engine/renderables/Renderable.cpp
        Engine/graphics/buffers/FrameBuffer.cpp
        Engine/physics/Spline.cpp
        Engine/renderables/Entity.cpp
        Engine/renderables/ImmovableObject.cpp
        Engine/renderables/Particle.cpp
        Engine/renderables/objects/ProceduralTerrain.cpp
        Engine/utils/Noise.cpp
        Engine/renderables/objects/BumperCar.cpp
        Engine/utils/Random.cpp
        Engine/graphics/buffers/UniformBuffer.cpp
        Engine/renderables/shapes/Plane.cpp
        Engine/renderables/objects/Trees.cpp
        Engine/renderables/objects/FerrisWheel.cpp
        Engine/renderables/objects/FerrisWheel.h
        Engine/renderables/objects/RollerCoaster.cpp
        Engine/renderables/objects/RollerCoaster.h
        Engine/renderables/objects/Walls.cpp
        Engine/renderables/objects/Walls.h
        Engine/renderables/objects/Scene.cpp
        Engine/renderables/objects/Scene.h
        Engine/renderables/objects/Clouds.cpp
        Engine/renderables/objects/Clouds.h
        Engine/renderables/objects/Barriers.cpp
        Engine/renderables/objects/Barriers.h
        Engine/renderables/objects/Lights.cpp
        Engine/renderables/objects/Lights.h
        Engine/utils/Lights.h
        Engine/renderables/objects/Spotlight.cpp
        Engine/renderables/objects/Spotlight.h
)

# Link libraries
target_link_libraries(CW PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm assimp::assimp) #${SOIL2_LIB})

# Set C++ standard
set_target_properties(CW PROPERTIES CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
