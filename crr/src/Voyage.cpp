#include "Voyage.h"
#include "FFT.h"

#include <random>
#include <iostream>
#include <vector>
#include <chrono>

using namespace std;

#define PI acos(-1)
/* 0-1正态分布生成 */
float normalRandom()
{
    static auto seed = chrono::high_resolution_clock::now().time_since_epoch().count();
    static default_random_engine generator((unsigned)seed);
    static normal_distribution<float> distribution(0.5, 0.1);
    return distribution(generator);
}

Voyage::Voyage(glm::vec2 wind, glm::vec2 velocity, int N, float amplitude, int timedelta): wind(wind), velocity(velocity), N(N), Amplitude(amplitude), timedelta(timedelta)
{
    g = 9.8f;
    Length =  N / 8;
    vertexCount = normalCount = 3 * N * N;
    indexCount  = 6 * N * N;
    hBuffer = new complex<float>[N * N];
    epsilonx = new complex<float>[N * N];
    epsilony = new complex<float>[N * N];
    
    for (unsigned int i = 0; i < N - 1; ++i) {
        for (unsigned int j = 0; j < N - 1; ++j) {
            indices.push_back((i * N + j));
            indices.push_back((i * N + j + 1));
            indices.push_back(((i + 1) * N + j));
            indices.push_back((i * N + j + 1));
            indices.push_back(((i + 1) * N + j));
            indices.push_back(((i + 1) * N + j + 1));
        }
    }
    
    for (int n = -N / 2; n < N / 2; ++n) {
        float kx = 2.0f * PI * n / Length;   //不需要在下面的循环中重复计算
        for (int m = -N / 2; m < N / 2; ++m) {
            glm::vec2 k = glm::vec2(kx, 2.0f * PI * m / Length);
            int bufferIndex = (n + N/2) * N + m + N/2;          //由负取整
            kBuffer.push_back(k);
        }
    }
}

Voyage::~Voyage()
{
    delete[] hBuffer;
    delete[] epsilonx;
    delete[] epsilony;
}

void Voyage::generateWave(float time)
{
    vertices.clear();
    normals.clear();
    time += timedelta;
    for (int n = -N / 2; n < N / 2; ++n) {
        for (int m = -N / 2; m < N / 2; ++m) {
            int bufferIndex = (n + N/2) * N + m + N/2;
            hBuffer[bufferIndex] = h(kBuffer[bufferIndex], time);

            epsilonx[bufferIndex] = hBuffer[bufferIndex] * complex<float>(0.0f, kBuffer[bufferIndex].x);
            epsilony[bufferIndex] = hBuffer[bufferIndex] * complex<float>(0.0f, kBuffer[bufferIndex].y);

        }
    }

    
    complex<float> *HBuffer = new complex<float>[N * N];

    // First round of FFT on rows
    for (int i = 0; i < N; ++i) {
        vector<complex<float>> temp1(N), temp2(N), buffer1(N), buffer2(N);

        for (int j = 0; j < N; ++j)
            temp1[j] = hBuffer[i * N + j];
        FFT(temp1, buffer1, N);
        for (int j = 0; j < N; ++j)
            HBuffer[i * N + j] = buffer1[j];

        for (int j = 0; j < N; ++j) {
            temp1[j] = epsilonx[i * N + j];
            temp2[j] = epsilony[i * N + j];
        }
        FFT(temp1, buffer1, N);
        FFT(temp2, buffer2, N);
        for (int j = 0; j < N; ++j) {
            epsilonx[i * N + j] = buffer1[j];
            epsilony[i * N + j] = buffer2[j];
        }

    }

    // Second round of FFT on columns
    for (int i = 0; i < N; ++i) {
        vector<complex<float>> temp1(N), temp2(N), buffer1(N), buffer2(N);
        for (int j = 0; j < N; ++j)
            temp1[j] = HBuffer[j * N + i];
        FFT(temp1, buffer1, N);
        for (int j = 0; j < N; ++j) {
            if ((i + j) % 2 == 0)
                HBuffer[j * N + i] = buffer1[j];
            else
                HBuffer[j * N + i] = -buffer1[j];
        }

        for (int j = 0; j < N; ++j) {
            temp1[j] = epsilonx[j * N + i];
            temp2[j] = epsilony[j * N + i];
        }
        FFT(temp1, buffer1, N);
        FFT(temp2, buffer2, N);
        for (int j = 0; j < N; ++j) {
            if ((i + j) % 2 == 0) {
                epsilonx[j * N + i] = buffer1[j];
                epsilony[j * N + i] = buffer2[j];
            } else {
                epsilonx[j * N + i] = -buffer1[j];
                epsilony[j * N + i] = -buffer2[j];
            }
        }

    }

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            int pos = 3 * (i * N + j);
            float x = Width * Length * (i - N / 2.0f) / N;
            float z = Width * Length * (j - N / 2.0f) / N;
            vertices.push_back(x);
            vertices.push_back(HBuffer[i * N + j].real());
            vertices.push_back(z);

            normals.push_back(-epsilonx[i * N + j].real());
            normals.push_back(1);
            normals.push_back(-epsilony[i * N + j].real());
        }
    }
    delete[] HBuffer;
    
}

/* 高度场每项 */
complex<float> Voyage::h(glm::vec2 k, float t)
{
    complex<float> result(0.0f, 0.0f);
    float omega_k = omega(k);
    float coswt = cos(omega_k * t);
    float sinwt = sin(omega_k * t);
    result += h0(k) * complex<float>(coswt, sinwt) + 
        conj(h0(-k)) * complex<float>(coswt, -sinwt);
    return result;
}

/* h0(k)计算 */
complex<float> Voyage::h0(glm::vec2 k)
{
    float xi_r = normalRandom(), xi_i = normalRandom();
    return (1.0f/sqrt(2.0f)) * complex<float>(xi_r, xi_i) * sqrt(Ph(k));
}

float Voyage::Ph(glm::vec2 k)
{
    if (glm::length(k) < 0.001f) return 0.0f;
    float kvalue = glm::length(k);
    float L = glm::length(velocity)*glm::length(velocity) / g;        //给一个v值，可调
    float result;
    result = Amplitude * 
        exp(-1.0f/((kvalue*L)*(kvalue*L))) / pow(kvalue, 4) *
        pow(glm::dot(glm::normalize(k), glm::normalize(wind)), 2);
    return result;
}

/* 根号下gk */
float Voyage::omega(glm::vec2 k)
{
    return sqrt(g * glm::length(k));
}

