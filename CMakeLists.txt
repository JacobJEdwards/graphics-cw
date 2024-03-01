cmake_minimum_required(VERSION 3.12)
project(CW)

# Find packages
find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(assimp REQUIRED)

find_library(SOIL2_LIB libsoil2 REQUIRED PATHS external/SOIL2/lib/macosx)
include_directories(external/SOIL2/src)

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
        Utils/AssimpGLMHelpers.cpp
        Graphics/Skybox.h
        Graphics/Skybox.cpp
        Graphics/ProceduralTerrain.h
        Graphics/Perlin.h
        Graphics/ProceduralTerrain.cpp
        Graphics/InfinitePlane.h
        Graphics/InfinitePlane.h
        Graphics/Sun.h
        Graphics/Sun.cpp
        external/imgui/imgui.cpp
        external/imgui/imgui.h
        external/imgui/imgui_internal.h
        external/imgui/imgui_widgets.cpp
        external/imgui/imgui_demo.cpp
        external/imgui/imgui_draw.cpp
        external/imgui/imgui_tables.cpp
        external/imgui/imconfig.h
        external/imgui/imstb_rectpack.h
        external/imgui/imstb_textedit.h
        external/imgui/imstb_truetype.h
        external/imgui/imgui_impl_glfw.cpp
        external/imgui/imgui_impl_glfw.h
        external/imgui/imgui_impl_opengl3.cpp
        external/imgui/imgui_impl_opengl3.h
        external/imgui/imgui_impl_opengl3_loader.h
        external/imgui/imgui_impl_osx.h
)

# Link libraries
target_link_libraries(CW PRIVATE OpenGL::GL GLEW::GLEW glfw glm::glm assimp::assimp ${SOIL2_LIB})

# Set C++ standard
set_target_properties(CW PROPERTIES CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



