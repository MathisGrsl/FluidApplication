#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "window.hpp"

namespace ge {

    inline int mouseX;
    inline int mouseY;
    inline bool keys[GLFW_KEY_LAST] = {false};
    inline bool keysPrev[GLFW_KEY_LAST] = {false};

    inline bool clicks[8] = {false};
    inline bool clicksPrev[8] = {false};

    inline void updateInput()
    {
        for(int i = 0; i < GLFW_KEY_LAST; ++i)
            keysPrev[i] = keys[i];

        for(int i = 0; i < 8; ++i)
            clicksPrev[i] = clicks[i];
    }

    inline void handleKey(int key, int action)
    {
        if (key < 0 || key >= GLFW_KEY_LAST) return;
        keys[key] = (action != GLFW_RELEASE);
    }

    inline void handleMouseButton(int button, int action)
    {
        if (button < 0 || button >= 8) return;
        clicks[button] = (action != GLFW_RELEASE);
    }

    inline bool keyPressed(int key) { return keys[key]; }
    inline bool keyJustPressed(int key) { return keys[key] && !keysPrev[key]; }
    inline bool keyJustReleased(int key) { return !keys[key] && keysPrev[key]; }

    inline bool mousePressed(int btn) { return clicks[btn]; }
    inline bool mouseJustPressed(int btn) { return clicks[btn] && !clicksPrev[btn]; }
    inline bool mouseJustReleased(int btn) { return !clicks[btn] && clicksPrev[btn]; }

}
