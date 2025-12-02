#version 430 core
layout(local_size_x = 256) in;

struct Molecule {vec4 position; vec4 color; vec4 direction;};
layout(std430, binding = 0) buffer Molecules {Molecule molecules[];};
layout(std430, binding = 2) buffer NbMolPerCell {int nbMolPerCell[];};

uniform int nbCells;
uniform int nbMolecules;
uniform float deltaTime;

void main() {
    uint i = gl_GlobalInvocationID.x;

    if (i < nbCells)
        nbMolPerCell[i] = 0;
    if (i < nbMolecules)
        molecules[i].position.xyz += molecules[i].direction.xyz * deltaTime;
    return;
}
