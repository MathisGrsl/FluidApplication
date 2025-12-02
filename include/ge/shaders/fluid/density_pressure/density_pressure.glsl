#version 430 core

layout(local_size_x = 256) in;

struct Molecule {vec4 position; vec4 color; vec4 direction;};
struct Map {vec3 a; float p1; vec3 b; float p2; vec3 c; float pad3; vec3 normal; float pad4;};
layout(std430, binding = 0) buffer Molecules {Molecule molecules[];};
layout(std430, binding = 1) buffer Triangles {Map triangles[];};
layout(std430, binding = 2) buffer NbMolPerCell {int nbMolPerCell[];};
layout(std430, binding = 3) buffer Grids {int grids[];};
layout(std430, binding = 4) buffer Pressures {float pressures[];};
layout(std430, binding = 5) buffer Densities {float densities[];};

uniform int nbMolecules;
uniform int nbTriangles;
uniform float liquidDensity;
uniform float radiusEffect;
uniform float mass;
uniform float cellSize;
uniform float deltaTime;

uniform float compressibilityFactor;
uniform float stiffness;

uniform int xHitBox;
uniform int yHitBox;
uniform int zHitBox;

const float PI = 3.14159265359;

float kernel3d(float r, float h) {
    if (r >= h) return 0.0;
    float hr2 = h * h - r * r;
    return (315.0 / (128.0 * PI * pow(h, 9.0))) * hr2 * hr2 * hr2;
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= nbMolecules) return;

    vec3 position = molecules[i].position.xyz;

    int xIdx = int(position.x / cellSize);
    int yIdx = int(position.y / cellSize);
    int zIdx = int(position.z / cellSize);

    if (yIdx >= yHitBox || zIdx >= zHitBox || xIdx >= xHitBox || yIdx < 0 || zIdx < 0 || xIdx < 0)
        return;

    int dxP = position.x - xIdx * cellSize > cellSize / 2.0f ? 1 : 0;
    int dyP = position.y - yIdx * cellSize > cellSize / 2.0f ? 1 : 0;
    int dzP = position.z - zIdx * cellSize > cellSize / 2.0f ? 1 : 0;

    float molDensity = 0.0;

    for (int dx = -1 + dxP; dx <= dxP; dx += 1)
        for (int dy = -1 + dyP; dy <= dyP; dy += 1)
            for (int dz = -1 + dzP; dz <= dzP; dz += 1) {

                int neighborCellIndex = (xIdx + dx) * yHitBox * zHitBox + (yIdx + dy) * zHitBox + (zIdx + dz);
                int nbMolInCell = min(nbMolPerCell[neighborCellIndex], 64);
                neighborCellIndex = neighborCellIndex * 64;

                for (int x = 0; x < nbMolInCell; x += 1)
                    molDensity += mass * kernel3d(length(position - molecules[grids[neighborCellIndex + x]].position.xyz), radiusEffect);
            }

    densities[i] = molDensity;
    pressures[i] = stiffness * (pow(molDensity / liquidDensity, compressibilityFactor) - 1);
}