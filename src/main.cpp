#include <iostream>
#include "global.hpp"
#include "ge.hpp"

int main() {

    if (ge::initWindow(30, 165) == -1)
        return 1;

    glfwSetCursorPosCallback(ge::window, mouse_callback);
    glfwSetFramebufferSizeCallback(ge::window, framebuffer_size_callback);
    glfwSetKeyCallback(ge::window, key_callback);
    glfwSetMouseButtonCallback(ge::window, button_callback);
    glfwSetScrollCallback(ge::window, scroll_callback);

    glfwSetCursorPos(ge::window, ge::screenWidth / 2, ge::screenHeight / 2);

    glfwMaximizeWindow(ge::window);

    initialisation();
    ge::loadMaps();

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(ge::window)) {
        double currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        if (ge::clockEvent.on(deltaTime))
            frameEvent(ge::max(deltaTime, ge::clockEvent.deltaLimit));
        if (ge::clockRendering.on(deltaTime))
            frameRendering(ge::max(deltaTime, ge::clockRendering.deltaLimit));
        if (ge::clockSecond.on(deltaTime))
            frameSecond(ge::max(deltaTime, ge::clockSecond.deltaLimit));
    }
    if (ge::objectSelected != nullptr) {
        ge::objectSelected->texture = ge::objectSelectedOriginSetting.texture;
    }
    ge::saveMaps();
    glfwDestroyWindow(ge::window);
    glfwTerminate();
    return 0;
}
