#version 430 core
in vec2 TexCoord;
in vec4 InstanceColor;
out vec4 FragColor;

void main() {
    // center at 0.5,0.5 -> distance for soft edge
    float dist = length(TexCoord - vec2(0.5));
    float alpha = smoothstep(0.5, 0.0, dist); // tweak radii if needed
    vec3 color = InstanceColor.rgb;

    // simple specular-ish highlight toward view
    float rim = pow(1.0 - dist, 8.0) * 0.25;
    color += rim;

    FragColor = vec4(color, alpha * InstanceColor.a);
    // discard very transparent fragments to avoid overdraw artifacts
    if (FragColor.a < 0.01) discard;
}
