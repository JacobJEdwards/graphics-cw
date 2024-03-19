//
// Created by Jacob Edwards on 22/02/2024.
//

#include "AssimpGLMHelpers.h"

#include <assimp/matrix4x4.h>
#include <assimp/quaternion.h>
#include <assimp/vector3.h>

auto AssimpGLMHelpers::getGLMMatrix4x4(const aiMatrix4x4& from) -> glm::mat4
{
    glm::mat4 mat4;
    // the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
    mat4[0][0] = from.a1;
    mat4[1][0] = from.a2;
    mat4[2][0] = from.a3;
    mat4[3][0] = from.a4;
    mat4[0][1] = from.b1;
    mat4[1][1] = from.b2;
    mat4[2][1] = from.b3;
    mat4[3][1] = from.b4;
    mat4[0][2] = from.c1;
    mat4[1][2] = from.c2;
    mat4[2][2] = from.c3;
    mat4[3][2] = from.c4;
    mat4[0][3] = from.d1;
    mat4[1][3] = from.d2;
    mat4[2][3] = from.d3;
    mat4[3][3] = from.d4;

    return mat4;
}

auto AssimpGLMHelpers::getGLMVec(const aiVector3D& from) -> glm::vec3
{
    return { from.x, from.y, from.z };
}

auto AssimpGLMHelpers::getGLMQuat(const aiQuaternion& from) -> glm::quat
{
    return { from.w, from.x, from.y, from.z };
}
