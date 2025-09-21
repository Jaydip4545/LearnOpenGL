#include "MeshSlicer.h"
#include "Mesh.h"
#include <cmath>
#include <cstring> 
#include <vector>  

MeshPartArray* MeshSlicer::sliceMesh(Mesh& originalMesh,
    const Transform& worldTransform,
    const CuttingPlane* cuttingPlanes,
    int numPlanes,
    float separationDistance) {
    MeshPartArray* currentParts = new MeshPartArray();
    
    
    

    // Create world-space version of original mesh
    Mesh* worldMesh = new Mesh();

    // Copy original mesh data
    int numVertices = originalMesh.getNumVertices();
    int numIndices = originalMesh.getNumIndices();

    // Transform vertices to world space
    float* worldVertices = new float[numVertices * 3];
    transformMeshToWorld(originalMesh, worldTransform, worldVertices);

    // Copy normals and indices (no colors)
    float* normals = new float[numVertices * 3];
    unsigned int* indices = new unsigned int[numIndices];

    // Use memcpy for efficient copying
    std::memcpy(normals, originalMesh.getNormals(), sizeof(float) * numVertices * 3);
    std::memcpy(indices, originalMesh.getIndices(), sizeof(unsigned int) * numIndices);

    // Set data to world mesh (without colors)
    // Mesh::setData will create its own copies of this data
    worldMesh->setData(worldVertices, indices, nullptr, normals, numVertices, numIndices);
    currentParts->add(worldMesh, 0);
    

    // Clean up temporary buffers
    delete[] worldVertices;
    delete[] normals;
    delete[] indices;
    if (numPlanes == 0)
    {
        return currentParts;
    }
    // Apply each cutting plane sequentially
    for (int planeIndex = 0; planeIndex < numPlanes; planeIndex++) {
        MeshPartArray* nextParts = new MeshPartArray();

        for (int partIndex = 0; partIndex < currentParts->getCount(); partIndex++) {
            Mesh* currentMesh = currentParts->getMesh(partIndex);
            if (currentMesh) {
                Mesh* positivePart = nullptr;
                Mesh* negativePart = nullptr;

                sliceMeshWithSinglePlane(*currentMesh, cuttingPlanes[planeIndex],
                    positivePart, negativePart);

                if (positivePart && positivePart->getNumVertices() > 0) {
                    nextParts->add(positivePart, nextParts->getCount());
                }
                else {
                    delete positivePart; // Clean up if empty
                }

                if (negativePart && negativePart->getNumVertices() > 0) {
                    nextParts->add(negativePart, nextParts->getCount());
                }
                else {
                    delete negativePart; // Clean up if empty
                }
            }
        }

        delete currentParts; // Deletes the previous set of mesh parts
        currentParts = nextParts;
    }

    // Calculate original center in world space
    float originalCenter[3];
    calculateMeshCenter(originalMesh, originalCenter);

    float worldCenter[3];
    worldTransform.transformPoint(originalCenter[0], originalCenter[1], originalCenter[2],
        worldCenter[0], worldCenter[1], worldCenter[2]);

    // Separate parts for visualization
    separateParts(currentParts, worldCenter, separationDistance);

    return currentParts;
}

void MeshSlicer::sliceMeshWithSinglePlane(const Mesh& inputMesh,
    const CuttingPlane& plane,
    Mesh*& positivePart,
    Mesh*& negativePart) {
    int numVertices = inputMesh.getNumVertices();
    int numIndices = inputMesh.getNumIndices();
    int numTriangles = numIndices / 3;

    
    int maxNewVertices = numVertices + numTriangles * 2;
    int maxNewIndices = numIndices + numTriangles * 3; // Each split can add a triangle

    TempVertexArray positiveArray;
    TempVertexArray negativeArray;
    positiveArray.reserve(maxNewVertices, maxNewIndices);
    negativeArray.reserve(maxNewVertices, maxNewIndices);

    const float* vertices = inputMesh.getVertices();
    const float* normals = inputMesh.getNormals();
    const unsigned int* indices = inputMesh.getIndices();

    // Process each triangle
    for (int t = 0; t < numTriangles; t++) {
        unsigned int i0 = indices[t * 3];
        unsigned int i1 = indices[t * 3 + 1];
        unsigned int i2 = indices[t * 3 + 2];

        const float* v0 = &vertices[i0 * 3];
        const float* v1 = &vertices[i1 * 3];
        const float* v2 = &vertices[i2 * 3];

        const float* n0 = &normals[i0 * 3];
        const float* n1 = &normals[i1 * 3];
        const float* n2 = &normals[i2 * 3];

        sliceTriangleWithPlane(v0, v1, v2, n0, n1, n2, plane,
            positiveArray, negativeArray);
    }

    // Create final mesh objects from the temporary arrays
    createMeshFromTempArray(positiveArray, positivePart);
    createMeshFromTempArray(negativeArray, negativePart);
}

void MeshSlicer::sliceTriangleWithPlane(const float v0[3], const float v1[3], const float v2[3],
    const float n0[3], const float n1[3], const float n2[3],
    const CuttingPlane& plane,
    TempVertexArray& positivePart,
    TempVertexArray& negativePart) {
    
    

    
        const float epsilon = 1e-6f;

        // Calculate signed distances from vertices to plane
        float d[3];
        d[0] = plane.distanceToPoint(v0[0], v0[1], v0[2]);
        d[1] = plane.distanceToPoint(v1[0], v1[1], v1[2]);
        d[2] = plane.distanceToPoint(v2[0], v2[1], v2[2]);

        // Classify vertices: 1 for positive, -1 for negative, 0 for on-plane
        int side[3];
        side[0] = (d[0] > epsilon) ? 1 : (d[0] < -epsilon) ? -1 : 0;
        side[1] = (d[1] > epsilon) ? 1 : (d[1] < -epsilon) ? -1 : 0;
        side[2] = (d[2] > epsilon) ? 1 : (d[2] < -epsilon) ? -1 : 0;

        int positiveCount = (side[0] > 0) + (side[1] > 0) + (side[2] > 0);
        int negativeCount = (side[0] < 0) + (side[1] < 0) + (side[2] < 0);

        // Case 1: Triangle is entirely on one side (or on-plane)
        if (negativeCount == 0) { // All positive or on-plane
            int baseIdx = positivePart.vertexCount;
            positivePart.addVertex(v0[0], v0[1], v0[2], n0[0], n0[1], n0[2]);
            positivePart.addVertex(v1[0], v1[1], v1[2], n1[0], n1[1], n1[2]);
            positivePart.addVertex(v2[0], v2[1], v2[2], n2[0], n2[1], n2[2]);
            positivePart.addTriangle(baseIdx, baseIdx + 1, baseIdx + 2);
            return;
        }
        if (positiveCount == 0) { // All negative or on-plane
            int baseIdx = negativePart.vertexCount;
            negativePart.addVertex(v0[0], v0[1], v0[2], n0[0], n0[1], n0[2]);
            negativePart.addVertex(v1[0], v1[1], v1[2], n1[0], n1[1], n1[2]);
            negativePart.addVertex(v2[0], v2[1], v2[2], n2[0], n2[1], n2[2]);
            negativePart.addTriangle(baseIdx, baseIdx + 1, baseIdx + 2);
            return;
        }

        // Case 2: Triangle is split. One vertex is on one side, two are on the other.
        const float* vertices[] = { v0, v1, v2 };
        const float* normals[] = { n0, n1, n2 };

        // Find the vertex that is on its own side of the plane
        int isolatedIndex = -1;
        if ((positiveCount == 1 && negativeCount == 2) || (positiveCount == 2 && negativeCount == 1)) {
            int isolatedSide = (positiveCount == 1) ? 1 : -1;
            for (int i = 0; i < 3; ++i) {
                if (side[i] == isolatedSide) {
                    isolatedIndex = i;
                    break;
                }
            }
        }
        else {
           
            int baseIdxPos = positivePart.vertexCount;
            positivePart.addVertex(v0[0], v0[1], v0[2], n0[0], n0[1], n0[2]);
            positivePart.addVertex(v1[0], v1[1], v1[2], n1[0], n1[1], n1[2]);
            positivePart.addVertex(v2[0], v2[1], v2[2], n2[0], n2[1], n2[2]);
            positivePart.addTriangle(baseIdxPos, baseIdxPos + 1, baseIdxPos + 2);

            int baseIdxNeg = negativePart.vertexCount;
            negativePart.addVertex(v0[0], v0[1], v0[2], n0[0], n0[1], n0[2]);
            negativePart.addVertex(v1[0], v1[1], v1[2], n1[0], n1[1], n1[2]);
            negativePart.addVertex(v2[0], v2[1], v2[2], n2[0], n2[1], n2[2]);
            negativePart.addTriangle(baseIdxNeg, baseIdxNeg + 1, baseIdxNeg + 2);
            return;
        }

        // The other two vertices
        int otherIndex1 = (isolatedIndex + 1) % 3;
        int otherIndex2 = (isolatedIndex + 2) % 3;

        // Find the two intersection points
        EdgeIntersection intersection1 = findEdgeIntersection(vertices[isolatedIndex], vertices[otherIndex1], normals[isolatedIndex], normals[otherIndex1], plane);
        EdgeIntersection intersection2 = findEdgeIntersection(vertices[isolatedIndex], vertices[otherIndex2], normals[isolatedIndex], normals[otherIndex2], plane);

        // Assign to the correct sides
        TempVertexArray& isolatedPart = (side[isolatedIndex] > 0) ? positivePart : negativePart;
        TempVertexArray& otherPart = (side[isolatedIndex] > 0) ? negativePart : positivePart;

        
        int baseIdx = isolatedPart.vertexCount;
        isolatedPart.addVertex(vertices[isolatedIndex][0], vertices[isolatedIndex][1], vertices[isolatedIndex][2], normals[isolatedIndex][0], normals[isolatedIndex][1], normals[isolatedIndex][2]);
        isolatedPart.addVertex(intersection1.point[0], intersection1.point[1], intersection1.point[2], intersection1.normal[0], intersection1.normal[1], intersection1.normal[2]);
        isolatedPart.addVertex(intersection2.point[0], intersection2.point[1], intersection2.point[2], intersection2.normal[0], intersection2.normal[1], intersection2.normal[2]);
        isolatedPart.addTriangle(baseIdx, baseIdx + 1, baseIdx + 2);

       
        baseIdx = otherPart.vertexCount;
        otherPart.addVertex(vertices[otherIndex1][0], vertices[otherIndex1][1], vertices[otherIndex1][2], normals[otherIndex1][0], normals[otherIndex1][1], normals[otherIndex1][2]);
        otherPart.addVertex(vertices[otherIndex2][0], vertices[otherIndex2][1], vertices[otherIndex2][2], normals[otherIndex2][0], normals[otherIndex2][1], normals[otherIndex2][2]);
        otherPart.addVertex(intersection1.point[0], intersection1.point[1], intersection1.point[2], intersection1.normal[0], intersection1.normal[1], intersection1.normal[2]);
        otherPart.addVertex(intersection2.point[0], intersection2.point[1], intersection2.point[2], intersection2.normal[0], intersection2.normal[1], intersection2.normal[2]);

        
        otherPart.addTriangle(baseIdx, baseIdx + 1, baseIdx + 3);
        otherPart.addTriangle(baseIdx, baseIdx + 3, baseIdx + 2);
    
   
   
    
}


MeshSlicer::EdgeIntersection MeshSlicer::findEdgeIntersection(
    const float v0[3], const float v1[3],
    const float n0[3], const float n1[3],
    const CuttingPlane& plane) {

    EdgeIntersection result;
    result.valid = false;

    float d0 = plane.distanceToPoint(v0[0], v0[1], v0[2]);
    float d1 = plane.distanceToPoint(v1[0], v1[1], v1[2]);

    // Ensure the edge actually crosses the plane
    if (d0 * d1 > 0) {
        return result;
    }

    float diff = d0 - d1;
    
    if (std::abs(diff) < 1e-6f) {
        return result;
    }

    // Calculate intersection parameter and interpolate
    result.t = d0 / diff;
    interpolateVertexData(v0, v1, result.t, n0, n1, result.point, result.normal);
    result.valid = true;

    return result;
}

void MeshSlicer::createMeshFromTempArray(const TempVertexArray& tempArray, Mesh*& mesh) {
    if (tempArray.vertexCount == 0 || tempArray.indexCount == 0) {
        mesh = nullptr;
        return;
    }

    // Allocate new buffers for the final mesh
    float* vertsCopy = new float[tempArray.vertexCount * 3];
    float* normsCopy = new float[tempArray.vertexCount * 3];
    unsigned int* indsCopy = new unsigned int[tempArray.indexCount];

    // Copy data from temp arrays to final buffers
    std::memcpy(vertsCopy, tempArray.vertices, sizeof(float) * tempArray.vertexCount * 3);
    std::memcpy(normsCopy, tempArray.normals, sizeof(float) * tempArray.vertexCount * 3);
    std::memcpy(indsCopy, tempArray.indices, sizeof(unsigned int) * tempArray.indexCount);

    mesh = new Mesh();
    // Mesh::setData will create its own internal copy of the data
    mesh->setData(vertsCopy, indsCopy, nullptr, normsCopy,
        tempArray.vertexCount, tempArray.indexCount);

    
    delete[] vertsCopy;
    delete[] normsCopy;
    delete[] indsCopy;
}

void MeshSlicer::separateParts(MeshPartArray* parts, const float originalCenter[3],
    float separationDistance) {
    if (separationDistance == 0.0f) return;

    for (int i = 0; i < parts->getCount(); i++) {
        Mesh* mesh = parts->getMesh(i);
        if (!mesh) continue;

        // Calculate part center
        float partCenter[3];
        calculateMeshCenter(*mesh, partCenter);

        // Calculate direction from original center to part center
        float direction[3] = {
            partCenter[0] - originalCenter[0],
            partCenter[1] - originalCenter[1],
            partCenter[2] - originalCenter[2]
        };

        // Normalize direction
        float length = sqrtf(direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2]);
        if (length > 1e-6f) {
            direction[0] /= length;
            direction[1] /= length;
            direction[2] /= length;
        }
        else {
            
            float angle = (2.0f * 3.14159265f * i) / parts->getCount();
            direction[0] = cosf(angle);
            direction[1] = sinf(angle);
            direction[2] = 0.0f;
        }

       
        float offset[3] = {
            direction[0] * separationDistance,
            direction[1] * separationDistance,
            direction[2] * separationDistance
        };

        // Move all vertices in this mesh part
        int numVertices = mesh->getNumVertices();
        float* vertices = mesh->getVertices();

        for (int v = 0; v < numVertices; v++) {
            vertices[v * 3] += offset[0];
            vertices[v * 3 + 1] += offset[1];
            vertices[v * 3 + 2] += offset[2];
        }

        
        mesh->calculateBoundingBox();
    }
}