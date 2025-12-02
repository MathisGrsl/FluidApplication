#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderDensity_pressure(void)
    {
        std::string shaderSource = ge::loadShaderSource("include/ge/shaders/fluid/density_pressure/density_pressure.glsl");
        const char *vertexShaderSrc = shaderSource.c_str();

        GLuint vertexShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);
        ge::checkShaderCompileErrors(vertexShader, "density pressure");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);

        return program;
    }
}

