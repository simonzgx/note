#include "reduce.h"

#include <cstdio>
#include <cuda.h>
#include <cuda_runtime.h>

static const int WARP_SIZE = 32;

template <typename Tp> __device__ __forceinline__ Tp warp_reduce_sum(Tp val) {
#pragma unroll
  for (int i = WARP_SIZE >> 1; i >= 1; i >>= 1) {
    val += __shfl_down_sync(0xffffffff, val, i);
  }
  return val;
}

__device__ void block_reduce_sum(float *dst, float *src, int m, int n) {
  static const int warp_num = 32;
  __shared__ float data[warp_num];

  int row = blockIdx.x;
  int col = blockDim.x * threadIdx.y + threadIdx.x;

  int lane_id = col % WARP_SIZE;
  int warp_id = col / WARP_SIZE;

  float val = row < m && col < n ? src[row * n + col] : float(0);
  __syncthreads();
  float sum = warp_reduce_sum<float>(val);
  if (lane_id == 0) {
    data[warp_id] = sum;
  }
  __syncthreads();
  val = col < warp_num ? data[col] : float(0);
  sum = warp_reduce_sum(val);

  if (col == 0) {
    dst[row] = sum;
  }
}

// in:  M x N
// out: M x 1
// grid(m) block(32, n / 128)
__global__ void block_reduce_sum_vec(float *out, float *in, int m, int n) {
  int row = blockIdx.x;
  int tid = threadIdx.y * blockDim.x + threadIdx.x;
  int col = tid * 4;
  int lane_id = tid % WARP_SIZE;
  int warp_id = tid / WARP_SIZE;

  extern __shared__ float data[];
  float4 val = row < m && col < n
                   ? *reinterpret_cast<float4 *>(&in[row * n + col])
                   : make_float4(0, 0, 0, 0);
  float sum = val.x + val.y + val.z + val.w;
  extern __shared__ float data[];
  __syncthreads();
  sum = warp_reduce_sum(sum);
  if (lane_id == 0) {
    data[warp_id] = sum;
  }
  __syncthreads();
  sum = warp_id == 0 ? data[lane_id] : float(0);
  sum = warp_reduce_sum(sum);
  __syncthreads();
  if (warp_id == 0 && lane_id == 0) {
    out[row] = sum;
  }
}

template <typename Tp>
__global__ void reduce_sum(Tp *dst, Tp *src, int m, int n) {
  block_reduce_sum(dst, src, m, n);
}

__global__ void reduce_avg(float *out, float *in, int m, int n) {
  // 先计算sum
  block_reduce_sum(out, in, m, n);
  int row = blockIdx.x;
  if (blockIdx.y == 0 && threadIdx.y == 0) {
    out[row] = out[row] / n;
  }
}

// reduce sum
// grid(m, n/32), block(1, 32)
void g_reduce_sum(float *out, float *in, int m, int n) {
  int blockY = (n + WARP_SIZE - 1) / WARP_SIZE;
  dim3 block(WARP_SIZE, blockY);
  dim3 grid(m);
  reduce_sum<<<grid, block>>>(out, in, m, n);
}

// reduce avg
// grid(m, n/32), block(1, 32)
void g_reduce_avg(float *out, float *in, int m, int n) {
  int blockX = (n + WARP_SIZE - 1) / WARP_SIZE;
  dim3 block(blockX, WARP_SIZE);
  dim3 grid(m);
  reduce_avg<<<grid, block>>>(out, in, m, n);
}

void g_reduce_sum_vec(float *out, float *in, int m, int n) {
  int blockY = (n + WARP_SIZE - 1) / WARP_SIZE;
  blockY = (blockY + 4 - 1) / 4;
  dim3 block(WARP_SIZE, blockY);
  dim3 grid(m);
  int sharedMemSize = blockY;
  printf("blockY:%d sharedMemSize:%d\n", blockY, sharedMemSize);
  block_reduce_sum_vec<<<grid, block, sharedMemSize>>>(out, in, m, n);
}