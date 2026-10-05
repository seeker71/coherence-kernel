#pragma clang fp contract(off)
using namespace metal;

// lab_mm_base: q38-mm64-msl as it stands in qwen35-dense-token-handle.fk (float operands in threadgroup memory at stride 36,
// the next K block's loads held in registers across the compute, a column-a-thread output through threadgroup memory).
#define MM64_LOAD(KBX) { xa = float4(0.0f); xb = float4(0.0f); if (tt < ntok) { device const float4* xp = (device const float4*)(x + (ulong)tt * (ulong)cols + (ulong)(KBX) * 32ul + (ulong)xk); xa = xp[0]; xb = xp[1]; } if (wrow < rows) { ulong bo = ((ulong)wrow * (ulong)nb + (ulong)(KBX)) * 34ul; ushort h = (ushort)qb[bo] | ((ushort)qb[bo + 1ul] << 8); d = float(as_type<half>(h)); device const uchar* qs = qb + bo + 2ul + (ulong)(whalf * 16u); w0 = char4(*(device const packed_char4*)(qs)); w1 = char4(*(device const packed_char4*)(qs + 4u)); w2 = char4(*(device const packed_char4*)(qs + 8u)); w3 = char4(*(device const packed_char4*)(qs + 12u)); } else { d = 0.0f; w0 = char4(0); w1 = char4(0); w2 = char4(0); w3 = char4(0); } }

kernel void lab_mm_base(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) {
  threadgroup float ws[2304]; threadgroup float xs[1152];
  uint tilesN = (rows + 63u) / 64u; uint tgx = gr % tilesN; uint tgy = gr / tilesN; uint n0 = tgx * 64u; uint t0 = tgy * 32u;
  simdgroup_float8x8 acc[4][2];
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 2u; j++) { acc[i][j] = simdgroup_float8x8(0.0f); } }
  uint rl = lt >> 1; uint whalf = lt & 1u; uint wrow = n0 + rl; uint xtok = lt >> 2; uint xk = (lt & 3u) * 8u; uint nb = cols / 32u; uint tt = t0 + xtok;
  threadgroup float4* xd = (threadgroup float4*)(xs + xtok * 36u + xk);
  threadgroup float4* wd = (threadgroup float4*)(ws + rl * 36u + whalf * 16u);
  float4 xa = float4(0.0f); float4 xb = float4(0.0f); char4 w0 = char4(0); char4 w1 = char4(0); char4 w2 = char4(0); char4 w3 = char4(0); float d = 0.0f;
  MM64_LOAD(0u)
  for (uint kb = 0u; kb < nb; kb++) {
    xd[0] = xa; xd[1] = xb;
    wd[0] = float4(d * float(w0.x), d * float(w0.y), d * float(w0.z), d * float(w0.w));
    wd[1] = float4(d * float(w1.x), d * float(w1.y), d * float(w1.z), d * float(w1.w));
    wd[2] = float4(d * float(w2.x), d * float(w2.y), d * float(w2.z), d * float(w2.w));
    wd[3] = float4(d * float(w3.x), d * float(w3.y), d * float(w3.z), d * float(w3.w));
    threadgroup_barrier(mem_flags::mem_threadgroup);
    if (kb + 1u < nb) { MM64_LOAD(kb + 1u) }
    for (uint ks = 0u; ks < 4u; ks++) {
      simdgroup_float8x8 a0; simdgroup_float8x8 a1; simdgroup_float8x8 a2; simdgroup_float8x8 a3; simdgroup_float8x8 b0; simdgroup_float8x8 b1;
      simdgroup_load(a0, xs + 0u * 36u + ks * 8u, 36); simdgroup_load(a1, xs + 8u * 36u + ks * 8u, 36); simdgroup_load(a2, xs + 16u * 36u + ks * 8u, 36); simdgroup_load(a3, xs + 24u * 36u + ks * 8u, 36);
      simdgroup_load(b0, ws + (sg * 16u) * 36u + ks * 8u, 36, ulong2(0, 0), true); simdgroup_load(b1, ws + (sg * 16u + 8u) * 36u + ks * 8u, 36, ulong2(0, 0), true);
      simdgroup_multiply_accumulate(acc[0][0], a0, b0, acc[0][0]); simdgroup_multiply_accumulate(acc[0][1], a0, b1, acc[0][1]);
      simdgroup_multiply_accumulate(acc[1][0], a1, b0, acc[1][0]); simdgroup_multiply_accumulate(acc[1][1], a1, b1, acc[1][1]);
      simdgroup_multiply_accumulate(acc[2][0], a2, b0, acc[2][0]); simdgroup_multiply_accumulate(acc[2][1], a2, b1, acc[2][1]);
      simdgroup_multiply_accumulate(acc[3][0], a3, b0, acc[3][0]); simdgroup_multiply_accumulate(acc[3][1], a3, b1, acc[3][1]);
    }
    threadgroup_barrier(mem_flags::mem_threadgroup);
  }
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 2u; j++) { simdgroup_store(acc[i][j], ws + (i * 8u) * 64u + sg * 16u + j * 8u, 64); } }
  threadgroup_barrier(mem_flags::mem_threadgroup);
  uint col = lt & 63u; uint trow = lt >> 6;
  for (uint t = trow; t < 32u; t += 2u) { uint tt2 = t0 + t; uint n = n0 + col; if (tt2 < ntok && n < rows) { y[(ulong)tt2 * (ulong)ystride + (ulong)n] = ws[t * 64u + col]; } }
}

// lab_mm_ll: ggml's kernel_mul_mm shape (mul_mm.metal, the non-tensor branch), Q8_0 dequantized to half in registers: 128 threads, 2x2 simdgroups
// each 32 weight rows x 16 tokens, a K step of 32, block-major 8x8 half tiles (stride 8) in 6 KB of threadgroup memory, the left operand the
// tokens (mb) and the right the weights (ma), accumulators stored straight to device memory. The grid order is tokens fastest (x), rows slowest.
// (This kernel's group index: gr = tokenTile + nTokTiles * rowTile.)
kernel void lab_mm_ll(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) {
  threadgroup half sa[2048]; threadgroup half sb[1024];
  uint nTok = (ntok + 31u) / 32u;
  uint r1 = (gr % nTok) * 32u; uint r0 = (gr / nTok) * 64u;
  uint nb = cols / 32u;
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u;
  uint lr1 = min(lt >> 2, ntok - r1 - 1u); uint iy = 8u * (lt & 3u);
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul;
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + iy;
  simdgroup_half8x8 ma[4]; simdgroup_half8x8 mb[2]; simdgroup_float8x8 mc[8];
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); }
  for (uint kb = 0u; kb < nb; kb++) {
    device const uchar* blk = xrow + (ulong)kb * 34ul;
    ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh));
    device const uchar* qs = blk + 2u + il0 * 16u;
    threadgroup_barrier(mem_flags::mem_threadgroup);
    for (short i = 0; i < 16; i++) {
      short sx = 2 * il0 + i / 8; short sy = (lt >> 1) / 8; short lx = (lt >> 1) % 8; short ly = i % 8; short ib = 8 * sx + sy;
      sa[64 * ib + 8 * ly + lx] = half(dd * float((char)qs[i]));
    }
    {
      short sx = lt & 3; short sy = (lt >> 2) / 8; short ly = (lt >> 2) % 8; short ib = 4 * sx + sy;
      device const float4* yp = (device const float4*)(yrow + (ulong)kb * 32ul);
      float4 v0 = yp[0]; float4 v1 = yp[1];
      *(threadgroup half4*)(sb + 64 * ib + 8 * ly) = half4(v0);
      *(threadgroup half4*)(sb + 64 * ib + 8 * ly + 4) = half4(v1);
    }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    threadgroup const half* lsma = sa + 4 * 64 * (sg % 2);
    threadgroup const half* lsmb = sb + 2 * 64 * (sg / 2);
    for (short ik = 0; ik < 4; ik++) {
      for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); }
      for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); }
      for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); }
      lsma += 8 * 64; lsmb += 4 * 64;
    }
  }
  if (r0 + 64u <= rows && r1 + 32u <= ntok && ystride == rows) {
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 16u * (sg >> 1)) * (ulong)rows;
    for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)rows * (i / 4), rows, 0, false); }
  }
}
