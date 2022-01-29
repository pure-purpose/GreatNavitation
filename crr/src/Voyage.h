#ifndef VOYAGE_H
#define VOYAGE_H


#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <complex>
#include <vector>
using namespace std;


class Voyage
{
public:
    Voyage(glm::vec2 wind, glm::vec2 velocity, int resolution, float amplitude, int timedelta);
    ~Voyage();

    void generateWave(float time);
    int vertexCount;
    //float* vertices;
    vector<float> vertices;
    int indexCount;
    vector<int>indices;
    int normalCount;
    //float* normals;
    vector<float> normals;

private:
    float g;
    float Width = 3.0f;
    int N;
    int timedelta;
    int Length;
    float Amplitude;
    glm::vec2 wind;
    glm::vec2 velocity;
    complex<float> *hBuffer;
    vector<glm::vec2>kBuffer;
    complex<float> *epsilonx;
    complex<float> *epsilony;

    complex<float> h(glm::vec2 k, float t);
    complex<float> h0(glm::vec2 k);
    float Ph(glm::vec2 k);
    float omega(glm::vec2 k);
};


#endif