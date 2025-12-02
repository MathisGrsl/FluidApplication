#version 430 core

in vec2 TexCoord;
flat in int TexIndex;

out vec4 FragColor;

uniform sampler2D textures[16];

void main()
{
    vec3 baseColor = texture(textures[TexIndex * 4], TexCoord).rgb;
    
    FragColor = vec4(baseColor, 1.0);
}
