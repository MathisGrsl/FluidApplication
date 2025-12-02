#pragma once

#include <GL/glew.h>
#include <string>
#include <vector>
#include <filesystem>
#include <iostream>

#include "stb_image.h"
#include "ge/shader.hpp"

namespace ge {

    inline std::vector<std::string> textureNames;

    inline int getIndiceTexture(const char *textureName)
    {
        int indice = 0;

        for (auto &name : ge::textureNames) {
            if (name == textureName)
                return indice;
            indice += 1;
        }
        return -1;
    }

    inline GLuint loadTexture(const std::string& path) {
        int width, height, channels;
        stbi_set_flip_vertically_on_load(true);
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
        if (!data) {
            return 0;
        }

        GLenum format = GL_RGB;
        if (channels == 1) format = GL_RED;
        else if (channels == 3) format = GL_RGB;
        else if (channels == 4) format = GL_RGBA;

        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
        return tex;
    }

    inline void applyAOToBaseColor(GLuint& baseTex, GLuint aoTex)
    {
        glBindTexture(GL_TEXTURE_2D, baseTex);

        int width, height;
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &width);
        glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &height);

        std::vector<unsigned char> baseData(width * height * 3);
        std::vector<unsigned char> aoData(width * height);

        glGetTexImage(GL_TEXTURE_2D, 0, GL_RGB, GL_UNSIGNED_BYTE, baseData.data());

        glBindTexture(GL_TEXTURE_2D, aoTex);
        glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, aoData.data());

        for (int i = 0; i < width * height; i += 1) {
            float a = aoData[i] / 255.0f;

            baseData[i * 3 + 0] = (unsigned char)(baseData[i * 3 + 0] * a);
            baseData[i * 3 + 1] = (unsigned char)(baseData[i * 3 + 1] * a);
            baseData[i * 3 + 2] = (unsigned char)(baseData[i * 3 + 2] * a);
        }

        glBindTexture(GL_TEXTURE_2D, baseTex);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, baseData.data());
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    inline int initTextures(int quality) {
        const char* qualities[4] = {
                "8K",
                "4K",
                "2K",
                "1K"
        };
        std::filesystem::path baseDir = "asset/textures";
        for (auto& dirEntry : std::filesystem::directory_iterator(baseDir)) {
            if (!dirEntry.is_directory()) continue;

            texture tex;
            textureNames.push_back(dirEntry.path().filename().string());
            std::filesystem::path texDir;
            for (int i = 3 - quality; i < 4; i += 1) {
                texDir = dirEntry.path() / qualities[i];
                if (std::filesystem::exists(texDir))
                    break;
            }
            
            if (!std::filesystem::exists(texDir)) {
                printf("not find texture\n");
                return -1;
            }
            tex.ambientOcclusion = loadTexture((texDir / "AmbientOcclusion.jpg").string());
            tex.baseColor = loadTexture((texDir / "BaseColor.jpg").string());
            tex.metallic = loadTexture((texDir / "Metallic.jpg").string());
            tex.normal = loadTexture((texDir / "Normal.png").string());
            tex.roughness = loadTexture((texDir / "Roughness.jpg").string());
            tex.preview = loadTexture((dirEntry.path() / "preview.png").string());

            if (tex.ambientOcclusion != 0)
                applyAOToBaseColor(tex.baseColor, tex.ambientOcclusion);

            std::filesystem::path sizeFile = dirEntry.path() / "data.size";
            if (std::filesystem::exists(sizeFile)) {
                std::ifstream file(sizeFile);
                if (file.is_open()) {
                    file >> tex.width >> tex.height;
                    file.close();
                } else {
                    std::cerr << "Erreur: impossible d'ouvrir " << sizeFile << std::endl;
                    tex.width = tex.height = 1.0f;
                }
            } else {
                std::cerr << "Fichier manquant : " << sizeFile << std::endl;
                tex.width = tex.height = 1.0f;
            }

            ge::textures.push_back(tex);
        }
        return 0;
    }

    inline void bindTextures(int array[4], GLuint shader)
    {
        GLint samplers[16] = {0};
        float sizeTextures[8];

        int i = 0;
        for (; i < 4; i += 1)
        {
            int indiceTexture = array[i];

            if (indiceTexture == -1)
                break;
            int idx = i * 4;

            sizeTextures[i * 2] = ge::textures[indiceTexture].width;
            sizeTextures[i * 2 + 1] = ge::textures[indiceTexture].height;

            glActiveTexture(GL_TEXTURE0 + idx + 0);
            glBindTexture(GL_TEXTURE_2D, ge::textures[indiceTexture].baseColor);
            samplers[idx + 0] = idx + 0;

            glActiveTexture(GL_TEXTURE0 + idx + 1);
            glBindTexture(GL_TEXTURE_2D, ge::textures[indiceTexture].normal);
            samplers[idx + 1] = idx + 1;

            glActiveTexture(GL_TEXTURE0 + idx + 2);
            glBindTexture(GL_TEXTURE_2D, ge::textures[indiceTexture].metallic);
            samplers[idx + 2] = idx + 2;

            glActiveTexture(GL_TEXTURE0 + idx + 3);
            glBindTexture(GL_TEXTURE_2D, ge::textures[indiceTexture].roughness);
            samplers[idx + 3] = idx + 3;
        }
        glUniform1iv(glGetUniformLocation(shader, "textures"), 16, samplers);
        glUniform2fv(glGetUniformLocation(shader, "sizeTextures"), 4, sizeTextures);
        glUniform1iv(glGetUniformLocation(shader, "indiceTextures"), 4, array);
    }
}
