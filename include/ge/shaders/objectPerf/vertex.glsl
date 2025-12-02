#version 430 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec2 aTexture;

layout (location = 4) in vec3 iPos;
layout (location = 5) in vec3 iRot;
layout (location = 6) in float iAngle;
layout (location = 7) in vec3 iScale;
layout (location = 8) in vec2 iUvScale;
layout (location = 9) in float isLight;
layout (location = 10) in float iTexture;

out vec2 TexCoord;
flat out int TexIndex;

uniform vec2 sizeTextures[4];
uniform int indiceTextures[4];
uniform mat4 projection;
uniform mat4 view;

int getIndice(int ind)
{
    for (int i = 0; i < 4; i += 1)
        if (ind == indiceTextures[i])
            return i;
    return -1;
}

mat4 rotationAxisAngle(vec3 axis, float angle)
{
    axis = normalize(axis);
    float c = cos(angle);
    float s = sin(angle);
    float oc = 1.0 - c;

    return mat4(
        vec4(oc * axis.x * axis.x + c, oc * axis.x * axis.y - axis.z * s, oc * axis.z * axis.x + axis.y * s, 0.0),
        vec4(oc * axis.x * axis.y + axis.z * s, oc * axis.y * axis.y + c, oc * axis.y * axis.z - axis.x * s, 0.0),
        vec4(oc * axis.z * axis.x - axis.y * s, oc * axis.y * axis.z + axis.x * s, oc * axis.z * axis.z + c, 0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

void main()
{
    TexIndex = getIndice(int(iTexture));

    if (TexIndex == -1) {
        gl_Position = vec4(0);
        return;
    }
    mat4 scaleMat = mat4(
        vec4(iScale.x, 0, 0, 0),
        vec4(0, iScale.y, 0, 0),
        vec4(0, 0, iScale.z, 0),
        vec4(0, 0, 0, 1)
    );

    mat4 rotMat = rotationAxisAngle(iRot, iAngle);

    mat4 translation = mat4(
        vec4(1, 0, 0, 0),
        vec4(0, 1, 0, 0),
        vec4(0, 0, 1, 0),
        vec4(iPos, 1)
    );

    mat4 model = translation * rotMat * scaleMat;

    vec4 worldPos = model * vec4(aPos, 1.0);
    if (int(isLight) == 1)
        TexCoord = aTexCoord;
    else
        TexCoord = aTexCoord * iUvScale / sizeTextures[TexIndex];

    gl_Position = projection * view * worldPos;
}
