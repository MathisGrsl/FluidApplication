#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderResetArray(void)
    {
        std::string shaderSource = ge::loadShaderSource("include/ge/shaders/fluid/resetArray/resetArray.glsl");
        const char *vertexShaderSrc = shaderSource.c_str();

        GLuint vertexShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);
        ge::checkShaderCompileErrors(vertexShader, "reset array and preshot next position update");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);

        return program;
    }
}

