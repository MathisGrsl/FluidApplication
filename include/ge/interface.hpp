#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <stb_easy_font.h>

namespace ge {
    inline int fps = 0;
    inline char fpsText[64] = "";
    inline char test[64] = "";

    inline void interfaceMode()
    {
        glUseProgram(0);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        glOrtho(0, ge::screenWidth, ge::screenHeight, 0, -1, 1);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glDisable(GL_DEPTH_TEST);
    }

    inline void renderText(const char* text, float x, float y, float scale, v3 color)
    {
        static char buffer[99999];
        int num_quads = stb_easy_font_print(x, y, (char*)text, nullptr, buffer, sizeof(buffer));

        glDisable(GL_DEPTH_TEST);
        glColor3f(color.x, color.y, color.z);

        glPushMatrix();
        glTranslatef(x, y, 0);
        glScalef(scale, scale, 1.0f);
        glTranslatef(-x, -y, 0);

        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(2, GL_FLOAT, 16, buffer);
        glDrawArrays(GL_QUADS, 0, num_quads * 4);
        glDisableClientState(GL_VERTEX_ARRAY);

        glPopMatrix();

        glEnable(GL_DEPTH_TEST);
    }
}
