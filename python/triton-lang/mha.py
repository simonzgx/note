import torch
from torch import nn
import pdb
from torch.nn.parameter import Parameter

device = torch.device(
    "cuda") if torch.cuda.is_available() else torch.device("cpu")
torch.manual_seed(42)
torch.cuda.manual_seed(42)
hiddem_dim = 1024
num_heads = 8


# Q = query * Weight + bias
# K = key * Weight + bias
# V = Value * Weight + bias
# atten = softmax(QK^T)V/sqrt(d_k)
# out = atten * Weight + bias


class MultiHeadAttention(torch.nn.Module):
    def __init__(self, embed_dim, num_heads):
        super().__init__()
        self.embed_dim = embed_dim
        self.num_heads = num_heads
        self.head_dim = embed_dim // num_heads
        self.q_weight = torch.Tensor(embed_dim, embed_dim)
        self.k_weight = torch.Tensor(embed_dim, embed_dim)
        self.v_weight = torch.Tensor(embed_dim, embed_dim)
        self.q_bias = torch.Tensor(embed_dim)
        self.k_bias = torch.Tensor(embed_dim)
        self.v_bias = torch.Tensor(embed_dim)
        self.out_weight = torch.Tensor(embed_dim, embed_dim)
        self.out_bias = torch.Tensor(embed_dim)

    def forward(self, query, key, value):
        pdb.set_trace()
        Q = nn.Linear(query, self.q_weight, self.q_bias)
        K = nn.Linear(key, self.k_weight, self.k_bias)
        V = nn.Linear(value, self.v_weight, self.v_bias)
        Q = Q.view(self.num_heads, -1, self.head_dim) / math.sqrt(self.head_dim)
        K = K.view(self.num_heads, -1, self.head_dim)
        V = V.view(self.num_heads, -1, self.head_dim)
        atten = torch.bmm(Q, K.transpose(-2, -1))
        atten = torch.softmax(atten, dim=-1)
        atten = torch.matmul(atten, V).view(-1, self.embed_dim)
        atten = torch.matmul(atten, self.out_weight) + self.out_bias
        return atten


def base(q, k, v):
    mha = nn.MultiheadAttention(hiddem_dim, num_heads)
    state_dict = torch.load("mha.pth")
    print(state_dict.keys())
    mha.load_state_dict(state_dict, strict=True)
    mha = mha.to(device=device).eval()
    atten, _ = mha(query, key, value)
    return atten


def exp(q, k, v):
    state_dict = torch.load("mha.pth")
    mha = MultiHeadAttention(hiddem_dim, num_heads)
    in_weight = state_dict["in_proj_weight"]
    mha.q_weight = in_weight[:hiddem_dim, :]
    mha.k_weight = in_weight[hiddem_dim:hiddem_dim*2, :]
    mha.v_weight = in_weight[-hiddem_dim:, :]
    mha.q_bias = state_dict["in_proj_bias"][:hiddem_dim]
    mha.k_bias = state_dict["in_proj_bias"][hiddem_dim:hiddem_dim*2]
    mha.v_bias = state_dict["in_proj_bias"][-hiddem_dim:]
    mha.out_weight = state_dict["out_proj.weight"]
    mha.out_bias = state_dict["out_proj.bias"]
    return mha(q, k, v)


if __name__ == "__main__":
    query = torch.randn(10, hiddem_dim).to(device)
    key = torch.randn(10, hiddem_dim).to(device)
    value = torch.randn(10, hiddem_dim).to(device)
    pdb.set_trace()
    print("base", base(query, key, value))
    print("exp", exp(query, key, value))
