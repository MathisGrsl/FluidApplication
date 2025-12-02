#version 430 core
layout(location = 0) in vec3 aPos;      // quad position in local billboard space (-0.5..0.5)
layout(location = 1) in vec2 aTex;      // tex coord for gradient

struct Molecule {
    vec4 position;
    vec4 color;
    vec4 direction;
};

layout(std430, binding = 0) buffer Molecules {
    Molecule molecules[];
};

uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec4 InstanceColor;

void main() {
    uint id = gl_InstanceID;
    vec3 instancePos = molecules[id].position.xyz;
    vec4 instanceColor = molecules[id].color;

    // Extract camera right/up from view matrix (column-major assumption from glm::lookAt)
    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);

    // aPos.x,aPos.y are in local quad space (e.g. -0.5..0.5). scale as needed:
    vec3 billboardOffset = aPos.x * right + aPos.y * up;

    vec4 worldPosition = vec4(instancePos + billboardOffset, 1.0);
    gl_Position = projection * view * worldPosition;

    TexCoord = aTex;
    InstanceColor = instanceColor;
}
