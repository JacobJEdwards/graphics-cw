//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef ASSIMPGLMHELPERS_H
#define ASSIMPGLMHELPERS_H

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/types.h>
#include <assimp/vector3.h>
#include <glm/fwd.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_float4x4.hpp>

class AssimpGLMHelpers {
public:
  static auto getGLMMatrix4x4(const aiMatrix4x4 &from) -> glm::mat4;

  static auto getGLMVec(const aiVector3D &from) -> glm::vec3;

  static auto getGLMVec(const aiColor3D &from) -> glm::vec3;

  static auto getGLMVec(const aiColor4D &from) -> glm::vec4;
};

#endif // ASSIMPGLMHELPERS_H
