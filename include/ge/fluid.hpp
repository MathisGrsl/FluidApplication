#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ge.hpp"

namespace ge {

    inline const int MAX_STORAGE = 100000;

    struct Molecule {
        v4 position;
        v4 color;
        v4 direction;
    };

    struct Map {
        v3 a;
        float p1;
        v3 b;
        float p2;
        v3 c;
        float pad3;
        v3 normal;
        float pad4;
    };

    inline GLuint vao = 0;
    inline GLuint vbo = 0;
    inline GLuint molecules = 0;
    inline GLuint triangles = 0;
    inline GLuint nbMolPerCell = 0;
    inline GLuint grids = 0;
    inline GLuint gridsHitbox = 0;
    inline GLuint pressures = 0;
    inline GLuint densities = 0;
    inline GLuint collisionFlagBuffer = 0;
    inline GLuint captors = 0;

    inline int precision = 20;
    inline int moleculeCount = 0;
    inline int triangleCount = 0;
    inline int captorCount = 0;

    inline float xHitBox = 220;
    inline float yHitBox = 50;
    inline float zHitBox = 300;
    inline int NB_CELLS = int(xHitBox) * int(yHitBox) * int(zHitBox);

    inline float radiusMol = 0;
    inline float massMol = 0;
    inline float radiusEffect = 0;
    inline float cellSize = 0;
    inline float liquidDensity = 1000.0f;

    inline float compressibilityFactor = 7.0f;
    inline float stiffness = 10.0f;
    inline float speed = 0.00f;

    inline float viscosity = 0.1;

    inline bool simulationOn = false;

    inline void initMolecules(float rad)
    {
        radiusMol = rad * 2.0;
        radiusEffect = rad * 4.0f;
        cellSize = radiusEffect * 2.0;
        massMol = (rad * 2.0f) * (rad * 2.0f) * (rad * 2.0f) * liquidDensity;

        float quadVertices[] = {
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, 0.0f, 0.0f, 1.0f,
            0.5f,  0.5f, 0.0f, 1.0f, 1.0f
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);

        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // texcoord
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);

        glBindVertexArray(0);


        glGenBuffers(1, &molecules);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, molecules);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_STORAGE * sizeof(Molecule), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &triangles);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangles);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_STORAGE * sizeof(Map), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &nbMolPerCell);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, nbMolPerCell);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NB_CELLS * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &grids);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, grids);
        glBufferData(GL_SHADER_STORAGE_BUFFER, NB_CELLS * 64 * sizeof(int), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        std::vector<int> init(NB_CELLS * 64, -1);
        glGenBuffers(1, &gridsHitbox);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, gridsHitbox);
        glBufferData(GL_SHADER_STORAGE_BUFFER,
                    init.size() * sizeof(int),
                    init.data(),
                    GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &pressures);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, pressures);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_STORAGE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &densities);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, densities);
        glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_STORAGE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &collisionFlagBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionFlagBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glGenBuffers(1, &captors);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, captors);
        glBufferData(GL_SHADER_STORAGE_BUFFER, 30 * sizeof(Map), nullptr, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


        glBindVertexArray(0);
    }

    inline void newMolecule(v4 pos, v4 color)
    {
        if (moleculeCount >= MAX_STORAGE) return;

        Molecule mol = {pos, color, v4(0, 0, 0, -1.0)};

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, molecules);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, moleculeCount * sizeof(Molecule), sizeof(Molecule), &mol);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        moleculeCount += 1;
    }

    inline void generateMolecules()
    {
        float spacing = 0.10;
        int widthCount  = int(2.0 / spacing);
        int lengthCount = int(2.0 / spacing);
        int perLayer    = widthCount * lengthCount;

        for (int i = 0; i < 50000; i += 1) {
            int x = i % widthCount;
            int y = (i / widthCount) % lengthCount;
            int z = i / perLayer;
            //v4(220/255.0, 200/255.0, 120/255.0, 0.4)
            newMolecule(
                v4(44.0 + x * spacing,
                0.2 + y * spacing + z * spacing / 5.5,
                23.0 + z * spacing, 0.0),
                v4(33/255.0, 166/255.0, 232/255.0, 0.4)
            );
        }
    }

    inline void newTriangle(v3 a, v3 b, v3 c)
    {
        if (triangleCount >= MAX_STORAGE) return;

        v3 normal = (cross(b - a, c - a)).normalized();

        Map mol = {a, 0.0, b, 0.0, c, 0.0, normal, 0.0};

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, triangles);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, triangleCount * sizeof(Map), sizeof(Map), &mol);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangles);

        triangleCount += 1;
    }

    inline void drawMolecule()
    {
        glm::mat4 projection = glm::perspective(glm::radians(ge::fov), (float)ge::screenWidth / ge::screenHeight, 0.1f, 1000.0f);
        glm::mat4 view = glm::lookAt(ge::camPos.toGlm(), (ge::camPos + ge::camDir).toGlm(), glm::vec3(0, 1, 0));

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, molecules);

        glUseProgram(shaderMolecule);
        glUniformMatrix4fv(glGetUniformLocation(shaderMolecule, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderMolecule, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, moleculeCount);

        glDisable(GL_BLEND);
        glBindVertexArray(0);
    }

    inline void initGridHitbox(std::vector<ge::object>& objects)
    {
        for (auto& object : objects) {
            for (auto& instance : object.instances) {
                glm::mat4 model = glm::translate(glm::mat4(1.0f), instance.position.toGlm());
                model = glm::rotate(model, instance.angle, glm::vec3(instance.rotation.x, instance.rotation.y, instance.rotation.z));

                for (size_t i = 0; i < object.vertices.size(); i += 27) {

                    glm::vec4 p0(object.vertices[i + 0], object.vertices[i + 1], object.vertices[i + 2], 1.0f);
                    glm::vec4 p1(object.vertices[i + 9], object.vertices[i + 10], object.vertices[i + 11], 1.0f);
                    glm::vec4 p2(object.vertices[i + 18], object.vertices[i + 19], object.vertices[i + 20], 1.0f);

                    p0 = model * p0;
                    p1 = model * p1;
                    p2 = model * p2;

                    newTriangle(
                        v3(p0.x, p0.y, p0.z),
                        v3(p1.x, p1.y, p1.z),
                        v3(p2.x, p2.y, p2.z)
                    );
                }
            }
        }
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, molecules);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangles);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nbMolPerCell);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grids);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pressures);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, densities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, gridsHitbox);

        glUseProgram(shaderInitHitbox);
        glUniform1f(glGetUniformLocation(shaderInitHitbox, "cellSize"), cellSize);
        glUniform1i(glGetUniformLocation(shaderInitHitbox, "nbCells"), NB_CELLS);
        glUniform1i(glGetUniformLocation(shaderInitHitbox, "nbTriangles"), triangleCount);

        glUniform1i(glGetUniformLocation(shaderInitHitbox, "xHitBox"), int(xHitBox));
        glUniform1i(glGetUniformLocation(shaderInitHitbox, "yHitBox"), int(yHitBox));
        glUniform1i(glGetUniformLocation(shaderInitHitbox, "zHitBox"), int(zHitBox));

        glDispatchCompute((NB_CELLS + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    inline void InitPressuresStart(float deltaTime)
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, molecules);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, triangles);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, nbMolPerCell);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, grids);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, pressures);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, densities);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 6, gridsHitbox);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 8, captors);

        glUseProgram(shaderResetArray);
        glUniform1i(glGetUniformLocation(shaderResetArray, "nbCells"), NB_CELLS);
        glUniform1i(glGetUniformLocation(shaderResetArray, "nbMolecules"), moleculeCount);
        glUniform1f(glGetUniformLocation(shaderResetArray, "deltaTime"), deltaTime);

        glDispatchCompute((NB_CELLS + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);


        glUseProgram(shaderSortMolecule);
        glUniform1i(glGetUniformLocation(shaderSortMolecule, "nbMolecules"), moleculeCount);
        glUniform1f(glGetUniformLocation(shaderSortMolecule, "cellSize"), cellSize);

        glUniform1i(glGetUniformLocation(shaderSortMolecule, "xHitBox"), int(xHitBox));
        glUniform1i(glGetUniformLocation(shaderSortMolecule, "yHitBox"), int(yHitBox));
        glUniform1i(glGetUniformLocation(shaderSortMolecule, "zHitBox"), int(zHitBox));

        glDispatchCompute((moleculeCount + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

    
        glUseProgram(shaderDensity_pressure);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "deltaTime"), deltaTime);
        glUniform1i(glGetUniformLocation(shaderDensity_pressure, "nbMolecules"), moleculeCount);
        glUniform1i(glGetUniformLocation(shaderDensity_pressure, "nbTriangles"), triangleCount);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "mass"), massMol);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "cellSize"), cellSize);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "radiusEffect"), radiusEffect);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "liquidDensity"), liquidDensity);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "compressibilityFactor"), compressibilityFactor);
        glUniform1f(glGetUniformLocation(shaderDensity_pressure, "stiffness"), stiffness);

        glUniform1i(glGetUniformLocation(shaderDensity_pressure, "xHitBox"), int(xHitBox));
        glUniform1i(glGetUniformLocation(shaderDensity_pressure, "yHitBox"), int(yHitBox));
        glUniform1i(glGetUniformLocation(shaderDensity_pressure, "zHitBox"), int(zHitBox));

        glDispatchCompute((moleculeCount + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
    }

    inline int update(float deltaTime)
    {
        InitPressuresStart(deltaTime);

        glUseProgram(shaderPosition_collision);
        glUniform1f(glGetUniformLocation(shaderPosition_collision, "deltaTime"), deltaTime);
        glUniform1i(glGetUniformLocation(shaderPosition_collision, "nbMolecules"), moleculeCount);
        glUniform1i(glGetUniformLocation(shaderPosition_collision, "nbTriangles"), triangleCount);
        glUniform1i(glGetUniformLocation(shaderPosition_collision, "nbCaptors"), captorCount);

        glUniform1f(glGetUniformLocation(shaderPosition_collision, "liquidDensity"), liquidDensity);
        glUniform1f(glGetUniformLocation(shaderPosition_collision, "radiusEffect"), radiusEffect);

        glUniform1f(glGetUniformLocation(shaderPosition_collision, "radiusDensity"), radiusEffect);
        glUniform1f(glGetUniformLocation(shaderPosition_collision, "radius"), radiusMol);

        glUniform1f(glGetUniformLocation(shaderPosition_collision, "cellSize"), cellSize);
        glUniform1f(glGetUniformLocation(shaderPosition_collision, "mass"), massMol);

        glUniform1f(glGetUniformLocation(shaderPosition_collision, "viscosity"), viscosity);

        glUniform1i(glGetUniformLocation(shaderPosition_collision, "xHitBox"), int(xHitBox));
        glUniform1i(glGetUniformLocation(shaderPosition_collision, "yHitBox"), int(yHitBox));
        glUniform1i(glGetUniformLocation(shaderPosition_collision, "zHitBox"), int(zHitBox));

        int zero = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionFlagBuffer);
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &zero);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 7, collisionFlagBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);


        glDispatchCompute((moleculeCount + 255) / 256, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        int result = 0;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionFlagBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int), &result);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

        return result;
    }
    
}
