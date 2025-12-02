#pragma once

#include <GL/glew.h>
#include <fstream>
#include <sstream>
#include <string>
#include <iostream>

namespace ge {
    inline GLuint shaderObject = 0;
    inline GLuint shaderGetObject = 0;
    inline GLuint shaderObjectPerf = 0;

    //fluid
    inline GLuint shaderMolecule = 0;
    inline GLuint shaderPosition_collision = 0;
    inline GLuint shaderSortMolecule = 0;
    inline GLuint shaderResetArray = 0;
    inline GLuint shaderDensity_pressure = 0;
    inline GLuint shaderInitHitbox = 0;

    inline void checkShaderCompileErrors(GLuint shader, const std::string& type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Shader compilation error (" << type << "): " << infoLog << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
                std::cerr << "Program linking error: " << infoLog << std::endl;
            }
        }
    }

    inline std::string loadShaderSource(const char* filePath)
    {
        std::ifstream file(filePath);
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }
}