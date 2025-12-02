#pragma once
#include <array>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ge/vector.hpp"
#include "ge/texture.hpp"

namespace ge {
    inline GLuint pickingFBO = 0;
    inline GLuint pickingTexture = 0;
    inline GLuint pickingDepth = 0;

    inline std::vector<std::string> objectNames;

    inline int getIndiceObject(const char *objectName)
    {
        int indice = 0;

        for (auto &name : ge::objectNames) {
            if (name == objectName)
                return indice;
            indice += 1;
        }
        return -1;
    }

    inline void newObject(const char *objectName, ge::v3 position, ge::v3 rotation, float angle, ge::v3 scale, int isLight, int texture, std::vector<ge::object> &objectsTarget)
    {
        int indice = getIndiceObject(objectName);
        ge::object::instanceData instance = {};

        if (indice == -1) {
            printf("error creation object %s\n", objectName);
            return;
        }
        instance.position = position;
        instance.rotation = rotation;
        instance.angle = angle;
        instance.scale = scale;
        instance.isLight = isLight;
        instance.texture = texture;
        instance.uvScale = { scale.x, scale.z};
        objectsTarget[indice].instances.push_back(instance);

        bool textureAlreadyUsed = false;
        for (int i = 0; i < int(objectsTarget[indice].texturesUse.size()); i += 1)
            textureAlreadyUsed = textureAlreadyUsed | (objectsTarget[indice].texturesUse[i] == texture);
        if (!textureAlreadyUsed)
            objectsTarget[indice].texturesUse.push_back(texture);
    }

    inline int loadMaps()
    {
        struct MapCategory {
            const char* folder;
            std::vector<ge::object>* target;
        };

        std::vector<MapCategory> categories = {
            { "maps/objects",  &ge::objects },
            { "maps/previous", &ge::previousObjects },
            { "maps/next",     &ge::nextObjects }
        };

        for (const auto& cat : categories)
        {
            std::filesystem::path dir = cat.folder;

            if (!std::filesystem::exists(dir)) {
                std::cerr << cat.folder << " n'existe pas\n";
                continue;  // On passe aux autres au lieu de fail
            }

            for (auto& entry : std::filesystem::directory_iterator(dir))
            {
                if (!entry.is_regular_file())
                    continue;

                std::string path = entry.path().string();
                std::string filename = entry.path().stem().string();
                std::string extension = entry.path().extension().string();

                if (extension != ".instances")
                    continue;

                int index = ge::getIndiceObject(filename.c_str());
                if (index == -1) {
                    std::cerr << "Objet inconnu dans les fichiers : " << filename << "\n";
                    continue;
                }

                std::ifstream in(path, std::ios::binary);
                if (!in.is_open()) {
                    std::cerr << "Impossible d'ouvrir : " << path << "\n";
                    continue;
                }

                int count = 0;
                in.read(reinterpret_cast<char*>(&count), sizeof(int));

                for (int i = 0; i < count; i++)
                {
                    ge::object::instanceData inst;
                    in.read(reinterpret_cast<char*>(&inst), sizeof(inst));

                    ge::newObject(
                        filename.c_str(),
                        inst.position,
                        inst.rotation,
                        inst.angle,
                        inst.scale,
                        int(inst.isLight),
                        int(inst.texture),
                        *cat.target   // <-- vecteur cible correct
                    );
                }

                in.close();
            }
        }

        return 0;
    }

    inline void saveMaps()
    {
        struct MapCategory {
            std::vector<ge::object>* source;
            std::string folder;
        };

        std::vector<MapCategory> categories = {
            { &ge::objects, "maps/objects" },
            { &ge::previousObjects, "maps/previous" },
            { &ge::nextObjects, "maps/next" }
        };

        for (const auto& cat : categories)
        {
            std::filesystem::create_directories(cat.folder);

            auto& vec = *cat.source;

            for (int i = 0; i < int(vec.size()); i++)
            {
                const auto& obj = vec[i];
                const std::string& name = ge::objectNames[i];

                std::string filePath = cat.folder + "/" + name + ".instances";

                std::ofstream out(filePath, std::ios::binary);
                if (!out.is_open()) {
                    std::cerr << "Impossible d'ouvrir : " << filePath << "\n";
                    continue;
                }

                int count = obj.instances.size();
                out.write(reinterpret_cast<const char*>(&count), sizeof(int));

                if (count > 0) {
                    out.write(
                        reinterpret_cast<const char*>(obj.instances.data()),
                        count * sizeof(ge::object::instanceData)
                    );
                }

                out.close();
            }
        }
    }

    inline std::vector<float> loadVerticesFile(const std::string& path)
    {
        std::vector<float> verts;

        std::ifstream in(path, std::ios::binary | std::ios::ate);
        if (!in.is_open())
            return verts;

        std::streamsize size = in.tellg();
        in.seekg(0, std::ios::beg);

        verts.resize(size / sizeof(float));
        in.read(reinterpret_cast<char*>(verts.data()), size);

        return verts;
    }

    inline int initObjects()
    {
        for (const auto& entry : std::filesystem::directory_iterator("include/ge/objects"))
        {
            if (!entry.is_regular_file())
                continue;

            std::string path = entry.path().string();

            if (entry.path().extension() == ".vertices")
            {
                objectNames.push_back(entry.path().stem().string());
                std::vector<float> verts = loadVerticesFile(path);

                ge::object obj = {};
                obj.vertices = std::move(verts);

                ge::objects.push_back(obj);
            }
        }

        for (auto& obj : ge::objects) {
            GLuint vao, vbo, instancesVBO;
            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);
    
            glGenBuffers(1, &vbo);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         obj.vertices.size() * sizeof(float),
                         obj.vertices.data(),
                         GL_STATIC_DRAW);
    
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);
            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);
            glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(3);
    
            glGenBuffers(1, &instancesVBO);
    
            obj.vao = vao;
            obj.vbo = vbo;
            obj.instancesVBO = instancesVBO;
    
            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        ge::previousObjects = objects;
        ge::nextObjects = objects;
        return 0;
    }

    inline void updateInstanceBuffer(const ge::object& obj)
    {
        glBindBuffer(GL_ARRAY_BUFFER, obj.instancesVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     obj.instances.size() * sizeof(ge::object::instanceData),
                     obj.instances.data(),
                     GL_DYNAMIC_DRAW);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, position));
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);

        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, rotation));
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(5, 1);

        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, angle));
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(6, 1);

        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, scale));
        glEnableVertexAttribArray(7);
        glVertexAttribDivisor(7, 1);

        glVertexAttribPointer(8, 2, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, uvScale));
        glEnableVertexAttribArray(8);
        glVertexAttribDivisor(8, 1);
        
        glVertexAttribPointer(9, 1, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, isLight));
        glEnableVertexAttribArray(9);
        glVertexAttribDivisor(9, 1);

        glVertexAttribPointer(10, 1, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, texture));
        glEnableVertexAttribArray(10);
        glVertexAttribDivisor(10, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    inline void drawObjects(std::vector<ge::object> &objectsTarget)
    {
        GLuint shader = ge::perfMode ? ge::shaderObjectPerf : ge::shaderObject;
        glUseProgram(shader);

        glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)ge::screenWidth / ge::screenHeight, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(
            glm::vec3(ge::camPos.x, ge::camPos.y, ge::camPos.z),
            glm::vec3(ge::camPos.x + ge::camDir.x, ge::camPos.y + ge::camDir.y, ge::camPos.z + ge::camDir.z),
            glm::vec3(0, 1, 0)
        );

        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniform3fv(glGetUniformLocation(shader, "camPos"), 1, glm::value_ptr(glm::vec3(ge::camPos.x, ge::camPos.y, ge::camPos.z)));

        for (auto& obj : objectsTarget)
        {
            if (obj.instances.empty())
                continue;
            int sizeTextureUse = int(obj.texturesUse.size());
            glBindVertexArray(obj.vao);
            updateInstanceBuffer(obj);

            for (int i = 0; i < sizeTextureUse; i += 4) {
                int array[4] = {-1};
                if (i < int(obj.texturesUse.size()))
                    array[0] = obj.texturesUse[i];
                if (i + 1 < int(obj.texturesUse.size()))
                    array[1] = obj.texturesUse[i + 1];
                if (i + 2 < int(obj.texturesUse.size()))
                    array[2] = obj.texturesUse[i + 2];
                if (i + 3 < int(obj.texturesUse.size()))
                    array[3] = obj.texturesUse[i + 3];
                ge::bindTextures(array, shader);
                glDrawArraysInstanced(GL_TRIANGLES, 0, obj.vertices.size() / 9, obj.instances.size());
            }
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }

    inline void initPickingBuffers()
    {
        if (pickingFBO != 0) return;

        glGenFramebuffers(1, &pickingFBO);
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);

        glGenTextures(1, &pickingTexture);
        glBindTexture(GL_TEXTURE_2D, pickingTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, ge::screenWidth, ge::screenHeight, 0,
                    GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, pickingTexture, 0);

        glGenRenderbuffers(1, &pickingDepth);
        glBindRenderbuffer(GL_RENDERBUFFER, pickingDepth);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                            ge::screenWidth, ge::screenHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                GL_RENDERBUFFER, pickingDepth);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    inline void updateInstanceBufferGetObject(const ge::object& obj)
    {
        glBindBuffer(GL_ARRAY_BUFFER, obj.instancesVBO);
        glBufferData(GL_ARRAY_BUFFER,
                     obj.instances.size() * sizeof(ge::object::instanceData),
                     obj.instances.data(),
                     GL_DYNAMIC_DRAW);

        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, position));
        glEnableVertexAttribArray(4);
        glVertexAttribDivisor(4, 1);

        glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, rotation));
        glEnableVertexAttribArray(5);
        glVertexAttribDivisor(5, 1);

        glVertexAttribPointer(6, 1, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, angle));
        glEnableVertexAttribArray(6);
        glVertexAttribDivisor(6, 1);

        glVertexAttribPointer(7, 3, GL_FLOAT, GL_FALSE, sizeof(ge::object::instanceData),
                              (void*)offsetof(ge::object::instanceData, scale));
        glEnableVertexAttribArray(7);
        glVertexAttribDivisor(7, 1);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    inline void drawObjectsPicking(std::vector<ge::object> &objectsTarget)
    {
        glUseProgram(ge::shaderGetObject);

        glm::mat4 projection = glm::perspective(glm::radians(70.0f),
            (float)ge::screenWidth / ge::screenHeight, 0.1f, 1000.0f);

        glm::mat4 view = glm::lookAt(
            glm::vec3(ge::camPos.x, ge::camPos.y, ge::camPos.z),
            glm::vec3(ge::camPos.x + ge::camDir.x, ge::camPos.y + ge::camDir.y, ge::camPos.z + ge::camDir.z),
            glm::vec3(0, 1, 0)
        );

        glUniformMatrix4fv(glGetUniformLocation(ge::shaderGetObject, "projection"),
                        1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(ge::shaderGetObject, "view"),
                        1, GL_FALSE, glm::value_ptr(view));
        int globalInstanceBase = 0;

        for (auto& obj : objectsTarget)
        {
            if (obj.instances.empty()) continue;

            glUniform1i(glGetUniformLocation(ge::shaderGetObject, "instanceBase"), globalInstanceBase);

            glBindVertexArray(obj.vao);
            updateInstanceBufferGetObject(obj);

            glDrawArraysInstanced(GL_TRIANGLES, 0, obj.vertices.size() / 9, obj.instances.size());

            globalInstanceBase += obj.instances.size();
        }

        glBindVertexArray(0);
        glUseProgram(0);
    }

    inline ge::object::instanceData *getObject(int x, int y, std::vector<ge::object> &objectsTarget,
        ge::v3 *impactPosition = nullptr, int *objType = nullptr)
    {
        initPickingBuffers();

        glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);
        glViewport(0, 0, ge::screenWidth, ge::screenHeight);
        glClearColor(0, 0, 0, 1);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawObjectsPicking(objectsTarget);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        float pixel[4];
        glBindFramebuffer(GL_FRAMEBUFFER, pickingFBO);
        glReadPixels(x, ge::screenHeight - y - 1, 1, 1, GL_RGBA, GL_FLOAT, pixel);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        int id = int(pixel[0]) - 1;

        if (id < 0)
            return nullptr;

        if (impactPosition != nullptr) {
            impactPosition->x = pixel[1];
            impactPosition->y = pixel[2];
            impactPosition->z = pixel[3];
        }

        int globalIndex = 0;
        int counter = 0;
        for (auto& obj : objectsTarget)
        {
            if (objType != nullptr)
                *objType = counter;
            counter += 1;
            if (id < int(globalIndex + obj.instances.size()))
            {
                return &obj.instances[id - globalIndex];
            }
            globalIndex += obj.instances.size();
        }

        return nullptr;
    }
}
