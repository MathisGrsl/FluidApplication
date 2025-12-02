#include <GL/glew.h>
#include <string>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace ge {

    inline ge::v3 camPos(0, 1.7, 0);
    inline ge::v3 camDir(1, 0, 0);
    inline float camDegX = 0;
    inline float camDegY = 0;
    inline float fov = 70;

    inline int perfMode = 0;

    inline int screenWidth;
    inline int screenHeight;

    struct texture {
        GLuint ambientOcclusion;
        GLuint baseColor;
        GLuint metallic;
        GLuint normal;
        GLuint roughness;
        float width;
        float height;
        GLuint preview;
    };
    inline std::vector<texture> textures;

    struct object {
        GLuint vao, vbo, instancesVBO;
        std::vector<float> vertices;
        std::vector<int> texturesUse;
        struct instanceData {
            ge::v3 position;
            ge::v3 rotation;
            float angle;
            ge::v3 scale;
            ge::v2 uvScale;
            float texture;
            float isLight;
            void changeTexture(int newTexture, object& obj)
            {
                bool alreadyUsed = false;
                for (int i = 0; i < int(obj.texturesUse.size()); i += 1) {
                    if (obj.texturesUse[i] == newTexture) {
                        alreadyUsed = true;
                        break;
                    }
                }
                if (!alreadyUsed)
                    obj.texturesUse.push_back(newTexture);
                texture = newTexture;
                return;
            }
            void changeDirectionObject(ge::v3 dir, float angleRotation)
            {
                glm::vec3 direction = glm::normalize(glm::vec3(dir.x, dir.y, dir.z));

                glm::vec3 worldUp = glm::vec3(0, 1, 0);

                if (abs(glm::dot(direction, worldUp)) > 0.999f)
                    worldUp = glm::vec3(0, 0, 1);

                glm::vec3 right = glm::normalize(glm::cross(worldUp, direction));
                glm::vec3 up    = glm::cross(direction, right);

                glm::mat4 rotMat(1.0f);
                rotMat[0] = glm::vec4(right, 0.0);
                rotMat[1] = glm::vec4(up, 0.0);
                rotMat[2] = glm::vec4(direction, 0.0);

                glm::quat q = glm::quat_cast(rotMat);

                angle = 2.f * acos(q.w);
                rotation = ge::v3(q.x, q.y, q.z).normalized();
            }
        };
        std::vector<instanceData> instances;
    };

    inline std::vector<ge::object> previousObjects;
    inline std::vector<ge::object> objects;
    inline std::vector<ge::object> nextObjects;
}