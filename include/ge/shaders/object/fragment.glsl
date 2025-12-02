#version 430 core

in vec2 TexCoord;
in vec3 FragPos;
in vec3 Normal;
flat in int TexIndex;
flat in int IsLight;

out vec4 FragColor;

uniform sampler2D textures[16];
uniform int indiceTextures[4];
uniform vec3 lightPos = vec3(-5, 5, -5);
uniform vec3 lightColor = vec3(1.0);
uniform vec3 camPos;

vec3 saturateColor(vec3 c, float factor)
{
    float gray = dot(c, vec3(0.299, 0.587, 0.114));
    return mix(vec3(gray), c, factor);
}

void main()
{
    vec3 baseColor = texture(textures[TexIndex * 4], TexCoord).rgb;
    float metallic = texture(textures[TexIndex * 4 + 2], TexCoord).r;
    float roughness = texture(textures[TexIndex * 4 + 3], TexCoord).r;

    vec3 N = normalize(Normal);

    if (IsLight == 0) {
        vec3 normalMap = texture(textures[TexIndex * 4 + 1], TexCoord).rgb;
        vec3 nMap = normalize(normalMap * 2.0 - 1.0);

        vec3 T = normalize(vec3(1.0, 0.0, 0.0));
        if(abs(dot(N, T)) > 0.999) T = normalize(vec3(0.0, 0.0, 1.0));
        vec3 B = normalize(cross(N, T));
        T = cross(B, N);

        mat3 TBN = mat3(T, B, N);

        N = normalize(TBN * nMap);;
    }

    vec3 L = normalize(lightPos - FragPos);
    if (IsLight == 1)
        L = -L;
    vec3 V = normalize(camPos - FragPos);
    vec3 H = normalize(L + V);

    baseColor = saturateColor(baseColor, 1.0 + metallic * 1.5);
    baseColor *= mix(1.0, 1.2, metallic);
    baseColor = mix(baseColor * 0.8, baseColor, 1.0 - roughness);

    float NdotL = max(dot(N, L), 0.0);

    vec3 diffuse = baseColor * NdotL;

    float specPower = mix(4.0, 64.0, metallic) * (1.0 - roughness);
    float spec = pow(max(dot(N, H), 0.0), specPower);
    vec3 specular = lightColor * spec * metallic;

    baseColor = diffuse + specular;

    float diff = 1.0;
    
    FragColor = vec4(baseColor, 1.0);
}
