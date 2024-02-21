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
        shaders/Shader.h
        shaders/Shader.cpp
        stb_image.h
        Texture.h
        Texture.cpp
        Camera.h
        Camera.cpp
        Vertex.h
        Mesh.h
        Vertex.cpp)

# Link libraries
target_link_libraries(CW PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm assimp::assimp)

# Set C++ standard
set_target_properties(CW PROPERTIES CXX_STANDARD 14)



