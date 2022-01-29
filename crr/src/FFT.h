#ifndef FFT_H
#define FFT_H

#include <complex>
#include <vector>
using namespace std;

#define PI acos(-1)

void Rader(const vector<complex<float>>& before, vector<std::complex<float>>& after, int N)
{
    int k, j = 0, temp = 0;
    int* index = new int[N];
    if (N != -1) {

        for (int i = 0; i < N; i++) {
            index[i] = i;
        }
        for (int i = 0; i < N - 1; i++)
        {
            if (i < j)
            {
                temp = index[i];
                index[i] = index[j];
                index[j] = temp;
            }

            k = N >> 1;

            while (k <= j)
            {
                j = j - k;
                k >>= 1;
            }

            j = j + k;
        }

    }

    for (int i = 0; i < N; i++) {
        after[i] = before[index[i]];
    }
    delete[] index;
}

void FFT(const vector<complex<float>>& before, vector<complex<float>>& after, int N)
{
    Rader(before, after, N);
    for (int i = 1; (1 << i) <= N; i++) {
        int m = (1 << i);
        complex<float> wm = complex<float>(cos(2 * PI / m), sin(2 * PI / m));
        for (int k = 0; k < N; k += m) {
            complex<float> w = 1.0;
            for (int j = 0; j < (m >> 1); j++) {
                complex<float> t = w * after[k + j + (m >> 1)];
                complex<float> u = after[k + j];
                after[k + j] = u + t;
                after[k + j + (m >> 1)] = u - t;
                w = w * wm;
            }
        }
    }
}

#endif