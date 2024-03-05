//
// Created by Jacob Edwards on 02/03/2024.
//

#ifndef CW_ENGINE_H
#define CW_ENGINE_H

#include <iostream>
#include <functional>
#include <initializer_list>
#include <string>
#include <filesystem>
#include <sstream>

#include "./include/View.h"

#include <GL/glew.h>
#include <glfw/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using Handle = std::function<void()>;

namespace Engine {
    static View view;

    bool init() {

    }
}

#endif //CW_ENGINE_H
