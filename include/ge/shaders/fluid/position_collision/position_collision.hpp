#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderPosition_collision(void)
    {
        std::string shaderSource = ge::loadShaderSource("include/ge/shader/position_collision/position_collision.glsl");
        const char *vertexShaderSrc = shaderSource.c_str();

        GLuint vertexShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);
        ge::checkShaderCompileErrors(vertexShader, "position collision");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);

        return program;
    }
}

