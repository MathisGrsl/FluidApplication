#include <iostream>
#include "ge.hpp"

void renderGridHitbox()
{
    glUseProgram(ge::shaderGrid);

    glm::mat4 projection = glm::perspective(glm::radians(ge::fov), (float)ge::screenWidth / ge::screenHeight, 0.1f, 10000.0f);
    glm::mat4 view = glm::lookAt(ge::camPos.toGlm(), ge::camPos.toGlm() + ge::camDir.toGlm(), glm::vec3(0, 1, 0));
    glm::mat4 model = glm::mat4(1.0f);

    glUniformMatrix4fv(glGetUniformLocation(ge::shaderGrid, "model"), 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(glGetUniformLocation(ge::shaderGrid, "view"), 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(glGetUniformLocation(ge::shaderGrid, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3f(glGetUniformLocation(ge::shaderGrid, "lineColor"), 0.0f, 1.0f, 0.0f);

    glLineWidth(1.5f);
    glBegin(GL_LINES);


                float x0 = 0.0;
                float y0 = 0.0;
                float z0 = 0.0;
                float x1 = (x0 + ge::xHitBox * ge::cellSize);
                float y1 = (y0 + ge::yHitBox * ge::cellSize);
                float z1 = (z0 + ge::zHitBox * ge::cellSize);

                auto edge = [&](float ax, float ay, float az, float bx, float by, float bz) {
                    glVertex3f(ax, ay, az);
                    glVertex3f(bx, by, bz);
                };

                // 12 arêtes
                edge(x0, y0, z0, x1, y0, z0);
                edge(x0, y0, z0, x0, y1, z0);
                edge(x0, y0, z0, x0, y0, z1);

                edge(x1, y0, z0, x1, y1, z0);
                edge(x1, y0, z0, x1, y0, z1);

                edge(x0, y1, z0, x1, y1, z0);
                edge(x0, y1, z0, x0, y1, z1);

                edge(x0, y0, z1, x1, y0, z1);
                edge(x0, y0, z1, x0, y1, z1);

                edge(x1, y1, z0, x1, y1, z1);
                edge(x1, y0, z1, x1, y1, z1);
                edge(x0, y1, z1, x1, y1, z1);

    glEnd();
}

void drawCursor()
{
    float size = 7.0f;
    float gap = 2.0f;
    float thickness = 2.0f;
    float x = float(ge::screenWidth / 2.0);
    float y = float(ge::screenHeight / 2.0);

    glColor3f(255 / 255.0f, 192 / 255.0f, 203 / 255.0f);
    glLineWidth(thickness);
    glBegin(GL_LINES);
    // Ligne horizontale gauche
    glVertex2f(x - gap - size, y);
    glVertex2f(x - gap, y);

    // Ligne horizontale droite
    glVertex2f(x + gap, y);
    glVertex2f(x + gap + size, y);

    // Ligne verticale haut
    glVertex2f(x, y - gap - size);
    glVertex2f(x, y - gap);

    // Ligne verticale bas
    glVertex2f(x, y + gap);
    glVertex2f(x, y + gap + size);
    glEnd();
}

const ge::v3 dirArrow[3] = {
    {1, 0, 0},
    {0, 0, 1},
    {0, 1, 0}
};

void moveArrows()
{
    ge::v3 camSide = ge::cross(ge::camDir, ge::v3(0, 1, 0)).normalized();
    float lengthObjectCam = (ge::camPos - ge::objectSelected->position).length();

    
    for (int i = 0; i < 3; i += 1) {
        ge::nextObjects[ge::getIndiceObject("arrow")].instances[i].scale = ge::v3(0.2, 0.2, 0.2) * lengthObjectCam * 0.08;
        ge::nextObjects[ge::getIndiceObject("arrow")].instances[i].position = ge::objectSelected->position + dirArrow[i] * lengthObjectCam * 0.15;
    }

    for (int i = 3; i < 6; i += 1) {
        ge::nextObjects[ge::getIndiceObject("arrow")].instances[i].scale = ge::v3(0.2, 0.2, 0.2) * lengthObjectCam * 0.08;
        ge::nextObjects[ge::getIndiceObject("arrow")].instances[i].position =
            ge::objectSelected->position + dirArrow[i] * lengthObjectCam * 0.15 + camSide * lengthObjectCam * 0.6;
    }
    ge::nextObjects[ge::getIndiceObject("sphere")].instances[0].position = ge::objectSelected->position;
    ge::nextObjects[ge::getIndiceObject("sphere")].instances[0].scale = ge::v3(1.0, 1.0, 1.0) * lengthObjectCam * 0.05;

    ge::nextObjects[ge::getIndiceObject("donut")].instances[0].scale = ge::v3(0.2, 0.2, 0.2) * lengthObjectCam * 0.08;
    ge::nextObjects[ge::getIndiceObject("donut")].instances[0].position =
        ge::objectSelected->position - camSide * lengthObjectCam * 0.5 + ge::v3(0, 1, 0) * lengthObjectCam * 0.3;
    ge::nextObjects[ge::getIndiceObject("donut")].instances[0].
        changeDirectionObject(ge::camPos - ge::nextObjects[ge::getIndiceObject("donut")].instances[0].position, 0);
    

    ge::nextObjects[ge::getIndiceObject("donut")].instances[1].scale = ge::v3(0.2, 0.2, 0.2) * lengthObjectCam * 0.08;
    ge::nextObjects[ge::getIndiceObject("donut")].instances[1].position =
        ge::objectSelected->position - camSide * lengthObjectCam * 0.5 + ge::v3(0, 1, 0) * lengthObjectCam * 0.3;
    ge::nextObjects[ge::getIndiceObject("donut")].instances[1].
        changeDirectionObject(camSide, 0);
    
    ge::nextObjects[ge::getIndiceObject("donut")].instances[2].position =
        ge::objectSelected->position - camSide * lengthObjectCam * 0.5 - ge::v3(0, 1, 0) * lengthObjectCam * 0.3;
    ge::nextObjects[ge::getIndiceObject("donut")].instances[2].scale = ge::v3(0.2, 0.2, 0.2) * lengthObjectCam * 0.08;
}

void manageCreating()
{
    if (ge::isCreating) {
        ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1].position = ge::camPos + ge::camDir * 5;
    }
    if (ge::isCreating && ge::mouseJustPressed(GLFW_MOUSE_BUTTON_LEFT)) {
        ge::objects[ge::newObjectSelected].instances.push_back(ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1]);
    }
    if (ge::keyJustPressed(GLFW_KEY_TAB) && ge::isCreating) {
        ge::objects[ge::newObjectSelected].instances.pop_back();
        ge::newObjectSelected = (ge::newObjectSelected + 1) % ge::objects.size();
        ge::newObject(ge::objectNames[ge::newObjectSelected].c_str(), ge::v3(0, 0, 0), ge::v3(0, 1, 0), 0, ge::v3(1, 1, 1), 0, 0, ge::objects);
    }
    if (ge::keyJustPressed(GLFW_KEY_T) && ge::isCreating) {
        int newTexture = (int(ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1].texture) + 1) % ge::textureNames.size();
        ge::objects[ge::newObjectSelected].instances[ge::objects[ge::newObjectSelected].instances.size() - 1].changeTexture(newTexture, ge::objects[ge::newObjectSelected]);
    }
}

void manageEditing()
{
    if (ge::keyJustPressed(GLFW_KEY_DELETE) && ge::isEditing)
    {
        auto& list = ge::objects[ge::objTypeIsEditing].instances;

        for (int i = 0; i < int(list.size()); ++i)
        {
            if (&list[i] == ge::objectSelected)
            {
                list.erase(list.begin() + i);
                ge::objectSelected = nullptr;
                ge::isEditing = false;
                glfwSetCursorPos(ge::window, ge::screenWidth / 2, ge::screenHeight / 2);
                break;
            }
        }
    }

    if (ge::keyJustPressed(GLFW_KEY_N) && !ge::isEditing) {
        ge::isCreating = !ge::isCreating;
        if (ge::isCreating) {
            ge::newObjectSelected = 0;
            ge::newObject(ge::objectNames[ge::newObjectSelected].c_str(), ge::v3(0, 0, 0), ge::v3(0, 1, 0), 0, ge::v3(1, 1, 1), 0, 0, ge::objects);
        }
        else {
            ge::objects[ge::newObjectSelected].instances.pop_back();
            ge::newObjectSelected = -1;
        }
    }

    if (ge::isEditing) {
        ge::changeCamDir((ge::objectSelected->position - ge::camPos).normalized());
        glfwSetInputMode(ge::window, GLFW_CURSOR, GLFW_CURSOR_CAPTURED);
    } else {
        glfwSetInputMode(ge::window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    }

    if (ge::objectSelected != nullptr && ge::isEditing)
        moveArrows();
}


int frameRendering(double deltaTime)
{
    ge::fps += 1;

    if (ge::keyJustPressed(GLFW_KEY_L)) {
        ge::simulationOn = !ge::simulationOn;
    }

    if (ge::simulationOn) {
        ge::updateMolecules(deltaTime * ge::speed);
    }
    
    ge::deplacementCamera(deltaTime);

    if (!ge::simulationOn) {
        manageEditing();
        manageCreating();
        if (!ge::isEditing && !ge::isCreating) {
            ge::v3 impact;
            ge::object::instanceData *result = ge::getObject(ge::screenWidth / 2, ge::screenHeight / 2, ge::objects, &impact, &ge::objTypeIsEditing);

            if (result != ge::objectSelected && ge::objectSelected != nullptr) {
                ge::objectSelected->texture = ge::objectSelectedOriginSetting.texture;
            }

            if (result != ge::objectSelected && result != nullptr) {
                ge::objectSelectedOriginSetting = *result;
                if (ge::objTypeIsEditing != -1)
                    result->changeTexture(ge::getIndiceTexture("edit"), ge::objects[ge::objTypeIsEditing]);
            }

            ge::objectSelected = result;
        } else {
            if (ge::mouseJustPressed(GLFW_MOUSE_BUTTON_LEFT))
                ge::arrowSelected = ge::getObject(ge::mouseX, ge::mouseY, ge::nextObjects);
            if (ge::mouseJustReleased(GLFW_MOUSE_BUTTON_LEFT))
                ge::arrowSelected = nullptr;
        }

        if (ge::objectSelected != nullptr && ge::keyJustPressed(GLFW_KEY_E) && !ge::isCreating) {
            if (ge::isEditing)
                glfwSetCursorPos(ge::window, ge::screenWidth / 2, ge::screenHeight / 2);
            ge::isEditing = !ge::isEditing;
        }
    }

    
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (ge::previousObjects[ge::getIndiceObject("dome")].instances.size()) {
        ge::previousObjects[ge::getIndiceObject("dome")].instances[0].position.x = ge::camPos.x;
        ge::previousObjects[ge::getIndiceObject("dome")].instances[0].position.z = ge::camPos.z;
    }
    ge::drawObjects(ge::previousObjects);

    renderGridHitbox();

    if (!ge::simulationOn)
        ge::drawObjects(ge::objects);

    glDepthMask(GL_FALSE);
    ge::drawMolecule();
    glDepthMask(GL_TRUE);

    if (ge::isEditing) {
        glClear(GL_DEPTH_BUFFER_BIT);
        ge::drawObjects(ge::nextObjects);
    }

    ge::interfaceMode();
    ge::renderText(ge::fpsText, 10, 10, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));

    if (!ge::isCreating && !ge::isEditing) {
        ge::renderText("press E to edit an object", 10, 150, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
        ge::renderText("press N to create an object", 10, 200, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
    }
    if (ge::isCreating) {
        ge::renderText("press TAB to change object", 10, 150, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
        ge::renderText("press T to change the texture", 10, 200, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
        ge::renderText("left click to add object", 10, 250, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
    }
    if (ge::isEditing) {
        ge::renderText("press SUP to delete the object", 10, 150, 2, ge::v3(255 / 255.0f, 192 / 255.0f, 203 / 255.0f));
    }

    drawCursor();
    glfwSwapBuffers(ge::window);
    ge::updateInput();
    glfwPollEvents();

    return 0;
}
