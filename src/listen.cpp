#include "ge.hpp"

void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    int deltaX = xpos - ge::mouseX;
    int deltaY = ypos - ge::mouseY;
    ge::mouseX = xpos;
    ge::mouseY = ypos;

    if (!ge::isEditing) {
        ge::moveCamera(xpos - ge::screenWidth / 2, ypos - ge::screenHeight / 2);
        glfwSetCursorPos(window, ge::screenWidth / 2, ge::screenHeight / 2);
    } else {
        if (ge::arrowSelected && ge::mousePressed(GLFW_MOUSE_BUTTON_LEFT)) {
            ge::v3 camSide = ge::cross(ge::camDir, ge::v3(0, 1, 0)).normalized();
            ge::v3 camTop = ge::cross(ge::camDir, camSide).normalized();
            float multiplicator = (ge::camPos - ge::objectSelected->position).length();

            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[0]) {
                float sens = ge::dot((camSide * deltaX).normalized(), ge::v3(1, 0, 0)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->position += sens * ge::v3(1, 0, 0) * 0.003 * multiplicator * ge::abs(deltaX);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[1]) {
                float sens = ge::dot((camSide * deltaX).normalized(), ge::v3(0, 0, 1)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->position += sens * ge::v3(0, 0, 1) * 0.003 * multiplicator * ge::abs(deltaX);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[2]) {
                float sens = ge::dot((camTop * deltaY).normalized(), ge::v3(0, 1, 0)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->position += sens * ge::v3(0, 1, 0) * 0.003 * multiplicator * ge::abs(deltaY);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[3]) {
                float sens = ge::dot((camSide * deltaX).normalized(), ge::v3(1, 0, 0)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->scale.x *= 1 + sens * 0.0003 * multiplicator * ge::abs(deltaX);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[4]) {
                float sens = ge::dot((camSide * deltaX).normalized(), ge::v3(0, 0, 1)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->scale.z *= 1 + sens * 0.0003 * multiplicator * ge::abs(deltaX);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("arrow")].instances[5]) {
                float sens = ge::dot((camTop * deltaY).normalized(), ge::v3(0, 1, 0)) < 0.0 ? -1.0 : 1.0;
                ge::objectSelected->scale.y *= 1 + sens * 0.0003 * multiplicator * ge::abs(deltaY);
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("donut")].instances[2]) {
                ge::objectSelected->angle += 0.003 * deltaX;
            } else
            if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("donut")].instances[0] ||
                ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("donut")].instances[1]) {
                    float yaw = atan2(ge::objectSelected->rotation.z, ge::objectSelected->rotation.x);
                    float pitch = -asin(ge::objectSelected->rotation.y);

                    if (ge::arrowSelected == &ge::nextObjects[ge::getIndiceObject("donut")].instances[1])
                        yaw += 0.003 * deltaY;
                    else
                        pitch += 0.003 * deltaY;
                    ge::objectSelected->rotation = ge::v3(cos(pitch) * cos(yaw), -sin(pitch), cos(pitch) * sin(yaw)).normalized();
                }
        }
    }
}

void scroll_callback(GLFWwindow */*window*/, double /*xoffset*/, double yoffset)
{
    if (ge::isCreating) {
        ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1].scale = 
        ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1].scale * (1.0 + yoffset * 10/100);
    }
}


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    window = window;
    glViewport(0, 0, width, height);

    ge::screenWidth = width;
    ge::screenHeight = height;
}

void key_callback(GLFWwindow */*window*/, int key, int /*scancode*/, int action, int /*mods*/)
{
    ge::handleKey(key, action);
}

void button_callback(GLFWwindow */*window*/, int button, int action, int /*mods*/)
{
    ge::handleMouseButton(button, action);
}

