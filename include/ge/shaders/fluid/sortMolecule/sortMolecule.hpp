#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderSortMolecule(void)
    {
        std::string shaderSource = ge::loadShaderSource("include/ge/shaders/fluid/sortMolecule/sortMolecule.glsl");
        const char *vertexShaderSrc = shaderSource.c_str();

        GLuint vertexShader = glCreateShader(GL_COMPUTE_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);
        ge::checkShaderCompileErrors(vertexShader, "parser");

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glLinkProgram(program);

        glDeleteShader(vertexShader);

        return program;
    }
}
