#ifndef MESH_H
#define MESH_H
#include <string>
#include <limits> // Required for std::numeric_limits
class Mesh {
public:
    Mesh();
    ~Mesh();
    
    bool loadOFF(const std::string& filename);
    // --- Getters ---
    float* getVertices() const { return vertices; }
    float* getColors() const { return nullptr; }
    float* getNormals() const { return normals; }
    unsigned int* getIndices() const { return indices; }
    float* getMinBoundingBox() const { return minBoundingBox; }
    float* getMaxBoundingBox() const { return maxBoundingBox; }
    int getNumVertices() const { return numVertices; }
    int getNumIndices() const { return numIndices; }
    void setData(float* vertices, unsigned int* indices, float* colors, float* normals,
        int numVerts, int numInds);

    // Method to clear mesh data
    void clear();
    // --- Calculation Functions ---
    void calculateBoundingBox();
    void calculateColors();
    void calculateNormals();
private:
    float* vertices;
    unsigned int* indices;
    float* colors;
    float* normals;
    float* minBoundingBox; // Stores {minX, minY, minZ}
    float* maxBoundingBox; // Stores {maxX, maxY, maxZ}
    int numVertices;
    int numIndices;
};
#endif
