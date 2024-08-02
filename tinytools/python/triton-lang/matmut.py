import triton
import triton.language as tl
import torch


@triton.jit
def matmut(C, A, B,
           M, K, N,
           stride_am, stride_ak,
           stride_bk, stride_bn,
           stride_cm, stride_cn,
           BLOCK_M: tl.constexpr, BLOCK_K: tl.constexpr, BLOCK_N: tl.constexpr):
    pid = tl.program_id(axis=0)
    num_pid_m = tl.cdiv(M, BLOCK_M)  # block num at row
    pid_m = pid // num_pid_m
    offset_am = pid_m * BLOCK_M + tl.arange(0, BLOCK_M)
    offset_bn = (pid * BLOCK_N + tl.arange(0, BLOCK_N)) % N
    offset_k = tl.arange(0, BLOCK_K)
    a_ptrs = A + offset_am[:, None] * stride_am + offset_k[None, :] * stride_ak
    b_ptrs = B + offset_k[:, None] * stride_bk + offset_bn[None, :] * stride_bn
    accumulator = tl.zeros((BLOCK_M, BLOCK_N), dtype=tl.float32)
    for k in range(0, tl.cdiv(K, BLOCK_K)):
        a = tl.load(a_ptrs, mask=offset_k[:, None] < K - k * BLOCK_K, other=0.0)
        b = tl.load(b_ptrs, mask=offset_k[None, :] < K - k * BLOCK_K, other=0.0)
        accumulator = tl.dot(a, b, accumulator)
        a_ptrs += BLOCK_K * stride_ak
        b_ptrs += BLOCK_K * stride_bk
    # print(pid, pid_m)
    offset_cm = pid_m * BLOCK_M + tl.arange(0, BLOCK_M)
    offset_cn = (pid * BLOCK_N + tl.arange(0, BLOCK_N)) % N
    offset_c = offset_cm[:, None] * stride_cm + offset_cn[None, :] * stride_cn
    mask_c = offset_c < M * N
    tl.store(C + offset_c, accumulator.to(dtype=tl.float16), mask=mask_c)


# (0,0) (0,1), (1,0) (1,1)
def test_matmut():
    M = 32
    K = 32
    N = 32
    BLOCK_M = 16
    BLOCK_K = 16
    BLOCK_N = 16
    a = torch.rand(M, K, device="cuda", dtype=torch.float16)
    b = torch.rand(K, N, device="cuda", dtype=torch.float16)
    triton_output = torch.zeros(M, N, device="cuda", dtype=torch.float16)
    matmut[(4, 1, 1)](triton_output, a, b,
                      M, K, N,
                      a.stride(0), a.stride(1),
                      b.stride(0), b.stride(1),
                      triton_output.stride(0), triton_output.stride(1),
                      BLOCK_M, BLOCK_K, BLOCK_N)
    torch_output = torch.matmul(a, b)
    print(torch_output)
    print(triton_output)
    if torch.allclose(triton_output, torch_output, atol=1e-2, rtol=0):
        print("triton output close to torch output")
    else:
        print("triton output not close to torch output")


if __name__ == "__main__":
    test_matmut()
