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
layout(std430, binding = 6) buffer GridsHitbox {int gridsHitbox[];};

uniform float cellSize;
uniform int nbCells;
uniform int nbTriangles;

uniform int xHitBox;
uniform int yHitBox;
uniform int zHitBox;

const vec3 gravity = vec3(0., -9.81, 0.);
const float PI = 3.14159265359;

int pointInTriangle(vec3 p, vec3 a, vec3 b, vec3 c)
{
    vec3 v0 = b - a;
    vec3 v1 = c - a;
    vec3 v2 = p - a;

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;

    if (abs(denom) < 1e-6) return 0;

    float v = (d11 * d20 - d01 * d21) / denom;
    float w = (d00 * d21 - d01 * d20) / denom;
    float u = 1.0 - v - w;

    return (u >= 0.0 && v >= 0.0 && w >= 0.0) ? 1 : 0;
}

float distancePointTriangle(vec3 p, vec3 a, vec3 b, vec3 c, vec3 normal)
{
    // vecteurs des arêtes
    vec3 ab = b - a;
    vec3 ac = c - a;
    vec3 bc = c - b;

    float distPlane = dot(normal, p - a);
    vec3 proj = p - distPlane * normal;

    // si la projection est à l'intérieur du triangle → renvoie la distance signée au plan
    if (pointInTriangle(proj, a, b, c) == 1)
        return distPlane;  

    // sinon, distance au bord du triangle (toujours positive)
    float distsq = 1e30;

    // distance au segment AB
    float t = clamp(dot(p - a, ab) / dot(ab, ab), 0.0, 1.0);
    vec3 closest = a + t * ab;
    distsq = min(distsq, dot(p - closest, p - closest));

    // distance au segment AC
    t = clamp(dot(p - a, ac) / dot(ac, ac), 0.0, 1.0);
    closest = a + t * ac;
    distsq = min(distsq, dot(p - closest, p - closest));

    // distance au segment BC
    t = clamp(dot(p - b, bc) / dot(bc, bc), 0.0, 1.0);
    closest = b + t * bc;
    distsq = min(distsq, dot(p - closest, p - closest));

    // renvoyer la distance POSITIVE aux arêtes, mais avec le signe du plan
    float edgeDist = sqrt(distsq);
    return (distPlane >= 0.0) ? edgeDist : -edgeDist;
}


void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= nbCells) return;
    int z = int(i);
    int x = int(z / float(yHitBox * zHitBox));
    z -= x * yHitBox * zHitBox;
    int y = int(z / float(zHitBox));
    z -= y * zHitBox;

    vec3 position = vec3(cellSize / 2.0 + x * cellSize, cellSize / 2.0 + y * cellSize, cellSize / 2.0 + z * cellSize);
    int ind = 0;

    for (int u = 0; u < nbTriangles; u += 1) {
        vec3 a = triangles[u].a;
        vec3 b = triangles[u].b;
        vec3 c = triangles[u].c;
        vec3 normal = triangles[u].normal;

        float d = distancePointTriangle(position, a, b, c, normal);
        if (abs(d) < cellSize) {
            gridsHitbox[i * 64 + ind] = u;
            ind += 1;
        }
        if (ind >= 64)
            return;
    }
}