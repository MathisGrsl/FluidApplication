#version 430 core

in vec2 TexCoord;
in vec4 InstanceColor;
in vec3 FragPos;
in vec3 SphereCenter;

uniform float radius;
uniform mat4 projection;
uniform mat4 view;

out vec4 FragColor;

void main() {
    // Ray-Sphere Intersection
    // Ray origin is camera position (inverse view matrix translation)
    // But easier: work in View Space or World Space.
    // Let's use World Space since we have FragPos and SphereCenter.
    
    vec3 camPos = vec3(inverse(view)[3]); // Extract camera position from view matrix
    vec3 rayDir = normalize(FragPos - camPos);
    vec3 sphereToCam = camPos - SphereCenter;

    float b = dot(rayDir, sphereToCam);
    float c = dot(sphereToCam, sphereToCam) - radius * radius;
    float h = b * b - c;

    if (h < 0.0) discard; // No intersection

    float t = -b - sqrt(h); // Closest intersection distance
    vec3 intersectionPos = camPos + t * rayDir;
    vec3 normal = normalize(intersectionPos - SphereCenter);

    // Calculate Depth
    vec4 clipPos = projection * view * vec4(intersectionPos, 1.0);
    float ndcDepth = clipPos.z / clipPos.w;
    gl_FragDepth = (ndcDepth + 1.0) / 2.0;

    // Lighting (Water Style)
    vec3 lightDir = normalize(vec3(0.5, 1.0, 0.5));
    vec3 viewDir = normalize(camPos - intersectionPos);
    vec3 halfDir = normalize(lightDir + viewDir);

    // Base Color (Deep Blue)
    vec3 baseColor = vec3(0.0, 0.3, 0.7); 

    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    
    // Specular (Sharp wet highlight)
    float spec = pow(max(dot(normal, halfDir), 0.0), 64.0);
    
    // Fresnel (Brighter edges)
    float fresnel = pow(1.0 - max(dot(normal, viewDir), 0.0), 3.0);

    vec3 finalColor = baseColor * 0.6 + vec3(1.0) * spec * 0.8 + vec3(0.4, 0.6, 1.0) * fresnel * 0.5;

    FragColor = vec4(finalColor, 1.0);
}
