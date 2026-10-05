#pragma clang fp contract(off)
using namespace metal;

kernel void lab_fill_range(device float* x [[buffer(0)]], constant uint& n [[buffer(1)]], constant float& lo [[buffer(2)]], constant float& hi [[buffer(3)]], uint t [[thread_position_in_grid]]) {
  if (t >= n) return;
  uint h = t * 2654435761u + 99u; h ^= h >> 15; h *= 2246822519u; h ^= h >> 13;
  x[t] = lo + (hi - lo) * (float(h & 0xffffu) * (1.0f / 65536.0f));
}

// lab_delta_old: q38-delta-span-msl as it stands: a thread a state row, the whole token loop inside, the row read and written in device memory twice a token.
kernel void lab_delta_old(device float* S [[buffer(0)]], device const float* c [[buffer(1)]], device const float* alpha [[buffer(2)]], device const float* beta [[buffer(3)]], device float* o [[buffer(4)]], constant uint& nv [[buffer(5)]], constant uint& dv [[buffer(6)]], constant uint& dk [[buffer(7)]], constant uint& nk [[buffer(8)]], constant float& qscale [[buffer(9)]], constant uint& cstride [[buffer(10)]], constant uint& gstride [[buffer(11)]], constant uint& ostride [[buffer(12)]], constant uint& ntok [[buffer(13)]], uint gid [[thread_position_in_grid]]) {
  if (gid >= nv * dv) return;
  uint h = gid / dv; uint i = gid - h * dv; uint kh = h - (h / nk) * nk; uint r = (h * dv + i) * dk;
  for (uint t = 0u; t < ntok; ++t) {
    uint co = t * cstride; device const float* q = c + co + kh * dk; device const float* k = c + co + nk * dk + kh * dk; float v = c[co + 2u * nk * dk + h * dv + i];
    float al = alpha[t * gstride + h]; float pred = 0.0f;
    for (uint j = 0u; j < dk; ++j) { float sp = al * S[r + j]; S[r + j] = sp; pred = pred + sp * k[j]; }
    float err = v - pred; float bw = beta[t * gstride + h] * err; float acc = 0.0f;
    for (uint j = 0u; j < dk; ++j) { float sn = S[r + j] + bw * k[j]; S[r + j] = sn; acc = acc + sn * (qscale * q[j]); }
    o[t * ostride + h * dv + i] = acc;
  }
}

// lab_delta_sg: ggml's gated_delta_net layout: ONE SIMDGROUP A STATE ROW, the row's dk = 128 values four to a lane in registers for the whole
// token loop, the two row reductions by simd_sum. A threadgroup is four simdgroups (four rows of one head). Same element arithmetic as the old
// kernel; only the sum's association differs. dk must be 128.
kernel void lab_delta_sg(device float* S [[buffer(0)]], device const float* c [[buffer(1)]], device const float* alpha [[buffer(2)]], device const float* beta [[buffer(3)]], device float* o [[buffer(4)]], constant uint& nv [[buffer(5)]], constant uint& dv [[buffer(6)]], constant uint& dk [[buffer(7)]], constant uint& nk [[buffer(8)]], constant float& qscale [[buffer(9)]], constant uint& cstride [[buffer(10)]], constant uint& gstride [[buffer(11)]], constant uint& ostride [[buffer(12)]], constant uint& ntok [[buffer(13)]], uint grp [[threadgroup_position_in_grid]], uint sgi [[simdgroup_index_in_threadgroup]], uint lane [[thread_index_in_simdgroup]]) {
  uint row = grp * 4u + sgi;
  if (row >= nv * dv) return;
  uint h = row / dv; uint i = row - h * dv; uint kh = h - (h / nk) * nk;
  device float4* sp = (device float4*)(S + (ulong)row * 128ul) + lane;
  float4 s = *sp;
  for (uint t = 0u; t < ntok; ++t) {
    uint co = t * cstride;
    float4 k4 = float4(((device const packed_float4*)(c + co + nk * 128u + kh * 128u))[lane]);
    float4 q4 = float4(((device const packed_float4*)(c + co + kh * 128u))[lane]);
    float v = c[co + 2u * nk * 128u + h * dv + i];
    float al = alpha[t * gstride + h]; float bt = beta[t * gstride + h];
    float4 sp4 = s * al;
    float4 pr = sp4 * k4;
    float pred = simd_sum((pr.x + pr.y) + (pr.z + pr.w));
    float err = v - pred; float bw = bt * err;
    s = sp4 + bw * k4;
    float4 ac = s * (qscale * q4);
    float accv = simd_sum((ac.x + ac.y) + (ac.z + ac.w));
    if (lane == 0u) { o[t * ostride + h * dv + i] = accv; }
  }
  *sp = s;
}
