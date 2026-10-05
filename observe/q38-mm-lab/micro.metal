#include <metal_stdlib>
#pragma clang fp contract(off)
using namespace metal;

// ---- data fills (hash-seeded, never zero: a zero operand can lower the power draw and so the clock)
kernel void lab_fill_f32(device float* x [[buffer(0)]], constant uint& n [[buffer(1)]], uint t [[thread_position_in_grid]]) {
  if (t >= n) return;
  uint h = t * 2654435761u + 12345u; h ^= h >> 15; h *= 2246822519u; h ^= h >> 13;
  x[t] = float(int(h & 0xffffu) - 32768) * (1.0f / 32768.0f);
}
kernel void lab_fill_q8(device uchar* qb [[buffer(0)]], constant uint& nblk [[buffer(1)]], uint t [[thread_position_in_grid]]) {
  if (t >= nblk) return;
  uint h = t * 2654435761u + 777u;
  half s = half(0.001f + float(h >> 28) * 0.0002f);
  ushort sb = as_type<ushort>(s);
  device uchar* p = qb + (ulong)t * 34ul;
  p[0] = (uchar)(sb & 255u); p[1] = (uchar)(sb >> 8);
  for (uint i = 0u; i < 32u; i++) { h = h * 1664525u + 1013904223u; p[2u + i] = (uchar)(h >> 24); }
}

// ---- the fp32 FMA ceiling: no matrix unit, no memory
kernel void lab_fma(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint t [[thread_position_in_grid]]) {
  float a0 = float(t) * 1e-6f, a1 = a0 + 0.1f, a2 = a0 + 0.2f, a3 = a0 + 0.3f, a4 = a0 + 0.4f, a5 = a0 + 0.5f, a6 = a0 + 0.6f, a7 = a0 + 0.7f;
  for (uint i = 0u; i < iters; i++) {
    a0 = fma(a0, 0.9999f, 1e-4f); a1 = fma(a1, 0.9999f, 1e-4f); a2 = fma(a2, 0.9999f, 1e-4f); a3 = fma(a3, 0.9999f, 1e-4f);
    a4 = fma(a4, 0.9999f, 1e-4f); a5 = fma(a5, 0.9999f, 1e-4f); a6 = fma(a6, 0.9999f, 1e-4f); a7 = fma(a7, 0.9999f, 1e-4f);
  }
  out[t] = a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7;
}
kernel void lab_fma_h(device half* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint t [[thread_position_in_grid]]) {
  half a0 = half(float(t) * 1e-6f), a1 = a0 + 0.1h, a2 = a0 + 0.2h, a3 = a0 + 0.3h, a4 = a0 + 0.4h, a5 = a0 + 0.5h, a6 = a0 + 0.6h, a7 = a0 + 0.7h;
  for (uint i = 0u; i < iters; i++) {
    a0 = fma(a0, 0.999h, 1e-3h); a1 = fma(a1, 0.999h, 1e-3h); a2 = fma(a2, 0.999h, 1e-3h); a3 = fma(a3, 0.999h, 1e-3h);
    a4 = fma(a4, 0.999h, 1e-3h); a5 = fma(a5, 0.999h, 1e-3h); a6 = fma(a6, 0.999h, 1e-3h); a7 = fma(a7, 0.999h, 1e-3h);
  }
  out[t] = a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7;
}

// ---- the matrix unit from registers: 8 accumulators, 4 + 2 operand matrices, no memory in the loop. Per iteration 8 MMAs = 8192 flop a simdgroup.
template<typename T, typename ACC>
inline void k_reg(device float* out, uint iters, uint tg, ushort sg, threadgroup ACC* scratch) {
  simdgroup_matrix<T, 8, 8> ma[4];
  simdgroup_matrix<T, 8, 8> mb[2];
  simdgroup_matrix<ACC, 8, 8> mc[8];
  float seed = 0.01f + float(iters & 1u) * 0.001f;
  for (short i = 0; i < 4; i++) { ma[i] = make_filled_simdgroup_matrix<T, 8, 8>(T(seed + float(i) * 0.001f)); }
  for (short i = 0; i < 2; i++) { mb[i] = make_filled_simdgroup_matrix<T, 8, 8>(T(seed + float(i) * 0.002f)); }
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<ACC, 8, 8>(ACC(0.0f)); }
  for (uint it = 0u; it < iters; it++) {
    for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); }
  }
  float tot = 0.0f;
  for (short i = 0; i < 8; i++) {
    simdgroup_store(mc[i], scratch + sg * 64, 8);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    tot += float(scratch[sg * 64 + 3]);
    threadgroup_barrier(mem_flags::mem_threadgroup);
  }
  if (tg == 0xffffffffu) out[sg] = tot;
}
kernel void lab_mma_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]]) { threadgroup float sc[256]; k_reg<float, float>(out, iters, tg, sg, sc); }
kernel void lab_mma_h_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]]) { threadgroup float sc[256]; k_reg<half, float>(out, iters, tg, sg, sc); }
kernel void lab_mma_h_h(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]]) { threadgroup half sc[256]; k_reg<half, half>(out, iters, tg, sg, sc); }
kernel void lab_mma_bf_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]]) { threadgroup float sc[256]; k_reg<bfloat, float>(out, iters, tg, sg, sc); }

// ---- the matrix unit with its operands loaded from threadgroup memory each K step of 8 (4 + 2 loads, 8 MMAs): the inner loop of a GEMM with no global traffic and no barrier.
// Block-major 8x8 tiles (stride 8, 64 contiguous elements a tile), the layout ggml's mul_mm uses. The base shifts by (it & 3) tiles so the loads cannot be hoisted.
template<typename T, typename ACC>
inline void k_tg(device float* out, uint iters, uint tg, ushort sg, ushort lt, threadgroup T* sa, threadgroup T* sb, threadgroup float* scratch) {
  for (uint i = lt; i < 2304u; i += 128u) { sa[i] = T(0.01f + float(i & 7u) * 0.001f); }
  for (uint i = lt; i < 1280u; i += 128u) { sb[i] = T(0.01f + float(i & 3u) * 0.001f); }
  threadgroup_barrier(mem_flags::mem_threadgroup);
  simdgroup_matrix<T, 8, 8> ma[4];
  simdgroup_matrix<T, 8, 8> mb[2];
  simdgroup_matrix<ACC, 8, 8> mc[8];
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<ACC, 8, 8>(ACC(0.0f)); }
  for (uint it = 0u; it < iters; it++) {
    threadgroup const T* lsma = sa + 4u * 64u * (sg & 1u) + 64u * (it & 3u);
    threadgroup const T* lsmb = sb + 2u * 64u * (sg >> 1) + 64u * (it & 3u);
    for (short ik = 0; ik < 4; ik++) {
      for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); }
      for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); }
      for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); }
      lsma += 8 * 64; lsmb += 4 * 64;
    }
  }
  float tot = 0.0f;
  for (short i = 0; i < 8; i++) {
    simdgroup_store(mc[i], scratch + sg * 64, 8);
    threadgroup_barrier(mem_flags::mem_threadgroup);
    tot += float(scratch[sg * 64 + 3]);
    threadgroup_barrier(mem_flags::mem_threadgroup);
  }
  if (tg == 0xffffffffu) out[sg] = tot;
}
kernel void lab_tg_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]], ushort lt [[thread_index_in_threadgroup]]) {
  threadgroup float sa[2304]; threadgroup float sb[1280]; threadgroup float sc[256]; k_tg<float, float>(out, iters, tg, sg, lt, sa, sb, sc); }
kernel void lab_tg_h_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]], ushort lt [[thread_index_in_threadgroup]]) {
  threadgroup half sa[2304]; threadgroup half sb[1280]; threadgroup float sc[256]; k_tg<half, float>(out, iters, tg, sg, lt, sa, sb, sc); }
kernel void lab_tg_bf_f32(device float* out [[buffer(0)]], constant uint& iters [[buffer(1)]], uint tg [[threadgroup_position_in_grid]], ushort sg [[simdgroup_index_in_threadgroup]], ushort lt [[thread_index_in_threadgroup]]) {
  threadgroup bfloat sa[2304]; threadgroup bfloat sb[1280]; threadgroup float sc[256]; k_tg<bfloat, float>(out, iters, tg, sg, lt, sa, sb, sc); }
