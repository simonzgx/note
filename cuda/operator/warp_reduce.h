#include <cstdio>
#include <cuda.h>
#include <cuda_runtime.h>

const int WARP_SIZE = 32;

template <typename Tp> __device__ __forceinline__ Tp warp_reduce_sum(Tp val) {
  for (int i = WARP_SIZE >> 1; i >= 1; i >>= 1) {
    val += __shfl_down_sync(0xffffffff, val, i);
  }
  return val;
}

template <typename Tp, const int M, const int N>
__global__ void block_reduce_sum(Tp *dst, Tp *src) {

  constexpr int warp_num = (N + WARP_SIZE - 1) / WARP_SIZE;
  __shared__ Tp data[warp_num];

  int row = blockIdx.x;
  int col = blockIdx.y * blockDim.y + threadIdx.x;

  int lane_id = col % WARP_SIZE;
  int warp_id = col / WARP_SIZE;

  Tp val = Tp(0);
  if (row < M && col < N) {
    val = src[row * N + col];
  }
  __syncthreads();
  Tp sum = warp_reduce_sum<Tp>(val);
  if (warp_id < warp_num && lane_id == 0) {
    data[warp_id] = sum;
  }
  __syncthreads();
  val = col < warp_num ? data[col] : Tp(0);
  sum = warp_reduce_sum(val);
  __syncthreads();

  if (col == 0) {
    dst[row] = sum;
  }
}