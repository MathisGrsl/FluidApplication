#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint initShaderMolecule(void)
    {
        std::string vertexSource = ge::loadShaderSource("include/ge/shaders/fluid/molecule/vertex.glsl");
        const char *vertexShaderSrc = vertexSource.c_str();

        std::string fragmentSource = ge::loadShaderSource("include/ge/shaders/fluid/molecule/fragment.glsl");
        const char* fragmentShaderSrc = fragmentSource.c_str();

        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSrc, nullptr);
        glCompileShader(vertexShader);

        GLint success;
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            printf("Vertex Shader Error:\n%s\n", infoLog);
        }

        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSrc, nullptr);
        glCompileShader(fragmentShader);

        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            printf("Fragment Shader Error:\n%s\n", infoLog);
        }

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            char infoLog[512];
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            fprintf(stderr, "Shader Link Error:\n%s\n", infoLog);
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }
}

