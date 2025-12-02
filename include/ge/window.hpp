#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "object.hpp"
#include "texture.hpp"
#include "camera.hpp"
#include "shader.hpp"
#include "shaders/object/object.hpp"
#include "shaders/getObject/getObject.hpp"
#include "shaders/objectPerf/objectPerf.hpp"

//fluid
#include "shaders/fluid/density_pressure/density_pressure.hpp"
#include "shaders/fluid/grid/grid.hpp"
#include "shaders/fluid/initHitbox/initHitbox.hpp"
#include "shaders/fluid/molecule/molecule.hpp"
#include "shaders/fluid/position_collision/position_collision.hpp"
#include "shaders/fluid/resetarray/resetarray.hpp"
#include "shaders/fluid/sortMolecule/sortMolecule.hpp"


namespace ge {
    inline GLFWwindow *window = nullptr;
    inline ge::clock clockEvent;
    inline ge::clock clockRendering;
    inline ge::clock clockSecond;

    inline int initWindow(int frameEvent, int frameRendering)
    {
        if (!glfwInit()) return -1;

        clockEvent = ge::clock(frameEvent);
        clockRendering = ge::clock(frameRendering);
        clockSecond = ge::clock(1);

        GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(primaryMonitor);
        glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
        ge::screenWidth = mode->width;
        ge::screenHeight = mode->height;
        window = glfwCreateWindow(ge::screenWidth, ge::screenHeight, "", nullptr, nullptr);
        if (!window) {
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);
        glewInit();
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        glfwSwapInterval(0);
        if (ge::initTextures(0) == -1)
            return -1;
        if (ge::initObjects() == -1)
            return -1;
        ge::shaderObject = ge::initShaderObject();
        ge::shaderGetObject = ge::initShaderGetObject();
        ge::shaderObjectPerf = ge::initShaderObjectPerf();

        //fluid
        ge::shaderMolecule = ge::initShaderMolecule();
        ge::shaderPosition_collision = ge::initShaderPosition_collision();
        ge::shaderSortMolecule = ge::initShaderSortMolecule();
        ge::shaderResetArray = ge::initShaderResetArray();
        ge::shaderDensity_pressure = ge::initShaderDensity_pressure();
        ge::shaderInitHitbox = ge::initShaderinitHitbox();
        return 0;
    }
}
