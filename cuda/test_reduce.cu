#include <cstdio>
#include <cuda.h>
#include <cuda_runtime.h>

#include "operator/reduce.h"

int main() {
  const int M = 1;
  const int N = 4096;

  float *src = static_cast<float *>(malloc(M * N * sizeof(float)));
  float *result = static_cast<float *>(malloc(M * sizeof(float)));
  for (int i = 0; i < M; ++i) {
    for (int j = 0; j < N; ++j) {
      src[i * N + j] = j;
    }
  }

  float *d_src, *d_result;
  cudaMalloc(&d_src, M * N * sizeof(float));
  cudaMalloc(&d_result, M * sizeof(float));

  cudaMemcpy(d_src, src, M * N * sizeof(float), cudaMemcpyHostToDevice);

  g_reduce_sum_vec(d_result, d_src, M, N);

  cudaMemcpy(result, d_result, M * sizeof(float), cudaMemcpyDeviceToHost);
  for (int i = 0; i < M; ++i) {
    printf("%f ", result[i]);
  }

  cudaFree(d_src);
  cudaFree(d_result);
  free(src);
  free(result);

  return 0;
}