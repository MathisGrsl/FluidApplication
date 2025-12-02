#version 430 core

layout(local_size_x = 256) in;

struct Molecule {
    vec4 position;
    vec4 color;
    vec4 direction;
};

layout(std430, binding = 0) buffer Molecules {
    Molecule molecules[];
};

layout(std430, binding = 2) buffer NbMolPerCell {
    int nbMolPerCell[];
};

layout(std430, binding = 3) buffer Grids {
    int grids[];
};

uniform int nbMolecules;
uniform float cellSize;

uniform int xHitBox;
uniform int yHitBox;
uniform int zHitBox;

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= nbMolecules) return;

    int xIdx = int(molecules[i].position.x / cellSize);
    int yIdx = int(molecules[i].position.y / cellSize);
    int zIdx = int(molecules[i].position.z / cellSize);
    int gridIndex = xIdx * yHitBox * zHitBox + yIdx * zHitBox + zIdx;

    if (yIdx >= yHitBox || zIdx >= zHitBox || xIdx >= xHitBox || yIdx < 0 || zIdx < 0 || xIdx < 0)
        return;

    int idx = atomicAdd(nbMolPerCell[gridIndex], 1);
    if (idx >= 64)
        return;

    grids[gridIndex * 64 + idx] = int(i);
    return;
}