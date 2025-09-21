#ifndef MESH_SLICER_H
#define MESH_SLICER_H

#include "Mesh.h"
#include <cmath> 


class Mesh;


struct CuttingPlane {
    float normal[3];    
    float point[3];     

    CuttingPlane() {
        normal[0] = 1.0f; normal[1] = 0.0f; normal[2] = 0.0f;
        point[0] = 0.0f; point[1] = 0.0f; point[2] = 0.0f;
    }

    CuttingPlane(float nx, float ny, float nz, float px, float py, float pz) {
        normal[0] = nx; normal[1] = ny; normal[2] = nz;
        point[0] = px; point[1] = py; point[2] = pz;
        normalize();
    }

    // Normalize the plane normal
    void normalize() {
        float length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (length > 1e-6f) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }
    }

    // Calculate signed distance from a point to the plane
    float distanceToPoint(float x, float y, float z) const {
        return normal[0] * (x - point[0]) +
            normal[1] * (y - point[1]) +
            normal[2] * (z - point[2]);
    }
};


struct Transform {
    float matrix[16];

    Transform() {
        // Initialize as identity matrix
        for (int i = 0; i < 16; i++) matrix[i] = 0.0f;
        matrix[0] = matrix[5] = matrix[10] = matrix[15] = 1.0f;
    }

    
    void transformPoint(float localX, float localY, float localZ,
        float& worldX, float& worldY, float& worldZ) const {
        worldX = matrix[0] * localX + matrix[4] * localY + matrix[8] * localZ + matrix[12];
        worldY = matrix[1] * localX + matrix[5] * localY + matrix[9] * localZ + matrix[13];
        worldZ = matrix[2] * localX + matrix[6] * localY + matrix[10] * localZ + matrix[14];
    }
};


class MeshPartArray {
private:
    Mesh** parts;
    int* partIds;
    int capacity;
    int count;

    void resize() {
        int newCapacity = (capacity == 0) ? 4 : capacity * 2;
        Mesh** newParts = new Mesh * [newCapacity];
        int* newIds = new int[newCapacity];

        for (int i = 0; i < count; i++) {
            newParts[i] = parts[i];
            newIds[i] = partIds[i];
        }

        delete[] parts;
        delete[] partIds;
        parts = newParts;
        partIds = newIds;
        capacity = newCapacity;
    }

public:
    MeshPartArray() : parts(nullptr), partIds(nullptr), capacity(4), count(0) {
        parts = new Mesh * [capacity];
        partIds = new int[capacity];
    }

    ~MeshPartArray() {
        for (int i = 0; i < count; i++) {
            delete parts[i];
        }
        delete[] parts;
        delete[] partIds;
    }

    void add(Mesh* mesh, int id) {
        if (count >= capacity) resize();
        parts[count] = mesh;
        partIds[count] = id;
        count++;
    }

    Mesh* getMesh(int index) const { return (index >= 0 && index < count) ? parts[index] : nullptr; }
    int getId(int index) const { return (index >= 0 && index < count) ? partIds[index] : -1; }
    int getCount() const { return count; }
};

class MeshSlicer {
private:
    
    struct TempVertexArray {
        float* vertices = nullptr;
        float* normals = nullptr;
        unsigned int* indices = nullptr;
        int vertexCount = 0;
        int indexCount = 0;
        int vertexCapacity = 0;
        int indexCapacity = 0;

        TempVertexArray() = default;

        
        TempVertexArray(const TempVertexArray&) = delete;
        TempVertexArray& operator=(const TempVertexArray&) = delete;

        ~TempVertexArray() {
            delete[] vertices;
            delete[] normals;
            delete[] indices;
        }

        void reserve(int vertCapacity, int indCapacity) {
            delete[] vertices;
            delete[] normals;
            delete[] indices;

            vertexCapacity = vertCapacity;
            indexCapacity = indCapacity;

            vertices = (vertexCapacity > 0) ? new float[vertexCapacity * 3] : nullptr;
            normals = (vertexCapacity > 0) ? new float[vertexCapacity * 3] : nullptr;
            indices = (indexCapacity > 0) ? new unsigned int[indexCapacity] : nullptr;

            vertexCount = 0;
            indexCount = 0;
        }

        void addVertex(float x, float y, float z, float nx, float ny, float nz) {
            if (vertexCount >= vertexCapacity) return; // Avoid overflow
            int idx = vertexCount * 3;
            vertices[idx] = x; vertices[idx + 1] = y; vertices[idx + 2] = z;
            normals[idx] = nx; normals[idx + 1] = ny; normals[idx + 2] = nz;
            vertexCount++;
        }

        void addTriangle(unsigned int v0, unsigned int v1, unsigned int v2) {
            if (indexCount + 2 >= indexCapacity) return; // Avoid overflow
            indices[indexCount++] = v0;
            indices[indexCount++] = v1;
            indices[indexCount++] = v2;
        }
    };

    
    struct EdgeIntersection {
        float point[3];
        float normal[3];
        float t = 0.0f; 
        bool valid = false;
    };

public:
    MeshSlicer() = default;
    ~MeshSlicer() = default;

    
    MeshPartArray* sliceMesh(Mesh& originalMesh,
        const Transform& worldTransform,
        const CuttingPlane* cuttingPlanes,
        int numPlanes,
        float separationDistance = 1.0f);

private:
    
    void transformMeshToWorld(const Mesh& mesh, const Transform& transform,
        float* worldVertices);

    EdgeIntersection findEdgeIntersection(const float v0[3], const float v1[3],
        const float n0[3], const float n1[3],
        const CuttingPlane& plane);

    void sliceTriangleWithPlane(const float v0[3], const float v1[3], const float v2[3],
        const float n0[3], const float n1[3], const float n2[3],
        const CuttingPlane& plane,
        TempVertexArray& positivePart,
        TempVertexArray& negativePart);

    void sliceMeshWithSinglePlane(const Mesh& inputMesh,
        const CuttingPlane& plane,
        Mesh*& positivePart,
        Mesh*& negativePart);

    void createMeshFromTempArray(const TempVertexArray& tempArray, Mesh*& mesh);

    void separateParts(MeshPartArray* parts, const float originalCenter[3],
        float separationDistance);

    void calculateMeshCenter(const Mesh& mesh, float center[3]);

    void interpolateVertexData(const float v0[3], const float v1[3], float t,
        const float n0[3], const float n1[3],
        float outVertex[3], float outNormal[3]);
};



inline void MeshSlicer::transformMeshToWorld(const Mesh& mesh, const Transform& transform,
    float* worldVertices) {
    int numVertices = mesh.getNumVertices();
    const float* localVertices = mesh.getVertices();

    for (int i = 0; i < numVertices; i++) {
        transform.transformPoint(localVertices[i * 3], localVertices[i * 3 + 1], localVertices[i * 3 + 2],
            worldVertices[i * 3], worldVertices[i * 3 + 1], worldVertices[i * 3 + 2]);
    }
}

inline void MeshSlicer::calculateMeshCenter(const Mesh& mesh, float center[3]) {
    center[0] = 0.0f; center[1] = 0.0f; center[2] = 0.0f;
    int numVertices = mesh.getNumVertices();
    if (numVertices == 0) return;

    const float* vertices = mesh.getVertices();
    for (int i = 0; i < numVertices; i++) {
        center[0] += vertices[i * 3];
        center[1] += vertices[i * 3 + 1];
        center[2] += vertices[i * 3 + 2];
    }

    center[0] /= numVertices;
    center[1] /= numVertices;
    center[2] /= numVertices;
}

inline void MeshSlicer::interpolateVertexData(const float v0[3], const float v1[3], float t,
    const float n0[3], const float n1[3],
    float outVertex[3], float outNormal[3]) {
    // Linear interpolation for position
    outVertex[0] = v0[0] + t * (v1[0] - v0[0]);
    outVertex[1] = v0[1] + t * (v1[1] - v0[1]);
    outVertex[2] = v0[2] + t * (v1[2] - v0[2]);

    // Linear interpolation for normal
    outNormal[0] = n0[0] + t * (n1[0] - n0[0]);
    outNormal[1] = n0[1] + t * (n1[1] - n0[1]);
    outNormal[2] = n0[2] + t * (n1[2] - n0[2]);

    // Normalize the interpolated normal
    float length = sqrt(outNormal[0] * outNormal[0] + outNormal[1] * outNormal[1] + outNormal[2] * outNormal[2]);
    if (length > 1e-6f) {
        outNormal[0] /= length;
        outNormal[1] /= length;
        outNormal[2] /= length;
    }
}

#endif // MESH_SLICER_H