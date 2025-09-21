#include "Mesh.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <limits>
#include <algorithm> // For std::min and std::max
#include <cmath>     // For std::sqrt

Mesh::Mesh() :
    vertices(nullptr), indices(nullptr), colors(nullptr), normals(nullptr),
    minBoundingBox(nullptr), maxBoundingBox(nullptr),
    numVertices(0), numIndices(0) {}

Mesh::~Mesh() {
    clear();
}

void Mesh::setData(float* newVertices, unsigned int* newIndices, float* newColors, float* newNormals,
    int numVerts, int numInds) {
    // Clear any existing data first to prevent leaks
    clear();

    numVertices = numVerts;
    numIndices = numInds;

    if (numVertices > 0) {
        vertices = new float[numVertices * 3];
        std::copy(newVertices, newVertices + numVertices * 3, vertices);

        if (newNormals) {
            normals = new float[numVertices * 3];
            std::copy(newNormals, newNormals + numVertices * 3, normals);
        }
    }

    if (numIndices > 0) {
        indices = new unsigned int[numIndices];
        std::copy(newIndices, newIndices + numIndices, indices);
    }

    // Note: Colors are not handled in this version.

    calculateBoundingBox();
}

void Mesh::clear() {
    delete[] vertices;
    delete[] indices;
    delete[] colors;
    delete[] normals;
    delete[] minBoundingBox;
    delete[] maxBoundingBox;

    vertices = nullptr;
    indices = nullptr;
    colors = nullptr;
    normals = nullptr;
    minBoundingBox = nullptr;
    maxBoundingBox = nullptr;
    numVertices = 0;
    numIndices = 0;
}

bool Mesh::loadOFF(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filename << std::endl;
        return false;
    }

    clear(); // Clear existing mesh data

    std::string header;
    file >> header;
    if (header != "OFF" && header != "COFF") {
        std::cerr << "Not a valid OFF/COFF file." << std::endl;
        return false;
    }

    int vCount, fCount, eCount;
    file >> vCount >> fCount >> eCount;

    numVertices = vCount;
    vertices = new float[vCount * 3];

    for (int i = 0; i < vCount; ++i) {
        file >> vertices[i * 3] >> vertices[i * 3 + 1] >> vertices[i * 3 + 2];
        if (header == "COFF") {
            int r, g, b, a;
            file >> r >> g >> b >> a; // Color data is read but ignored
        }
    }

    std::vector<unsigned int> tempIndices;
    tempIndices.reserve(fCount * 3); // Pre-allocate memory

    for (int i = 0; i < fCount; ++i) {
        int n;
        file >> n;
        std::vector<int> faceVertices(n);
        for (int j = 0; j < n; ++j) {
            file >> faceVertices[j];
        }

        // Triangulate the polygon into a triangle fan
        for (int j = 1; j < n - 1; ++j) {
            tempIndices.push_back(faceVertices[0]);
            tempIndices.push_back(faceVertices[j]);
            tempIndices.push_back(faceVertices[j + 1]);
        }
    }

    numIndices = tempIndices.size();
    indices = new unsigned int[numIndices];
    std::copy(tempIndices.begin(), tempIndices.end(), indices);

    file.close();

    calculateNormals();
    calculateBoundingBox();

    return true;
}

void Mesh::calculateBoundingBox() {
    delete[] minBoundingBox;
    minBoundingBox = nullptr;
    delete[] maxBoundingBox;
    maxBoundingBox = nullptr;

    if (numVertices == 0 || vertices == nullptr) {
        return;
    }

    minBoundingBox = new float[3];
    maxBoundingBox = new float[3];

    minBoundingBox[0] = maxBoundingBox[0] = vertices[0];
    minBoundingBox[1] = maxBoundingBox[1] = vertices[1];
    minBoundingBox[2] = maxBoundingBox[2] = vertices[2];

    for (int i = 1; i < numVertices; ++i) {
        int idx = i * 3;
        minBoundingBox[0] = std::min(minBoundingBox[0], vertices[idx]);
        maxBoundingBox[0] = std::max(maxBoundingBox[0], vertices[idx]);

        minBoundingBox[1] = std::min(minBoundingBox[1], vertices[idx + 1]);
        maxBoundingBox[1] = std::max(maxBoundingBox[1], vertices[idx + 1]);

        minBoundingBox[2] = std::min(minBoundingBox[2], vertices[idx + 2]);
        maxBoundingBox[2] = std::max(maxBoundingBox[2], vertices[idx + 2]);
    }
}

void Mesh::calculateColors() {
    // This function is a stub
}

void Mesh::calculateNormals() {
    delete[] normals;
    normals = new float[numVertices * 3](); // Value-initialize to zero

    for (int i = 0; i < numIndices; i += 3) {
        unsigned int i0 = indices[i];
        unsigned int i1 = indices[i + 1];
        unsigned int i2 = indices[i + 2];

        const float* v0 = &vertices[i0 * 3];
        const float* v1 = &vertices[i1 * 3];
        const float* v2 = &vertices[i2 * 3];

        float edge1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        float edge2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };

        float faceNormal[3];
        faceNormal[0] = edge1[1] * edge2[2] - edge1[2] * edge2[1];
        faceNormal[1] = edge1[2] * edge2[0] - edge1[0] * edge2[2];
        faceNormal[2] = edge1[0] * edge2[1] - edge1[1] * edge2[0];

        // Add this face normal to the normals of its three vertices
        for (int j = 0; j < 3; ++j) {
            normals[i0 * 3 + j] += faceNormal[j];
            normals[i1 * 3 + j] += faceNormal[j];
            normals[i2 * 3 + j] += faceNormal[j];
        }
    }

    // Normalize all the vertex normals
    for (int i = 0; i < numVertices; ++i) {
        float* n = &normals[i * 3];
        float length = std::sqrt(n[0] * n[0] + n[1] * n[1] + n[2] * n[2]);
        if (length > 1e-6f) {
            n[0] /= length;
            n[1] /= length;
            n[2] /= length;
        }
    }
}