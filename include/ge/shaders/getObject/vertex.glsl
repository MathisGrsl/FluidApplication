#version 430 core
layout(location = 0) in vec3 aPos;

layout(location = 4) in vec3 iPos;
layout(location = 5) in vec3 iRot;
layout(location = 6) in float iAngle;
layout(location = 7) in vec3 iScale;

uniform mat4 projection;
uniform mat4 view;
uniform int instanceBase;

flat out int vInstanceID;
out vec3 vWorldPos;

mat4 rotationAxisAngle(vec3 axis, float angle)
{
    axis = normalize(axis);
    float c = cos(angle);
    float s = sin(angle);
    float oc = 1.0 - c;

    return mat4(
        vec4(oc*axis.x*axis.x + c, oc*axis.x*axis.y - axis.z*s, oc*axis.z*axis.x + axis.y*s, 0.0),
        vec4(oc*axis.x*axis.y + axis.z*s, oc*axis.y*axis.y + c, oc*axis.y*axis.z - axis.x*s, 0.0),
        vec4(oc*axis.z*axis.x - axis.y*s, oc*axis.y*axis.z + axis.x*s, oc*axis.z*axis.z + c, 0.0),
        vec4(0,0,0,1)
    );
}

void main()
{
    mat4 model = mat4(1.0);
    model = model
        * mat4(1,0,0,0, 0,1,0,0, 0,0,1,0, iPos,1)
        * rotationAxisAngle(iRot, iAngle)
        * mat4(iScale.x,0,0,0, 0,iScale.y,0,0, 0,0,iScale.z,0, 0,0,0,1);

    vInstanceID = gl_InstanceID + instanceBase;

    vec4 worldPos = model * vec4(aPos, 1.0);
    vWorldPos = worldPos.xyz;

    gl_Position = projection * view * worldPos;
}
