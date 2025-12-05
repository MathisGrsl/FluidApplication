#version 430 core
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aTex;

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
uniform float radius;

out vec2 TexCoord;
out vec4 InstanceColor;
out vec3 FragPos;
out vec3 SphereCenter;

void main() {
    uint id = gl_InstanceID;
    vec3 instancePos = molecules[id].position.xyz;
    vec4 instanceColor = molecules[id].color;

    vec3 right = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 up    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 billboardOffset = (aPos.x * right + aPos.y * up) * radius * 2.0;

    vec4 worldPosition = vec4(instancePos + billboardOffset, 1.0);
    gl_Position = projection * view * worldPosition;

    TexCoord = aTex;
    InstanceColor = instanceColor;
    FragPos = worldPosition.xyz;
    SphereCenter = instancePos;
}
