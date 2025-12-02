#version 430 core

layout(local_size_x = 256) in;

struct Molecule {vec4 position; vec4 color; vec4 direction;};
struct Map {vec3 a; float p1; vec3 b; float p2; vec3 c; float pad3; vec3 normal; float pad4;};
struct Captor {vec3 position; float p1; vec3 direction; float p2; vec3 data; float p3; vec3 data1; float p4;};
layout(std430, binding = 0) buffer Molecules {Molecule molecules[];};
layout(std430, binding = 1) buffer Triangles {Map triangles[];};
layout(std430, binding = 2) buffer NbMolPerCell {int nbMolPerCell[];};
layout(std430, binding = 3) buffer Grids {int grids[];};
layout(std430, binding = 4) buffer Pressures {float pressures[];};
layout(std430, binding = 5) buffer Densities {float densities[];};
layout(std430, binding = 6) buffer GridsHitbox {int gridsHitbox[];};

layout(std430, binding = 7) buffer CollisionFlag {int collisionFlag;};

layout(std430, binding = 8) buffer Captors {Captor captors[];};

uniform float deltaTime;
uniform int nbMolecules;
uniform int nbTriangles;
uniform int nbCaptors;
uniform float liquidDensity;
uniform float radiusDensity;
uniform float radiusEffect;
uniform float radius;
uniform float cellSize;
uniform float mass;

uniform float viscosity;

uniform int xHitBox;
uniform int yHitBox;
uniform int zHitBox;

float diameter = 2. * radius;
const vec3 gravity = vec3(0., -9.81, 0.);
const float PI = 3.14159265359;

const float spikyFactor = -45.0 / (PI * pow(radiusEffect, 6.0)); // gradient spiky (3D)
const float viscLaplacian = 45.0 / (PI * pow(radiusEffect, 6.0)); // laplacien viscosité (3D)

const float spikyFactor2D = -30.0 / (PI * pow(radiusEffect, 5.0));   // grad Spiky en 2D
const float viscLaplacian2D = 40.0 / (PI * pow(radiusEffect, 5.0));

// Helpers kernels (3D)
float W_poly6(float r, float h) {
    if (r >= h) return 0.0;
    float hr2 = h*h - r*r;
    return (315.0 / (128.0 * PI * pow(h, 9.0))) * hr2*hr2*hr2;
}

float gradW_spiky2D(float dist, float radiusEffect) {
    if (dist <= 0.0 || dist >= radiusEffect) return 0.0;
    float f = radiusEffect * radiusEffect - dist * dist;
    float coef = -24.0 / (PI * pow(radiusEffect, 8));
    return coef * dist * f * f;
}

float gradW_spiky(float dist, float h) {
    if (dist <= 0.0 || dist >= h) return 0.0;
    float coef = -45.0 / (PI * pow(h, 6.0));
    return coef * (h - dist) * (h - dist);
}



float laplacianW_visc(float r, float h) {
    if (r >= h) return 0.0;
    return 45.0 / (PI * pow(h, 6.0)) * (h - r);
}


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

// Retourne la distance signée entre un point p et un triangle (a,b,c)
// signe basé sur la normale du triangle
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

int pointCrossTriangle(vec3 position, vec3 nextPos, vec3 a, vec3 b, vec3 c, vec3 normal)
{
    float denom = dot(normal, nextPos - position);
    if (abs(denom) < 1e-6) return 0;

    float t = dot(normal, a - position) / denom;
    if (t < 0.0 || t > 1.0) return 0;

    vec3 p = position + t * (nextPos - position);

    if (pointInTriangle(p, a, b, c) == 1)
        return 1;
    return 0;
}

void main() {
    uint i = gl_GlobalInvocationID.x;
    if (i >= nbMolecules) return;

    vec3 direction = molecules[i].direction.xyz;
    vec3 position = molecules[i].position.xyz;

    int xIdx = int(position.x / cellSize);
    int yIdx = int(position.y / cellSize);
    int zIdx = int(position.z / cellSize);

    if (yIdx >= yHitBox || zIdx >= zHitBox || xIdx >= xHitBox || yIdx < 0 || zIdx < 0 || xIdx < 0) {
        molecules[i].color = vec4(1, 1, 1, 0.0);
        //molecules[i].position = vec4(44.0 + 0.1 * (xIdx % 20), 1.0 + 2.1 * ((xIdx + zIdx) % 20), 30.0 + 0.1 * (zIdx % 20), 0);
        molecules[i].direction = vec4(0);
        return;
    }
    int gridIndex = xIdx * yHitBox * zHitBox + yIdx * zHitBox + zIdx;

    float density = densities[i];
    float pressure = pressures[i];

    vec3 pressureForce = vec3(0.);
    vec3 viscosityForce = vec3(0.);
    vec3 totalForce = vec3(0.);

    int dxP = position.x - xIdx * cellSize > cellSize / 2. ? 1 : 0;
    int dyP = position.y - yIdx * cellSize > cellSize / 2. ? 1 : 0;
    int dzP = position.z - zIdx * cellSize > cellSize / 2. ? 1 : 0;

    float alone = 0.997;

    for (int dx = -1 + dxP; dx <= dxP; dx += 1) {
        for (int dy = -1 + dyP; dy <= dyP; dy += 1) {
            for (int dz = -1 + dzP; dz <= dzP; dz += 1) {
                int neighborCellIndex = (xIdx + dx) * yHitBox * zHitBox + (yIdx + dy) * zHitBox + (zIdx + dz);

                int molPerCell = nbMolPerCell[neighborCellIndex];
                int molTraveled = min(molPerCell, 64);

                for (int x = 0; x < molTraveled; x += 1) {
                    int neighborIndex = grids[neighborCellIndex * 64 + x];
                    vec3 rij = molecules[neighborIndex].position.xyz - position;
                    float dist = length(rij);

                    if (dist > 0.0 && dist < radiusEffect) {
                        alone = 1.0;
                        float neighborPressure = pressures[neighborIndex];
                        float neighborDensity = densities[neighborIndex];
                        
                        vec3 vel_j = molecules[neighborIndex].direction.xyz;

                        float hr = (radiusEffect - dist);
                        float gradW_coeff = spikyFactor * hr * hr;
                        vec3 gradW = gradW_coeff * (rij / dist);

                        pressureForce -= mass * ((pressure / density * density) + (neighborPressure / neighborDensity * neighborDensity)) * gradW;

                        float laplacianW = viscLaplacian * hr;
                        viscosityForce += viscosity * mass * (vel_j - direction) / neighborDensity * laplacianW;
                    }
                }
            }
        }
    }
    totalForce = (pressureForce + viscosityForce) / max(density, 1e-6) + gravity;


    vec3 origDir = direction + totalForce * deltaTime;
    position -= direction * deltaTime;
    vec3 nextPos = position + origDir * deltaTime;

    vec3 normals[3];
    int nbNormals = 0;

    int lastTriangle = int(molecules[i].direction.w);
    molecules[i].direction.w = -1;

    for (int u = 0; nbNormals < 3; u += 1) {
        int t = gridsHitbox[gridIndex * 64 + u];
        if (t == -1)
            break;
        vec3 a = triangles[t].a;
        vec3 b = triangles[t].b;
        vec3 c = triangles[t].c;
        vec3 normal = triangles[t].normal;

        float d = distancePointTriangle(position, a, b, c, normal);
        if (d < 0.0)
            normal *= -1.0;
        if (abs(d) <= radius || pointCrossTriangle(position, nextPos, a, b, c, normal) == 1) {
            bool similar = false;
            for (int k = 0; k < nbNormals; k += 1) {
                if (abs(dot(normals[k], normal)) > 0.999) { similar = true; break; }
            }
            if (!similar) {
                normals[nbNormals] = normal;
                nbNormals++;
                if (t != lastTriangle)
                    molecules[i].direction.w = float(t);
            }
        }
    }

    vec3 newDir = origDir;

    if (nbNormals == 1) {
        vec3 normal = normals[0];

        if (abs(dot(origDir, normal)) > 0.90) {
            newDir = normalize(origDir - dot(origDir, normal) * normal) * length(direction.xyz) * 0.99;
            if (lastTriangle != -1)
                newDir = normalize((origDir - triangles[lastTriangle].normal) - dot((origDir - triangles[lastTriangle].normal), normal) * normal) * length(direction) * 0.99;
        }
        else {
            newDir = origDir - dot(origDir, normal) * normal;
            newDir = normalize(newDir + normal * 0.1) * length(newDir) * alone;
        }
    } else
    if (nbNormals == 2) {
        vec3 normal = normals[0];
        vec3 normal2 = normals[1];
        if (dot(normal, origDir) < dot(origDir, normals[1])) {
            normal = normals[1];
            normal2 = normals[0];
        }
        newDir = normal * length(direction) * 0.9;
        newDir = newDir - dot(newDir, normal2) * normal2;
        newDir = newDir * 0.9 + normal2 * 0.1;
    }
    if (nbNormals == 3) {
        vec3 n0 = normalize(normals[0] + normals[1] + normals[2]);
        newDir = n0 * 0.9;
    }
    for (int i = 0; i < nbCaptors; i += 1) {
        if (length(position - vec3(captors[i].position.x, position.y, captors[i].position.z)) < 1.0) {
            atomicOr(collisionFlag, 1 << i);
        }
    }
    molecules[i].direction = vec4(newDir, molecules[i].direction.w);

    molecules[i].position.xyz = position + molecules[i].direction.xyz * deltaTime;
}