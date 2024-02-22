cmake_minimum_required(VERSION 3.12)
project(CW)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(assimp REQUIRED)

# Add executable
add_executable(CW main.cpp
        Graphics/Shader.h
        Graphics/Shader.cpp
        Graphics/stb_image.h
        Graphics/Texture.h
        Graphics/Texture.cpp
        Game/Camera.h
        Game/Camera.cpp
        Graphics/Vertex.h
        Graphics/Mesh.h
        Graphics/Vertex.cpp
        Graphics/Mesh.cpp
        Graphics/Model.h
        Graphics/Model.cpp
        Graphics/Model.cpp
        Utils/AssimpGLMHelpers.h
        Utils/AssimpGLMHelpers.cpp)

# Link libraries
target_link_libraries(CW PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm assimp::assimp)

# Set C++ standard
set_target_properties(CW PROPERTIES CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



