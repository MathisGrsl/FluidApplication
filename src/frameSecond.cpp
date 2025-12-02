#include <iostream>
#include "ge.hpp"

int frameSecond(double /*deltaTime*/)
{
    snprintf(ge::fpsText, sizeof(ge::fpsText), "FPS : %d", ge::fps);

    ge::fps = 0;
    return 0;
}
