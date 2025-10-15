// ///////////////////////////////////////////////////////////////////////////////
// // Sphere.cpp
// // ==========
// // Sphere for OpenGL with (radius, sectors, stacks)
// // The min number of sectors is 3 and the min number of stacks are 2.
// // The default up axis is +Z axis. You can change the up axis with setUpAxis():
// // X=1, Y=2, Z=3.
// //
// //  AUTHOR: Song Ho Ahn (song.ahn@gmail.com)
// // CREATED: 2017-11-01
// // UPDATED: 2023-03-11
// ///////////////////////////////////////////////////////////////////////////////

// #ifdef _WIN32
// #include <windows.h>    // include windows.h to avoid thousands of compile errors even though this class is not depending on Windows
// #endif

// #ifdef __APPLE__
// #include <OpenGL/gl.h>
// #else
// #include <GL/gl.h>
// #endif

// #include <iostream>
// #include <iomanip>
// #include <cmath>
// #include "Sphere.h"



// // constants //////////////////////////////////////////////////////////////////
// const int MIN_SECTOR_COUNT = 3;
// const int MIN_STACK_COUNT  = 2;



// ///////////////////////////////////////////////////////////////////////////////
// // ctor
// ///////////////////////////////////////////////////////////////////////////////
// Sphere::Sphere(float radius, int sectors, int stacks, bool smooth, int up) : interleavedStride(32)
// {
//     set(radius, sectors, stacks, smooth, up);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // setters
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::set(float radius, int sectors, int stacks, bool smooth, int up)
// {
//     if(radius > 0)
//         this->radius = radius;
//     this->sectorCount = sectors;
//     if(sectors < MIN_SECTOR_COUNT)
//         this->sectorCount = MIN_SECTOR_COUNT;
//     this->stackCount = stacks;
//     if(stacks < MIN_STACK_COUNT)
//         this->stackCount = MIN_STACK_COUNT;
//     this->smooth = smooth;
//     this->upAxis = up;
//     if(up < 1 || up > 3)
//         this->upAxis = 3;

//     if(smooth)
//         buildVerticesSmooth();
//     else
//         buildVerticesFlat();
// }

// void Sphere::setRadius(float radius)
// {
//     if(radius != this->radius)
//         set(radius, sectorCount, stackCount, smooth, upAxis);
// }

// void Sphere::setSectorCount(int sectors)
// {
//     if(sectors != this->sectorCount)
//         set(radius, sectors, stackCount, smooth, upAxis);
// }

// void Sphere::setStackCount(int stacks)
// {
//     if(stacks != this->stackCount)
//         set(radius, sectorCount, stacks, smooth, upAxis);
// }

// void Sphere::setSmooth(bool smooth)
// {
//     if(this->smooth == smooth)
//         return;

//     this->smooth = smooth;
//     if(smooth)
//         buildVerticesSmooth();
//     else
//         buildVerticesFlat();
// }

// void Sphere::setUpAxis(int up)
// {
//     if(this->upAxis == up || up < 1 || up > 3)
//         return;

//     changeUpAxis(this->upAxis, up);
//     this->upAxis = up;
// }



// ///////////////////////////////////////////////////////////////////////////////
// // flip the face normals to opposite directions
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::reverseNormals()
// {
//     std::size_t i, j;
//     std::size_t count = normals.size();
//     for(i = 0, j = 3; i < count; i+=3, j+=8)
//     {
//         normals[i]   *= -1;
//         normals[i+1] *= -1;
//         normals[i+2] *= -1;

//         // update interleaved array
//         interleavedVertices[j]   = normals[i];
//         interleavedVertices[j+1] = normals[i+1];
//         interleavedVertices[j+2] = normals[i+2];
//     }

//     // also reverse triangle windings
//     unsigned int tmp;
//     count = indices.size();
//     for(i = 0; i < count; i+=3)
//     {
//         tmp = indices[i];
//         indices[i]   = indices[i+2];
//         indices[i+2] = tmp;
//     }
// }



// ///////////////////////////////////////////////////////////////////////////////
// // print itself
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::printSelf() const
// {
//     std::cout << "===== Sphere =====\n"
//               << "        Radius: " << radius << "\n"
//               << "  Sector Count: " << sectorCount << "\n"
//               << "   Stack Count: " << stackCount << "\n"
//               << "Smooth Shading: " << (smooth ? "true" : "false") << "\n"
//               << "       Up Axis: " << (upAxis == 1 ? "X" : (upAxis == 2 ? "Y" : "Z")) << "\n"
//               << "Triangle Count: " << getTriangleCount() << "\n"
//               << "   Index Count: " << getIndexCount() << "\n"
//               << "  Vertex Count: " << getVertexCount() << "\n"
//               << "  Normal Count: " << getNormalCount() << "\n"
//               << "TexCoord Count: " << getTexCoordCount() << std::endl;
// }



// ///////////////////////////////////////////////////////////////////////////////
// // draw a sphere in VertexArray mode
// // OpenGL RC must be set before calling it
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::draw() const
// {
//     // interleaved array
//     glEnableClientState(GL_VERTEX_ARRAY);
//     glEnableClientState(GL_NORMAL_ARRAY);
//     glEnableClientState(GL_TEXTURE_COORD_ARRAY);
//     glVertexPointer(3, GL_FLOAT, interleavedStride, &interleavedVertices[0]);
//     glNormalPointer(GL_FLOAT, interleavedStride, &interleavedVertices[3]);
//     glTexCoordPointer(2, GL_FLOAT, interleavedStride, &interleavedVertices[6]);

//     glDrawElements(GL_TRIANGLES, (unsigned int)indices.size(), GL_UNSIGNED_INT, indices.data());

//     glDisableClientState(GL_VERTEX_ARRAY);
//     glDisableClientState(GL_NORMAL_ARRAY);
//     glDisableClientState(GL_TEXTURE_COORD_ARRAY);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // draw lines only
// // the caller must set the line width before call this
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::drawLines(const float lineColor[4]) const
// {
//     // set line colour
//     glColor4fv(lineColor);
//     glMaterialfv(GL_FRONT, GL_DIFFUSE,   lineColor);

//     // draw lines with VA
//     glDisable(GL_LIGHTING);
//     glDisable(GL_TEXTURE_2D);
//     glEnableClientState(GL_VERTEX_ARRAY);
//     glVertexPointer(3, GL_FLOAT, 0, vertices.data());

//     glDrawElements(GL_LINES, (unsigned int)lineIndices.size(), GL_UNSIGNED_INT, lineIndices.data());

//     glDisableClientState(GL_VERTEX_ARRAY);
//     glEnable(GL_LIGHTING);
//     glEnable(GL_TEXTURE_2D);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // draw a sphere surfaces and lines on top of it
// // the caller must set the line width before call this
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::drawWithLines(const float lineColor[4]) const
// {
//     glEnable(GL_POLYGON_OFFSET_FILL);
//     glPolygonOffset(1.0, 1.0f); // move polygon backward
//     this->draw();
//     glDisable(GL_POLYGON_OFFSET_FILL);

//     // draw lines with VA
//     drawLines(lineColor);
// }



// /*@@ FIXME: when the radius  = 0
// ///////////////////////////////////////////////////////////////////////////////
// // update vertex positions only
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::updateRadius()
// {
//     float scale = sqrtf(radius * radius / (vertices[0] * vertices[0] + vertices[1] * vertices[1] + vertices[2] * vertices[2]));

//     std::size_t i, j;
//     std::size_t count = vertices.size();
//     for(i = 0, j = 0; i < count; i += 3, j += 8)
//     {
//         vertices[i]   *= scale;
//         vertices[i+1] *= scale;
//         vertices[i+2] *= scale;

//         // for interleaved array
//         interleavedVertices[j]   *= scale;
//         interleavedVertices[j+1] *= scale;
//         interleavedVertices[j+2] *= scale;
//     }
// }
// */



// ///////////////////////////////////////////////////////////////////////////////
// // dealloc vectors
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::clearArrays()
// {
//     std::vector<float>().swap(vertices);
//     std::vector<float>().swap(normals);
//     std::vector<float>().swap(texCoords);
//     std::vector<unsigned int>().swap(indices);
//     std::vector<unsigned int>().swap(lineIndices);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // build vertices of sphere with smooth shading using parametric equation
// // x = r * cos(u) * cos(v)
// // y = r * cos(u) * sin(v)
// // z = r * sin(u)
// // where u: stack(latitude) angle (-90 <= u <= 90)
// //       v: sector(longitude) angle (0 <= v <= 360)
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::buildVerticesSmooth()
// {
//     const float PI = acos(-1.0f);

//     // clear memory of prev arrays
//     clearArrays();

//     float x, y, z, xy;                              // vertex position
//     float nx, ny, nz, lengthInv = 1.0f / radius;    // normal
//     float s, t;                                     // texCoord

//     float sectorStep = 2 * PI / sectorCount;
//     float stackStep = PI / stackCount;
//     float sectorAngle, stackAngle;

//     for(int i = 0; i <= stackCount; ++i)
//     {
//         stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
//         xy = radius * cosf(stackAngle);             // r * cos(u)
//         z = radius * sinf(stackAngle);              // r * sin(u)

//         // add (sectorCount+1) vertices per stack
//         // the first and last vertices have same position and normal, but different tex coords
//         for(int j = 0; j <= sectorCount; ++j)
//         {
//             sectorAngle = j * sectorStep;           // starting from 0 to 2pi

//             // vertex position
//             x = xy * cosf(sectorAngle);             // r * cos(u) * cos(v)
//             y = xy * sinf(sectorAngle);             // r * cos(u) * sin(v)
//             addVertex(x, y, z);

//             // normalized vertex normal
//             nx = x * lengthInv;
//             ny = y * lengthInv;
//             nz = z * lengthInv;
//             addNormal(nx, ny, nz);

//             // vertex tex coord between [0, 1]
//             s = (float)j / sectorCount;
//             t = (float)i / stackCount;
//             addTexCoord(s, t);
//         }
//     }

//     // indices
//     //  k1--k1+1
//     //  |  / |
//     //  | /  |
//     //  k2--k2+1
//     unsigned int k1, k2;
//     for(int i = 0; i < stackCount; ++i)
//     {
//         k1 = i * (sectorCount + 1);     // beginning of current stack
//         k2 = k1 + sectorCount + 1;      // beginning of next stack

//         for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
//         {
//             // 2 triangles per sector excluding 1st and last stacks
//             if(i != 0)
//             {
//                 addIndices(k1, k2, k1+1);   // k1---k2---k1+1
//             }

//             if(i != (stackCount-1))
//             {
//                 addIndices(k1+1, k2, k2+1); // k1+1---k2---k2+1
//             }

//             // vertical lines for all stacks
//             lineIndices.push_back(k1);
//             lineIndices.push_back(k2);
//             if(i != 0)  // horizontal lines except 1st stack
//             {
//                 lineIndices.push_back(k1);
//                 lineIndices.push_back(k1 + 1);
//             }
//         }
//     }

//     // generate interleaved vertex array as well
//     buildInterleavedVertices();

//     // change up axis from Z-axis to the given
//     if(this->upAxis != 3)
//         changeUpAxis(3, this->upAxis);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // generate vertices with flat shading
// // each triangle is independent (no shared vertices)
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::buildVerticesFlat()
// {
//     const float PI = acos(-1.0f);

//     // tmp vertex definition (x,y,z,s,t)
//     struct Vertex
//     {
//         float x, y, z, s, t;
//     };
//     std::vector<Vertex> tmpVertices;

//     float sectorStep = 2 * PI / sectorCount;
//     float stackStep = PI / stackCount;
//     float sectorAngle, stackAngle;

//     // compute all vertices first, each vertex contains (x,y,z,s,t) except normal
//     for(int i = 0; i <= stackCount; ++i)
//     {
//         stackAngle = PI / 2 - i * stackStep;        // starting from pi/2 to -pi/2
//         float xy = radius * cosf(stackAngle);       // r * cos(u)
//         float z = radius * sinf(stackAngle);        // r * sin(u)

//         // add (sectorCount+1) vertices per stack
//         // the first and last vertices have same position and normal, but different tex coords
//         for(int j = 0; j <= sectorCount; ++j)
//         {
//             sectorAngle = j * sectorStep;           // starting from 0 to 2pi

//             Vertex vertex;
//             vertex.x = xy * cosf(sectorAngle);      // x = r * cos(u) * cos(v)
//             vertex.y = xy * sinf(sectorAngle);      // y = r * cos(u) * sin(v)
//             vertex.z = z;                           // z = r * sin(u)
//             vertex.s = (float)j/sectorCount;        // s
//             vertex.t = (float)i/stackCount;         // t
//             tmpVertices.push_back(vertex);
//         }
//     }

//     // clear memory of prev arrays
//     clearArrays();

//     Vertex v1, v2, v3, v4;                          // 4 vertex positions and tex coords
//     std::vector<float> n;                           // 1 face normal

//     int i, j, k, vi1, vi2;
//     int index = 0;                                  // index for vertex
//     for(i = 0; i < stackCount; ++i)
//     {
//         vi1 = i * (sectorCount + 1);                // index of tmpVertices
//         vi2 = (i + 1) * (sectorCount + 1);

//         for(j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
//         {
//             // get 4 vertices per sector
//             //  v1--v3
//             //  |    |
//             //  v2--v4
//             v1 = tmpVertices[vi1];
//             v2 = tmpVertices[vi2];
//             v3 = tmpVertices[vi1 + 1];
//             v4 = tmpVertices[vi2 + 1];

//             // if 1st stack and last stack, store only 1 triangle per sector
//             // otherwise, store 2 triangles (quad) per sector
//             if(i == 0) // a triangle for first stack ==========================
//             {
//                 // put a triangle
//                 addVertex(v1.x, v1.y, v1.z);
//                 addVertex(v2.x, v2.y, v2.z);
//                 addVertex(v4.x, v4.y, v4.z);

//                 // put tex coords of triangle
//                 addTexCoord(v1.s, v1.t);
//                 addTexCoord(v2.s, v2.t);
//                 addTexCoord(v4.s, v4.t);

//                 // put normal
//                 n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v4.x,v4.y,v4.z);
//                 for(k = 0; k < 3; ++k)  // same normals for 3 vertices
//                 {
//                     addNormal(n[0], n[1], n[2]);
//                 }

//                 // put indices of 1 triangle
//                 addIndices(index, index+1, index+2);

//                 // indices for line (first stack requires only vertical line)
//                 lineIndices.push_back(index);
//                 lineIndices.push_back(index+1);

//                 index += 3;     // for next
//             }
//             else if(i == (stackCount-1)) // a triangle for last stack =========
//             {
//                 // put a triangle
//                 addVertex(v1.x, v1.y, v1.z);
//                 addVertex(v2.x, v2.y, v2.z);
//                 addVertex(v3.x, v3.y, v3.z);

//                 // put tex coords of triangle
//                 addTexCoord(v1.s, v1.t);
//                 addTexCoord(v2.s, v2.t);
//                 addTexCoord(v3.s, v3.t);

//                 // put normal
//                 n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v3.x,v3.y,v3.z);
//                 for(k = 0; k < 3; ++k)  // same normals for 3 vertices
//                 {
//                     addNormal(n[0], n[1], n[2]);
//                 }

//                 // put indices of 1 triangle
//                 addIndices(index, index+1, index+2);

//                 // indices for lines (last stack requires both vert/hori lines)
//                 lineIndices.push_back(index);
//                 lineIndices.push_back(index+1);
//                 lineIndices.push_back(index);
//                 lineIndices.push_back(index+2);

//                 index += 3;     // for next
//             }
//             else // 2 triangles for others ====================================
//             {
//                 // put quad vertices: v1-v2-v3-v4
//                 addVertex(v1.x, v1.y, v1.z);
//                 addVertex(v2.x, v2.y, v2.z);
//                 addVertex(v3.x, v3.y, v3.z);
//                 addVertex(v4.x, v4.y, v4.z);

//                 // put tex coords of quad
//                 addTexCoord(v1.s, v1.t);
//                 addTexCoord(v2.s, v2.t);
//                 addTexCoord(v3.s, v3.t);
//                 addTexCoord(v4.s, v4.t);

//                 // put normal
//                 n = computeFaceNormal(v1.x,v1.y,v1.z, v2.x,v2.y,v2.z, v3.x,v3.y,v3.z);
//                 for(k = 0; k < 4; ++k)  // same normals for 4 vertices
//                 {
//                     addNormal(n[0], n[1], n[2]);
//                 }

//                 // put indices of quad (2 triangles)
//                 addIndices(index, index+1, index+2);
//                 addIndices(index+2, index+1, index+3);

//                 // indices for lines
//                 lineIndices.push_back(index);
//                 lineIndices.push_back(index+1);
//                 lineIndices.push_back(index);
//                 lineIndices.push_back(index+2);

//                 index += 4;     // for next
//             }
//         }
//     }

//     // generate interleaved vertex array as well
//     buildInterleavedVertices();

//     // change up axis from Z-axis to the given
//     if(this->upAxis != 3)
//         changeUpAxis(3, this->upAxis);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // generate interleaved vertices: V/N/T
// // stride must be 32 bytes
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::buildInterleavedVertices()
// {
//     std::vector<float>().swap(interleavedVertices);

//     std::size_t i, j;
//     std::size_t count = vertices.size();
//     for(i = 0, j = 0; i < count; i += 3, j += 2)
//     {
//         interleavedVertices.push_back(vertices[i]);
//         interleavedVertices.push_back(vertices[i+1]);
//         interleavedVertices.push_back(vertices[i+2]);

//         interleavedVertices.push_back(normals[i]);
//         interleavedVertices.push_back(normals[i+1]);
//         interleavedVertices.push_back(normals[i+2]);

//         interleavedVertices.push_back(texCoords[j]);
//         interleavedVertices.push_back(texCoords[j+1]);
//     }
// }



// ///////////////////////////////////////////////////////////////////////////////
// // transform vertex/normal (x,y,z) coords
// // assume from/to values are validated: 1~3 and from != to
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::changeUpAxis(int from, int to)
// {
//     // initial transform matrix cols
//     float tx[] = {1.0f, 0.0f, 0.0f};    // x-axis (left)
//     float ty[] = {0.0f, 1.0f, 0.0f};    // y-axis (up)
//     float tz[] = {0.0f, 0.0f, 1.0f};    // z-axis (forward)

//     // X -> Y
//     if(from == 1 && to == 2)
//     {
//         tx[0] =  0.0f; tx[1] =  1.0f;
//         ty[0] = -1.0f; ty[1] =  0.0f;
//     }
//     // X -> Z
//     else if(from == 1 && to == 3)
//     {
//         tx[0] =  0.0f; tx[2] =  1.0f;
//         tz[0] = -1.0f; tz[2] =  0.0f;
//     }
//     // Y -> X
//     else if(from == 2 && to == 1)
//     {
//         tx[0] =  0.0f; tx[1] = -1.0f;
//         ty[0] =  1.0f; ty[1] =  0.0f;
//     }
//     // Y -> Z
//     else if(from == 2 && to == 3)
//     {
//         ty[1] =  0.0f; ty[2] =  1.0f;
//         tz[1] = -1.0f; tz[2] =  0.0f;
//     }
//     //  Z -> X
//     else if(from == 3 && to == 1)
//     {
//         tx[0] =  0.0f; tx[2] = -1.0f;
//         tz[0] =  1.0f; tz[2] =  0.0f;
//     }
//     // Z -> Y
//     else
//     {
//         ty[1] =  0.0f; ty[2] = -1.0f;
//         tz[1] =  1.0f; tz[2] =  0.0f;
//     }

//     std::size_t i, j;
//     std::size_t count = vertices.size();
//     float vx, vy, vz;
//     float nx, ny, nz;
//     for(i = 0, j = 0; i < count; i += 3, j += 8)
//     {
//         // transform vertices
//         vx = vertices[i];
//         vy = vertices[i+1];
//         vz = vertices[i+2];
//         vertices[i]   = tx[0] * vx + ty[0] * vy + tz[0] * vz;   // x
//         vertices[i+1] = tx[1] * vx + ty[1] * vy + tz[1] * vz;   // y
//         vertices[i+2] = tx[2] * vx + ty[2] * vy + tz[2] * vz;   // z

//         // transform normals
//         nx = normals[i];
//         ny = normals[i+1];
//         nz = normals[i+2];
//         normals[i]   = tx[0] * nx + ty[0] * ny + tz[0] * nz;   // nx
//         normals[i+1] = tx[1] * nx + ty[1] * ny + tz[1] * nz;   // ny
//         normals[i+2] = tx[2] * nx + ty[2] * ny + tz[2] * nz;   // nz

//         // trnasform interleaved array
//         interleavedVertices[j]   = vertices[i];
//         interleavedVertices[j+1] = vertices[i+1];
//         interleavedVertices[j+2] = vertices[i+2];
//         interleavedVertices[j+3] = normals[i];
//         interleavedVertices[j+4] = normals[i+1];
//         interleavedVertices[j+5] = normals[i+2];
//     }
// }



// ///////////////////////////////////////////////////////////////////////////////
// // add single vertex to array
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::addVertex(float x, float y, float z)
// {
//     vertices.push_back(x);
//     vertices.push_back(y);
//     vertices.push_back(z);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // add single normal to array
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::addNormal(float nx, float ny, float nz)
// {
//     normals.push_back(nx);
//     normals.push_back(ny);
//     normals.push_back(nz);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // add single texture coord to array
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::addTexCoord(float s, float t)
// {
//     texCoords.push_back(s);
//     texCoords.push_back(t);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // add 3 indices to array
// ///////////////////////////////////////////////////////////////////////////////
// void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3)
// {
//     indices.push_back(i1);
//     indices.push_back(i2);
//     indices.push_back(i3);
// }



// ///////////////////////////////////////////////////////////////////////////////
// // return face normal of a triangle v1-v2-v3
// // if a triangle has no surface (normal length = 0), then return a zero vector
// ///////////////////////////////////////////////////////////////////////////////
// std::vector<float> Sphere::computeFaceNormal(float x1, float y1, float z1,  // v1
//                                              float x2, float y2, float z2,  // v2
//                                              float x3, float y3, float z3)  // v3
// {
//     const float EPSILON = 0.000001f;

//     std::vector<float> normal(3, 0.0f);     // default return value (0,0,0)
//     float nx, ny, nz;

//     // find 2 edge vectors: v1-v2, v1-v3
//     float ex1 = x2 - x1;
//     float ey1 = y2 - y1;
//     float ez1 = z2 - z1;
//     float ex2 = x3 - x1;
//     float ey2 = y3 - y1;
//     float ez2 = z3 - z1;

//     // cross product: e1 x e2
//     nx = ey1 * ez2 - ez1 * ey2;
//     ny = ez1 * ex2 - ex1 * ez2;
//     nz = ex1 * ey2 - ey1 * ex2;

//     // normalize only if the length is > 0
//     float length = sqrtf(nx * nx + ny * ny + nz * nz);
//     if(length > EPSILON)
//     {
//         // normalize
//         float lengthInv = 1.0f / length;
//         normal[0] = nx * lengthInv;
//         normal[1] = ny * lengthInv;
//         normal[2] = nz * lengthInv;
//     }

//     return normal;
// }

#include <glad/glad.h> 

#include <iostream>
#include <iomanip>
#include <cmath> 
#include "Sphere.h" 

// constantes
const int MIN_SECTOR_COUNT = 3; 
const int MIN_STACK_COUNT = 2;  

Sphere::Sphere(float radius, int sectors, int stacks, bool smooth, int up) : interleavedStride(32)
{
    set(radius, sectors, stacks, smooth, up); 
}

void Sphere::set(float radius, int sectors, int stacks, bool smooth, int up)
{
    // 1. Armazena e valida os parâmetros.
    this->radius = radius;
    this->sectorCount = sectors;
    if(sectors < MIN_SECTOR_COUNT)
        this->sectorCount = MIN_SECTOR_COUNT; // mínimo de setores.
    this->stackCount = stacks;
    if(stacks < MIN_STACK_COUNT)
        this->stackCount = MIN_STACK_COUNT; // mínimo de pilhas.
    this->smooth = smooth;
    this->upAxis = up; // Eixo 'para cima

    if(smooth)
        buildVerticesSmooth(); // Gera vértices com normais para sombreamento suave (Gouraud/Phong).
    else
        buildVerticesFlat();   // Gera vértices com normais de face para sombreamento plano (facetado).

    setupMesh();
}

// Método de renderização: Vincula o VAO e desenha a malha.
void Sphere::draw() const
{
    glBindVertexArray(VAO); // Vincula o Vertex Array Object (VAO).
    // glDrawElements desenha triângulos usando o Element Buffer Object (EBO).
 
    glDrawElements(GL_TRIANGLES, getIndexCount(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0); // Desvincula o VAO.
}

// Libera a memória ocupada pelos vetores de dados da malha.
void Sphere::clearArrays()
{
    // Usa a técnica swap para liberar a memória 
    std::vector<float>().swap(vertices);
    std::vector<float>().swap(normals);
    std::vector<float>().swap(texCoords);
    std::vector<unsigned int>().swap(indices);
    std::vector<unsigned int>().swap(lineIndices); 
}

// Constrói a malha para sombreamento suave, com vértices e índices compartilhados
void Sphere::buildVerticesSmooth()
{
    const float PI = acos(-1.0f); // Define PI.
    clearArrays();

    float x, y, z, xy; // Coordenadas temporárias
    float nx, ny, nz, lengthInv = 1.0f / radius; // Componentes da Norma
    float s, t; // Coordenadas de Textura (UV)

    // Calcula os incrementos angulares.
    float sectorStep = 2 * PI / sectorCount; // Incremento de longitude 
    float stackStep = PI / stackCount;       // Incremento de latitude 
    float sectorAngle, stackAngle;

    // 1. Geração de Vértices, Normais e Coordenadas de Textura.
    // O laço externo itera sobre as pilhas (latitude).
    for(int i = 0; i <= stackCount; ++i)
    {
        // 'stackAngle' varia de PI/2 (Polo Norte) a -PI/2 (Polo Sul).
        stackAngle = PI / 2 - i * stackStep;
        xy = radius * cosf(stackAngle); // Raio do círculo da pilha atual no plano XY.
        z = radius * sinf(stackAngle);  // Coordenada Z (altura) do círculo.

        // O laço interno itera sobre os setores (longitude).
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep; // Ângulo de longitude.

            // a. Posição
            x = xy * cosf(sectorAngle); // Conversão de polar para cartesiano.
            y = xy * sinf(sectorAngle);
            addVertex(x, y, z);

            // b. Normal 
            nx = x * lengthInv; // = x / radius
            ny = y * lengthInv; // = y / radius
            nz = z * lengthInv; // = z / radius
            addNormal(nx, ny, nz);

            // c. Coordenadas de Textura (UV).
            s = (float)j / sectorCount; // s (horizontal) vai de 0 a 1.
            t = (float)i / stackCount;   // t (vertical) vai de 0 a 1.
            addTexCoord(s, t);
        }
    }

    // 2. Geração de Índices (cria a malha de triângulos).
    unsigned int k1, k2; // Índices para a pilha atual (k1) e próxima pilha (k2).
    for(int i = 0; i < stackCount; ++i)
    {
        k1 = i * (sectorCount + 1);      // Índice inicial da pilha atual.
        k2 = k1 + sectorCount + 1;       // Índice inicial da próxima pilha.

        for(int j = 0; j < sectorCount; ++j, ++k1, ++k2)
        {
            // O quad (retângulo) é formado pelos vértices: k1, k1+1, k2, k2+1.
            
            // Primeiro triângulo do quad: (k1, k2, k1+1).
            // Ignorado no Polo Norte (i=0) pois k1 e k1+1 são o mesmo ponto.
            if(i != 0)
            {
                addIndices(k1, k2, k1 + 1); // Triângulo superior.
            }
            
            // Segundo triângulo do quad: (k1+1, k2, k2+1).
            // Ignorado no Polo Sul (i=stackCount-1) pois k2 e k2+1 são o mesmo ponto.
            if(i != (stackCount - 1))
            {
                addIndices(k1 + 1, k2, k2 + 1); // Triângulo inferior.
            }
        }
    }
    
    // 3. Prepara o vetor final com dados intercalados para a GPU.
    buildInterleavedVertices();
}

// Constrói a malha para sombreamento plano, duplicando vértices e usando normais de face.
void Sphere::buildVerticesFlat()
{
    const float PI = acos(-1.0f);
    // Estrutura para armazenar temporariamente os pontos do grid (posições e UVs).
    struct Vertex { float x, y, z, s, t; };
    std::vector<Vertex> tmpVertices;

    float sectorStep = 2 * PI / sectorCount;
    float stackStep = PI / stackCount;
    float sectorAngle, stackAngle;

    // 1. Gera o grid inicial de pontos (Posição e UVs) 
    for(int i = 0; i <= stackCount; ++i)
    {
        stackAngle = PI / 2 - i * stackStep;
        float xy = radius * cosf(stackAngle);
        float z = radius * sinf(stackAngle);
        for(int j = 0; j <= sectorCount; ++j)
        {
            sectorAngle = j * sectorStep;
            Vertex vertex;
            vertex.x = xy * cosf(sectorAngle);
            vertex.y = xy * sinf(sectorAngle);
            vertex.z = z;
            vertex.s = (float)j / sectorCount;
            vertex.t = (float)i / stackCount;
            tmpVertices.push_back(vertex);
        }
    }

    clearArrays();
    Vertex v1, v2, v3, v4; // Vértices do quad 
    std::vector<float> n;  // Vetor  da normal da face
    int index = 0;         // Índice do próximo vértice
    
    // 2. Itera sobre os quads para gerar os triângulos finais
    for(int i = 0; i < stackCount; ++i)
    {
        int vi1 = i * (sectorCount + 1);     // Índice do topo esquerdo.
        int vi2 = (i + 1) * (sectorCount + 1); // Índice da base esquerda.

        for(int j = 0; j < sectorCount; ++j, ++vi1, ++vi2)
        {
            // Obtém os 4 vértices do quad no grid temporário.
            v1 = tmpVertices[vi1];
            v2 = tmpVertices[vi2];
            v3 = tmpVertices[vi1 + 1];
            v4 = tmpVertices[vi2 + 1];

            // Tratamento especial para os triângulos perto do Polo Norte (Topo).
            if(i == 0)
            {
                // Triângulo superior: (v1, v2, v4).
                addVertex(v1.x, v1.y, v1.z);
                addVertex(v2.x, v2.y, v2.z);
                addVertex(v4.x, v4.y, v4.z);
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v4.s, v4.t);
                
                // Calcula a Normal da Face.
                n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v4.x, v4.y, v4.z);
                // ATRIBUI A MESMA NORMAL PARA TODOS OS 3 VÉRTICES (sombreamento plano)
                for(int k = 0; k < 3; ++k) addNormal(n[0], n[1], n[2]); 
                
                addIndices(index, index + 1, index + 2);
                index += 3; // O próximo vértice começará após os 3 novos vértices
            }
            // Tratamento especial para os triângulos perto da base
            else if(i == (stackCount - 1))
            {
                // Triângulo inferior: (v1, v2, v3).
                addVertex(v1.x, v1.y, v1.z);
                addVertex(v2.x, v2.y, v2.z);
                addVertex(v3.x, v3.y, v3.z);
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v3.s, v3.t);
                
                // Calcula a Normal da Face
                n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
                // ATRIBUI A MESMA NORMAL PARA OS 3 VÉRTICES
                for(int k = 0; k < 3; ++k) addNormal(n[0], n[1], n[2]);
                
                addIndices(index, index + 1, index + 2);
                index += 3;
            }
            // Faces regulares no meio 
            else
            {
                // Adiciona os 4 vértices 
                addVertex(v1.x, v1.y, v1.z); // k1
                addVertex(v2.x, v2.y, v2.z); // k2
                addVertex(v3.x, v3.y, v3.z); // k1+1
                addVertex(v4.x, v4.y, v4.z); // k2+1
                addTexCoord(v1.s, v1.t);
                addTexCoord(v2.s, v2.t);
                addTexCoord(v3.s, v3.t);
                addTexCoord(v4.s, v4.t);

                // Normal da Face para o primeiro triângulo (k1, k2, k3).
                n = computeFaceNormal(v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, v3.x, v3.y, v3.z);
                // ATRIBUI A NORMAL CALCULADA PARA OS 4 VÉRTICES.
                for(int k = 0; k < 4; ++k) addNormal(n[0], n[1], n[2]); 
                
                // Índices para o primeiro triângulo: (index, index+1, index+2).
                addIndices(index, index + 1, index + 2); 
                // Índices para o segundo triângulo: (index+2, index+1, index+3).
                addIndices(index + 2, index + 1, index + 3); 
                
                index += 4; // Avança 4 novos vértices no vetor.
            }
        }
    }
    buildInterleavedVertices();
}


void Sphere::buildInterleavedVertices()
{
    std::vector<float>().swap(interleavedVertices); 
    std::size_t count = vertices.size(); // Número de componentes 
    
    for(std::size_t i = 0, j = 0; i < count; i += 3, j += 2)
    {
        // Posição (3 floats)
        interleavedVertices.push_back(vertices[i]);
        interleavedVertices.push_back(vertices[i+1]);
        interleavedVertices.push_back(vertices[i+2]);
        // Normal (3 floats)
        interleavedVertices.push_back(normals[i]);
        interleavedVertices.push_back(normals[i+1]);
        interleavedVertices.push_back(normals[i+2]);
        // Coordenada de Textura (2 floats)
        interleavedVertices.push_back(texCoords[j]);
        interleavedVertices.push_back(texCoords[j+1]);
    }
}

void Sphere::addVertex(float x, float y, float z) { vertices.push_back(x); vertices.push_back(y); vertices.push_back(z); }
void Sphere::addNormal(float nx, float ny, float nz) { normals.push_back(nx); normals.push_back(ny); normals.push_back(nz); }
void Sphere::addTexCoord(float s, float t) { texCoords.push_back(s); texCoords.push_back(t); }
void Sphere::addIndices(unsigned int i1, unsigned int i2, unsigned int i3) { indices.push_back(i1); indices.push_back(i2); indices.push_back(i3); }

// Calcula o vetor normal de uma face triangular definita por 3 pontos.
std::vector<float> Sphere::computeFaceNormal(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3)
{
    const float EPSILON = 0.000001f;
    std::vector<float> normal(3, 0.0f);
    
    // 1. Cria os vetores de aresta e1 e e2
    float ex1 = x2 - x1, ey1 = y2 - y1, ez1 = z2 - z1;
    float ex2 = x3 - x1, ey2 = y3 - y1, ez2 = z3 - z1;
    
    // 2. Calcula o Produto Vetorial de e1 e e2
    // O resultado é um vetor perpendicular 
    float nx = ey1 * ez2 - ez1 * ey2;
    float ny = ez1 * ex2 - ex1 * ez2;
    float nz = ex1 * ey2 - ey1 * ex2;
    
    // 3. Normaliza o vetor resultante 
    float length = sqrtf(nx * nx + ny * ny + nz * nz);
    if(length > EPSILON) 
    {
        float lengthInv = 1.0f / length;
        normal[0] = nx * lengthInv;
        normal[1] = ny * lengthInv;
        normal[2] = nz * lengthInv;
    }
    return normal;
}

// Configura o Vertex Array Object (VAO) e os Buffers de Vértices/Elementos (VBO/EBO)
void Sphere::setupMesh()
{
    // 1. Gera os IDs dos buffers
    glGenVertexArrays(1, &VAO); // Vertex Array Object
    glGenBuffers(1, &VBO);       // Vertex Buffer Object (vértice)
    glGenBuffers(1, &EBO);       // Element Buffer Object (índice)

    // 2. Vincula e envia os dados para a GPU
    glBindVertexArray(VAO);
    
    // VBO (Vertices Intercalados)
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // Envia o vetor 'interleavedVertices' para a GPU.
    glBufferData(GL_ARRAY_BUFFER, getInterleavedVertexSize(), getInterleavedVertices(), GL_STATIC_DRAW);
    
    // EBO (Índices)
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // Envia o vetor 'indices' para a GPU.
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, getIndexSize(), getIndices(), GL_STATIC_DRAW);

    // 3. Define os Atributos de Vértice (layout na memória da GPU).
    const GLsizei stride = interleavedStride; // Stride é o tamanho total de um vértice (32 bytes).

    // Attribute 0: Posição (layout(location = 0))
    glEnableVertexAttribArray(0);
    // 3 floats, sem normalização, stride=32, offset=0 (início do vértice).
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0); 
    
    // Attribute 1: Normal (layout(location = 1))
    glEnableVertexAttribArray(1);
    // 3 floats, offset=3 * sizeof(float) = 12 bytes (após 3 floats de Posição).
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    
    // Attribute 2: Coordenada de Textura (layout(location = 2))
    glEnableVertexAttribArray(2);
    // 2 floats, offset=6 * sizeof(float) = 24 bytes (após 3 Posição + 3 Normal).
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));

    // 4. Desvincula o VAO para evitar modificações acidentais.
    glBindVertexArray(0);
}