#ifndef CYLINDER_H
#define CYLINDER_H

#include <glad/glad.h>

class Cylinder {
public:
    unsigned int VAO, VBO;
    int vertexCount;

    Cylinder(int sectorCount = 36);
    ~Cylinder();
    
    void draw();
};

#endif