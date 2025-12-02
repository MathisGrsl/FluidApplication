#pragma once

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "window.hpp"
#include "event.hpp"

namespace ge {

    inline void moveCamera(double deltaX, float deltaY)
    {
        ge::camDegX += deltaX * 0.1f;
        ge::camDegY += deltaY * 0.1f;
        if (ge::camDegY > 89.0f) ge::camDegY = 89.0f;
        if (ge::camDegY < -89.0f) ge::camDegY = -89.0f;
        float yawRad = ge::deg2rad(ge::camDegX);
        float pitchRad = ge::deg2rad(ge::camDegY);

        ge::camDir.x = cos(pitchRad) * cos(yawRad);
        ge::camDir.y = -sin(pitchRad);
        ge::camDir.z = cos(pitchRad) * sin(yawRad);
        ge::camDir = ge::camDir.normalized();
    }
    inline void changeCamDir(ge::v3 direction)
    {
        ge::camDir = direction.normalized();
        float pitch = -asin(direction.y);
        float yaw   = atan2(direction.z, direction.x);

        ge::camDegY = ge::rad2deg(pitch);
        ge::camDegX = ge::rad2deg(yaw);
    }
    inline void deplacementCamera(float deltaTime)
    {
        const float speed = 5.0f;
        ge::v3 right = ge::cross(ge::camDir, ge::v3(0, 1, 0)).normalized();
        ge::v3 up(0, 1, 0);

        if (keys[GLFW_KEY_W])
            ge::camPos += ge::camDir * speed * deltaTime;
        if (keys[GLFW_KEY_S])
            ge::camPos -= ge::camDir * speed * deltaTime;
        if (keys[GLFW_KEY_A])
            ge::camPos -= right * speed * deltaTime;
        if (keys[GLFW_KEY_D])
            ge::camPos += right * speed * deltaTime;
        if (keys[GLFW_KEY_SPACE])
            ge::camPos += up * speed * deltaTime;
        if (keys[GLFW_KEY_LEFT_SHIFT])
            ge::camPos -= up * speed * deltaTime;
    }
}
