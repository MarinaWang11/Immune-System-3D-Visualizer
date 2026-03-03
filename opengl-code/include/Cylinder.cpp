#include "../include/Cylinder.h"
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Cylinder::Cylinder(int sectorCount) {
    std::vector<float> vertices;
    float radius = 0.5f;     // Diâmetro = 1.0f
    float halfHeight = 0.5f; // Altura = 1.0f
    float sectorStep = 2 * M_PI / sectorCount;

    // CORPO LATERAL DO CILINDRO
    for(int i = 0; i < sectorCount; ++i) {
        float t1 = i * sectorStep;
        float t2 = (i + 1) * sectorStep;

        float x1 = radius * cos(t1); float z1 = radius * sin(t1);
        float x2 = radius * cos(t2); float z2 = radius * sin(t2);

        // Normais (fora do cilindro)
        float nx1 = cos(t1); float nz1 = sin(t1);
        float nx2 = cos(t2); float nz2 = sin(t2);

        vertices.insert(vertices.end(), {x1, halfHeight, z1, nx1, 0.0f, nz1});
        vertices.insert(vertices.end(), {x1, -halfHeight, z1, nx1, 0.0f, nz1});
        vertices.insert(vertices.end(), {x2, -halfHeight, z2, nx2, 0.0f, nz2});
               
        vertices.insert(vertices.end(), {x1, halfHeight, z1, nx1, 0.0f, nz1});
        vertices.insert(vertices.end(), {x2, -halfHeight, z2, nx2, 0.0f, nz2});
        vertices.insert(vertices.end(), {x2, halfHeight, z2, nx2, 0.0f, nz2});
    }

    // TAMPA SUPERIOR 
    for(int i = 0; i < sectorCount; ++i) {
        float t1 = i * sectorStep;
        float t2 = (i + 1) * sectorStep;
        float x1 = radius * cos(t1); float z1 = radius * sin(t1);
        float x2 = radius * cos(t2); float z2 = radius * sin(t2);

        // O centro aponta para cima 
        vertices.insert(vertices.end(), {0.0f, halfHeight, 0.0f, 0.0f, 1.0f, 0.0f});
        vertices.insert(vertices.end(), {x1, halfHeight, z1, 0.0f, 1.0f, 0.0f});
        vertices.insert(vertices.end(), {x2, halfHeight, z2, 0.0f, 1.0f, 0.0f});
    }

    // TAMPA INFERIOR 
    for(int i = 0; i < sectorCount; ++i) {
        float t1 = i * sectorStep;
        float t2 = (i + 1) * sectorStep;
        float x1 = radius * cos(t1); float z1 = radius * sin(t1);
        float x2 = radius * cos(t2); float z2 = radius * sin(t2);

        // O centro aponta para baixo 
        vertices.insert(vertices.end(), {0.0f, -halfHeight, 0.0f, 0.0f, -1.0f, 0.0f});
        vertices.insert(vertices.end(), {x2, -halfHeight, z2, 0.0f, -1.0f, 0.0f});
        vertices.insert(vertices.end(), {x1, -halfHeight, z1, 0.0f, -1.0f, 0.0f});
    }

    vertexCount = vertices.size() / 6; // 6 floats por vértice (3 pos + 3 normal)

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Cylinder::~Cylinder() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}

void Cylinder::draw() {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}