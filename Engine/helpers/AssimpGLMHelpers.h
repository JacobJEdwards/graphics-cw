//
// Created by Jacob Edwards on 22/02/2024.
//

#ifndef ASSIMPGLMHELPERS_H
#define ASSIMPGLMHELPERS_H

#include <assimp/quaternion.h>
#include <assimp/vector3.h>
#include <assimp/matrix4x4.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

class AssimpGLMHelpers {
public:
    static auto getGLMMatrix4x4(const aiMatrix4x4& from) -> glm::mat4;
    static auto getGLMVec(const aiVector3D& from) -> glm::vec3;
    static auto getGLMQuat(const aiQuaternion& from) -> glm::quat;
};

#endif //ASSIMPGLMHELPERS_H
