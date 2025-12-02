#version 430 core

flat in int vInstanceID;
in vec3 vWorldPos;

out vec4 FragColor;

void main()
{
    FragColor = vec4(float(vInstanceID + 1), vWorldPos);
}
