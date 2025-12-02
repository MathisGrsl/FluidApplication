#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderGrid(void)
    {
        std::string vertexSource = ge::loadShaderSource("include/ge/shaders/fluid/grid/vertex.glsl");
        const char *vertexShaderSrc = vertexSource.c_str();

        std::string fragmentSource = loadShaderSource("include/ge/shaders/fluid/grid/fragment.glsl");
        const char* fragmentShaderSrc = fragmentSource.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);
        ge::checkShaderCompileErrors(vertexShader, "VERTEX grid");

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
        glCompileShader(fragmentShader);
        ge::checkShaderCompileErrors(fragmentShader, "FRAGMENT grid");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);
        checkShaderCompileErrors(program, "PROGRAM");

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
}

