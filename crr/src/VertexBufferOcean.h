//
// A class that stores ocean height field
// which is calculated using Discrete Fourier Transform
// Created by 何昊 on 2018/03/05.
//

#ifndef PROJECT_VERTEX_BUFFER_OCEAN_H
#define PROJECT_VERTEX_BUFFER_OCEAN_H


// GLM Math Library
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <complex>


class VertexBufferOcean
{
public:
    VertexBufferOcean(glm::vec2 wind, int resolution, float amplitude);
    ~VertexBufferOcean();

    // Given current time, generate wave
    void generateWave(float time);

    // The 3*N*N array to store final vertices position
    int vertexCount;
    float *vertices;
    int indexCount;
    unsigned int *indices;
    // The 3*N*N array to store normal vector results
    int normalCount;
    float *normals;
    // The flag to control generating method
    bool useFFT;
private:
    float g;
    float PI;
    float unitWidth;
    // Choppiness of the wave
    float choppy;
    // Resolution
    int N;
    // Water Length
    int L;
    // Wave height amplitude parameter
    float A;
    // Wind direction and speed in one vector
    glm::vec2 w;
    // the buffer to store computed results
    std::complex<float> *hBuffer;
    glm::vec2 *kBuffer;
    std::complex<float> *epsilonBufferx;
    std::complex<float> *epsilonBuffery;
    std::complex<float> *displacementBufferx;
    std::complex<float> *displacementBuffery;

    // Returns height
    float H(float x, float z, float t);

    std::complex<float> h(glm::vec2 k, float t);

    std::complex<float> h0(glm::vec2 k);

    inline float normalRandom();

    inline float Ph(glm::vec2 k);

    inline float omega(glm::vec2 k);

    glm::vec3 epsilon(float x, float z, float t);

    // Returns displacement vector
    glm::vec3 D(float x, float z, float t);
};


#endif