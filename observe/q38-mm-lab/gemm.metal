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

// ---- ablations of lab_mm_base, one piece off at a time (the output is wrong on purpose; only the time is read).
//   DOMMA: the four K-step matrix loops;  DOGL: the global loads (off: registers from the thread id);  DOBAR: the two barriers a K step;
//   DOST: the dequantized stores to threadgroup memory (off: the tiles keep their first contents);  DOOUT: the staged output.
#define MM_LOADABL(KBX, DOGL) { if (DOGL) { xa = float4(0.0f); xb = float4(0.0f); if (tt < ntok) { device const float4* xp = (device const float4*)(x + (ulong)tt * (ulong)cols + (ulong)(KBX) * 32ul + (ulong)xk); xa = xp[0]; xb = xp[1]; } if (wrow < rows) { ulong bo = ((ulong)wrow * (ulong)nb + (ulong)(KBX)) * 34ul; ushort h = (ushort)qb[bo] | ((ushort)qb[bo + 1ul] << 8); d = float(as_type<half>(h)); device const uchar* qs = qb + bo + 2ul + (ulong)(whalf * 16u); w0 = char4(*(device const packed_char4*)(qs)); w1 = char4(*(device const packed_char4*)(qs + 4u)); w2 = char4(*(device const packed_char4*)(qs + 8u)); w3 = char4(*(device const packed_char4*)(qs + 12u)); } else { d = 0.0f; w0 = char4(0); w1 = char4(0); w2 = char4(0); w3 = char4(0); } } else { float fk = float((KBX) & 7u) * 0.001f + 0.01f; xa = float4(fk, fk + 0.001f, fk + 0.002f, fk + 0.003f); xb = xa * 1.5f; d = 0.002f; w0 = char4(int(lt & 15u) - 8, 3, -5, 7); w1 = char4(1, -2, 4, 6); w2 = char4(-7, 5, 3, 1); w3 = char4(2, 2, -3, -1); } }
#define MM_ABL(NAME, DOMMA, DOGL, DOBAR, DOST, DOOUT) kernel void NAME(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) { \
  threadgroup float ws[2304]; threadgroup float xs[1152]; \
  uint tilesN = (rows + 63u) / 64u; uint tgx = gr % tilesN; uint tgy = gr / tilesN; uint n0 = tgx * 64u; uint t0 = tgy * 32u; \
  simdgroup_float8x8 acc[4][2]; \
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 2u; j++) { acc[i][j] = simdgroup_float8x8(0.0f); } } \
  uint rl = lt >> 1; uint whalf = lt & 1u; uint wrow = n0 + rl; uint xtok = lt >> 2; uint xk = (lt & 3u) * 8u; uint nb = cols / 32u; uint tt = t0 + xtok; \
  threadgroup float4* xd = (threadgroup float4*)(xs + xtok * 36u + xk); \
  threadgroup float4* wd = (threadgroup float4*)(ws + rl * 36u + whalf * 16u); \
  float4 xa = float4(0.0f); float4 xb = float4(0.0f); char4 w0 = char4(0); char4 w1 = char4(0); char4 w2 = char4(0); char4 w3 = char4(0); float d = 0.0f; \
  for (uint i = lt; i < 2304u; i += 128u) { ws[i] = 0.01f + float(i & 7u) * 0.001f; } for (uint i = lt; i < 1152u; i += 128u) { xs[i] = 0.01f + float(i & 3u) * 0.001f; } threadgroup_barrier(mem_flags::mem_threadgroup); \
  MM_LOADABL(0u, DOGL) \
  for (uint kb = 0u; kb < nb; kb++) { \
    if (DOST) { xd[0] = xa; xd[1] = xb; \
    wd[0] = float4(d * float(w0.x), d * float(w0.y), d * float(w0.z), d * float(w0.w)); \
    wd[1] = float4(d * float(w1.x), d * float(w1.y), d * float(w1.z), d * float(w1.w)); \
    wd[2] = float4(d * float(w2.x), d * float(w2.y), d * float(w2.z), d * float(w2.w)); \
    wd[3] = float4(d * float(w3.x), d * float(w3.y), d * float(w3.z), d * float(w3.w)); } \
    if (DOBAR) { threadgroup_barrier(mem_flags::mem_threadgroup); } \
    if (kb + 1u < nb) { MM_LOADABL(kb + 1u, DOGL) } \
    if (DOMMA) { for (uint ks = 0u; ks < 4u; ks++) { \
      simdgroup_float8x8 a0; simdgroup_float8x8 a1; simdgroup_float8x8 a2; simdgroup_float8x8 a3; simdgroup_float8x8 b0; simdgroup_float8x8 b1; \
      simdgroup_load(a0, xs + 0u * 36u + ks * 8u, 36); simdgroup_load(a1, xs + 8u * 36u + ks * 8u, 36); simdgroup_load(a2, xs + 16u * 36u + ks * 8u, 36); simdgroup_load(a3, xs + 24u * 36u + ks * 8u, 36); \
      simdgroup_load(b0, ws + (sg * 16u) * 36u + ks * 8u, 36, ulong2(0, 0), true); simdgroup_load(b1, ws + (sg * 16u + 8u) * 36u + ks * 8u, 36, ulong2(0, 0), true); \
      simdgroup_multiply_accumulate(acc[0][0], a0, b0, acc[0][0]); simdgroup_multiply_accumulate(acc[0][1], a0, b1, acc[0][1]); \
      simdgroup_multiply_accumulate(acc[1][0], a1, b0, acc[1][0]); simdgroup_multiply_accumulate(acc[1][1], a1, b1, acc[1][1]); \
      simdgroup_multiply_accumulate(acc[2][0], a2, b0, acc[2][0]); simdgroup_multiply_accumulate(acc[2][1], a2, b1, acc[2][1]); \
      simdgroup_multiply_accumulate(acc[3][0], a3, b0, acc[3][0]); simdgroup_multiply_accumulate(acc[3][1], a3, b1, acc[3][1]); } } \
    if (DOBAR) { threadgroup_barrier(mem_flags::mem_threadgroup); } \
  } \
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 2u; j++) { simdgroup_store(acc[i][j], ws + (i * 8u) * 64u + sg * 16u + j * 8u, 64); } } \
  threadgroup_barrier(mem_flags::mem_threadgroup); \
  uint col = lt & 63u; uint trow = lt >> 6; \
  for (uint t = trow; t < 32u; t += 2u) { uint tt2 = t0 + t; uint n = n0 + col; if (tt2 < ntok && n < rows) { y[(ulong)tt2 * (ulong)ystride + (ulong)n] = ws[t * 64u + col] + float(DOOUT) * (xa.x + xb.y + d + float(w0.x + w1.y + w2.z + w3.w)); } } \
}
MM_ABL(lab_mm_abl_full, 1, 1, 1, 1, 1)
MM_ABL(lab_mm_abl_nomma, 0, 1, 1, 1, 1)
MM_ABL(lab_mm_abl_nogl, 1, 0, 1, 1, 1)
MM_ABL(lab_mm_abl_nobar, 1, 1, 0, 1, 1)
MM_ABL(lab_mm_abl_nost, 1, 1, 1, 0, 1)
MM_ABL(lab_mm_abl_mmaonly, 1, 0, 0, 0, 1)
MM_ABL(lab_mm_abl_glonly, 0, 1, 0, 0, 1)
MM_ABL(lab_mm_abl_glst, 0, 1, 1, 1, 1)

// ---- lab_mm_t64: a 64-row x 64-token tile. Four simdgroups in 2x2, each 32 rows x 32 tokens in sixteen 8x8 accumulators (4 + 4 operand loads for
// 16 matrix multiplies, 0.5 a multiply where the 64x32 tile's is 0.75). Each weight block is loaded and dequantized once for 64 tokens instead of
// twice. Float operands at stride 36 as in lab_mm_base. The tile leaves through the same threadgroup array, a column a thread.
#define MM_T64_LOAD(KBX) { if (wrow < rows) { ulong bo = ((ulong)wrow * (ulong)nb + (ulong)(KBX)) * 34ul; ushort h = (ushort)qb[bo] | ((ushort)qb[bo + 1ul] << 8); d = float(as_type<half>(h)); device const uchar* qs = qb + bo + 2ul + (ulong)(half16 * 16u); w0 = char4(*(device const packed_char4*)(qs)); w1 = char4(*(device const packed_char4*)(qs + 4u)); w2 = char4(*(device const packed_char4*)(qs + 8u)); w3 = char4(*(device const packed_char4*)(qs + 12u)); } else { d = 0.0f; w0 = char4(0); w1 = char4(0); w2 = char4(0); w3 = char4(0); } xa = float4(0.0f); xb = float4(0.0f); xc = float4(0.0f); xd4 = float4(0.0f); if (tt < ntok) { device const float4* xp = (device const float4*)(x + (ulong)tt * (ulong)cols + (ulong)(KBX) * 32ul + (ulong)(half16 * 16u)); xa = xp[0]; xb = xp[1]; xc = xp[2]; xd4 = xp[3]; } }

kernel void lab_mm_t64(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) {
  threadgroup float sm[4608];
  threadgroup float* ws = sm; threadgroup float* xs = sm + 2304;
  uint tilesN = (rows + 63u) / 64u; uint tgx = gr % tilesN; uint tgy = gr / tilesN; uint n0 = tgx * 64u; uint t0 = tgy * 64u;
  simdgroup_float8x8 acc[4][4];
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 4u; j++) { acc[i][j] = simdgroup_float8x8(0.0f); } }
  uint rl = lt >> 1; uint half16 = lt & 1u; uint wrow = n0 + rl; uint tt = t0 + rl; uint nb = cols / 32u;
  threadgroup float4* xdst = (threadgroup float4*)(xs + rl * 36u + half16 * 16u);
  threadgroup float4* wdst = (threadgroup float4*)(ws + rl * 36u + half16 * 16u);
  float4 xa = float4(0.0f); float4 xb = float4(0.0f); float4 xc = float4(0.0f); float4 xd4 = float4(0.0f); char4 w0 = char4(0); char4 w1 = char4(0); char4 w2 = char4(0); char4 w3 = char4(0); float d = 0.0f;
  MM_T64_LOAD(0u)
  uint sr = (sg & 1u) * 32u; uint st = (sg >> 1) * 32u;
  for (uint kb = 0u; kb < nb; kb++) {
    xdst[0] = xa; xdst[1] = xb; xdst[2] = xc; xdst[3] = xd4;
    wdst[0] = float4(d * float(w0.x), d * float(w0.y), d * float(w0.z), d * float(w0.w));
    wdst[1] = float4(d * float(w1.x), d * float(w1.y), d * float(w1.z), d * float(w1.w));
    wdst[2] = float4(d * float(w2.x), d * float(w2.y), d * float(w2.z), d * float(w2.w));
    wdst[3] = float4(d * float(w3.x), d * float(w3.y), d * float(w3.z), d * float(w3.w));
    threadgroup_barrier(mem_flags::mem_threadgroup);
    if (kb + 1u < nb) { MM_T64_LOAD(kb + 1u) }
    for (uint ks = 0u; ks < 4u; ks++) {
      simdgroup_float8x8 a0; simdgroup_float8x8 a1; simdgroup_float8x8 a2; simdgroup_float8x8 a3;
      simdgroup_float8x8 b0; simdgroup_float8x8 b1; simdgroup_float8x8 b2; simdgroup_float8x8 b3;
      simdgroup_load(a0, xs + (st + 0u) * 36u + ks * 8u, 36); simdgroup_load(a1, xs + (st + 8u) * 36u + ks * 8u, 36); simdgroup_load(a2, xs + (st + 16u) * 36u + ks * 8u, 36); simdgroup_load(a3, xs + (st + 24u) * 36u + ks * 8u, 36);
      simdgroup_load(b0, ws + (sr + 0u) * 36u + ks * 8u, 36, ulong2(0, 0), true); simdgroup_load(b1, ws + (sr + 8u) * 36u + ks * 8u, 36, ulong2(0, 0), true);
      simdgroup_load(b2, ws + (sr + 16u) * 36u + ks * 8u, 36, ulong2(0, 0), true); simdgroup_load(b3, ws + (sr + 24u) * 36u + ks * 8u, 36, ulong2(0, 0), true);
      simdgroup_multiply_accumulate(acc[0][0], a0, b0, acc[0][0]); simdgroup_multiply_accumulate(acc[0][1], a0, b1, acc[0][1]); simdgroup_multiply_accumulate(acc[0][2], a0, b2, acc[0][2]); simdgroup_multiply_accumulate(acc[0][3], a0, b3, acc[0][3]);
      simdgroup_multiply_accumulate(acc[1][0], a1, b0, acc[1][0]); simdgroup_multiply_accumulate(acc[1][1], a1, b1, acc[1][1]); simdgroup_multiply_accumulate(acc[1][2], a1, b2, acc[1][2]); simdgroup_multiply_accumulate(acc[1][3], a1, b3, acc[1][3]);
      simdgroup_multiply_accumulate(acc[2][0], a2, b0, acc[2][0]); simdgroup_multiply_accumulate(acc[2][1], a2, b1, acc[2][1]); simdgroup_multiply_accumulate(acc[2][2], a2, b2, acc[2][2]); simdgroup_multiply_accumulate(acc[2][3], a2, b3, acc[2][3]);
      simdgroup_multiply_accumulate(acc[3][0], a3, b0, acc[3][0]); simdgroup_multiply_accumulate(acc[3][1], a3, b1, acc[3][1]); simdgroup_multiply_accumulate(acc[3][2], a3, b2, acc[3][2]); simdgroup_multiply_accumulate(acc[3][3], a3, b3, acc[3][3]);
    }
    threadgroup_barrier(mem_flags::mem_threadgroup);
  }
  for (uint i = 0u; i < 4u; i++) { for (uint j = 0u; j < 4u; j++) { simdgroup_store(acc[i][j], sm + (st + i * 8u) * 64u + sr + j * 8u, 64); } }
  threadgroup_barrier(mem_flags::mem_threadgroup);
  uint col = lt & 63u; uint trow = lt >> 6;
  for (uint t = trow; t < 64u; t += 2u) { uint tt2 = t0 + t; uint n = n0 + col; if (tt2 < ntok && n < rows) { y[(ulong)tt2 * (ulong)ystride + (ulong)n] = sm[t * 64u + col]; } }
}

// ---- lab_mm_ll with the tile type a parameter: lab_mm_llf keeps ggml's whole shape (tokens-fast order, block-major 8x8 tiles, 2x2 simdgroups
// of 32 rows x 16 tokens, direct device stores) but holds the tiles in FLOAT, so the exactness of fp32 operands is not given up.
#define MM_LL(NAME, TS, TS4) kernel void NAME(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) { \
  threadgroup TS sa[2048]; threadgroup TS sb[1024]; \
  uint nTok = (ntok + 31u) / 32u; \
  uint r1 = (gr % nTok) * 32u; uint r0 = (gr / nTok) * 64u; \
  uint nb = cols / 32u; \
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u; \
  uint lr1 = min(lt >> 2, ntok - r1 - 1u); uint iy = 8u * (lt & 3u); \
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul; \
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + iy; \
  simdgroup_matrix<TS, 8, 8> ma[4]; simdgroup_matrix<TS, 8, 8> mb[2]; simdgroup_float8x8 mc[8]; \
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); } \
  for (uint kb = 0u; kb < nb; kb++) { \
    device const uchar* blk = xrow + (ulong)kb * 34ul; \
    ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh)); \
    device const uchar* qs = blk + 2u + il0 * 16u; \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    for (short i = 0; i < 16; i++) { \
      short sx = 2 * il0 + i / 8; short sy = (lt >> 1) / 8; short lx = (lt >> 1) % 8; short ly = i % 8; short ib = 8 * sx + sy; \
      sa[64 * ib + 8 * ly + lx] = TS(dd * float((char)qs[i])); \
    } \
    { short sx = lt & 3; short sy = (lt >> 2) / 8; short ly = (lt >> 2) % 8; short ib = 4 * sx + sy; \
      device const float4* yp = (device const float4*)(yrow + (ulong)kb * 32ul); \
      float4 v0 = yp[0]; float4 v1 = yp[1]; \
      *(threadgroup TS4*)(sb + 64 * ib + 8 * ly) = TS4(v0); \
      *(threadgroup TS4*)(sb + 64 * ib + 8 * ly + 4) = TS4(v1); } \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    threadgroup const TS* lsma = sa + 4 * 64 * (sg % 2); \
    threadgroup const TS* lsmb = sb + 2 * 64 * (sg / 2); \
    for (short ik = 0; ik < 4; ik++) { \
      for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); } \
      lsma += 8 * 64; lsmb += 4 * 64; \
    } \
  } \
  if (r0 + 64u <= rows && r1 + 32u <= ntok && ystride == rows) { \
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 16u * (sg >> 1)) * (ulong)rows; \
    for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)rows * (i / 4), rows, 0, false); } \
  } \
}
MM_LL(lab_mm_llf, float, float4)
MM_LL(lab_mm_llh, half, half4)

// ---- lab_mm_llk: the lab_mm_llf/llh shape with NKB Q8_0 blocks (K = 32 * NKB) a barrier pair.
#define MM_LLK(NAME, TS, TS4, NKB) kernel void NAME(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) { \
  threadgroup TS sa[2048 * NKB]; threadgroup TS sb[1024 * NKB]; \
  uint nTok = (ntok + 31u) / 32u; \
  uint r1 = (gr % nTok) * 32u; uint r0 = (gr / nTok) * 64u; \
  uint nb = cols / 32u; \
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u; \
  uint lr1 = min(lt >> 2, ntok - r1 - 1u); uint iy = 8u * (lt & 3u); \
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul; \
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + iy; \
  simdgroup_matrix<TS, 8, 8> ma[4]; simdgroup_matrix<TS, 8, 8> mb[2]; simdgroup_float8x8 mc[8]; \
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); } \
  for (uint kb = 0u; kb < nb; kb += NKB) { \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    for (short j = 0; j < NKB; j++) { \
      device const uchar* blk = xrow + (ulong)(kb + j) * 34ul; \
      ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh)); \
      device const uchar* qs = blk + 2u + il0 * 16u; \
      for (short i = 0; i < 16; i++) { \
        short sx = 4 * j + 2 * il0 + i / 8; short sy = (lt >> 1) / 8; short lx = (lt >> 1) % 8; short ly = i % 8; short ib = 8 * sx + sy; \
        sa[64 * ib + 8 * ly + lx] = TS(dd * float((char)qs[i])); \
      } \
      { short sx = 4 * j + (lt & 3); short sy = (lt >> 2) / 8; short ly = (lt >> 2) % 8; short ib = 4 * sx + sy; \
        device const float4* yp = (device const float4*)(yrow + (ulong)(kb + j) * 32ul); \
        float4 v0 = yp[0]; float4 v1 = yp[1]; \
        *(threadgroup TS4*)(sb + 64 * ib + 8 * ly) = TS4(v0); \
        *(threadgroup TS4*)(sb + 64 * ib + 8 * ly + 4) = TS4(v1); } \
    } \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    threadgroup const TS* lsma = sa + 4 * 64 * (sg % 2); \
    threadgroup const TS* lsmb = sb + 2 * 64 * (sg / 2); \
    for (short ik = 0; ik < 4 * NKB; ik++) { \
      for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); } \
      lsma += 8 * 64; lsmb += 4 * 64; \
    } \
  } \
  if (r0 + 64u <= rows && r1 + 32u <= ntok && ystride == rows) { \
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 16u * (sg >> 1)) * (ulong)rows; \
    for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)rows * (i / 4), rows, 0, false); } \
  } \
}
MM_LLK(lab_mm_llf_k2, float, float4, 2)
MM_LLK(lab_mm_llh_k2, half, half4, 2)

// ---- lab_mm_lld: double-buffered tiles, one barrier a K step: the next block's loads are in registers before the compute and are stored into the
// other buffer after it. (Shared memory twice a tile.)
#define MM_LLD(NAME, TS, TS4) kernel void NAME(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) { \
  threadgroup TS sa[4096]; threadgroup TS sb[2048]; \
  uint nTok = (ntok + 31u) / 32u; \
  uint r1 = (gr % nTok) * 32u; uint r0 = (gr / nTok) * 64u; \
  uint nb = cols / 32u; \
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u; \
  uint lr1 = min(lt >> 2, ntok - r1 - 1u); uint iy = 8u * (lt & 3u); \
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul; \
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + iy; \
  short asy = (lt >> 1) / 8; short alx = (lt >> 1) % 8; short bsx = lt & 3; short bsy = (lt >> 2) / 8; short bly = (lt >> 2) % 8; \
  simdgroup_matrix<TS, 8, 8> ma[4]; simdgroup_matrix<TS, 8, 8> mb[2]; simdgroup_float8x8 mc[8]; \
  for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); } \
  float dd = 0.0f; char4 w0 = char4(0); char4 w1 = char4(0); char4 w2 = char4(0); char4 w3 = char4(0); float4 v0 = float4(0.0f); float4 v1 = float4(0.0f); \
  { device const uchar* blk = xrow; ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); dd = float(as_type<half>(hh)); device const uchar* qs = blk + 2u + il0 * 16u; \
    w0 = char4(*(device const packed_char4*)(qs)); w1 = char4(*(device const packed_char4*)(qs + 4u)); w2 = char4(*(device const packed_char4*)(qs + 8u)); w3 = char4(*(device const packed_char4*)(qs + 12u)); \
    device const float4* yp = (device const float4*)yrow; v0 = yp[0]; v1 = yp[1]; } \
  for (uint kb = 0u; kb < nb; kb++) { \
    threadgroup TS* sad = sa + (kb & 1u) * 2048u; threadgroup TS* sbd = sb + (kb & 1u) * 1024u; \
    for (short i = 0; i < 16; i++) { short sx = 2 * il0 + i / 8; short ib = 8 * sx + asy; short ly = i % 8; float q = (i < 4) ? float(w0[i & 3]) : ((i < 8) ? float(w1[i & 3]) : ((i < 12) ? float(w2[i & 3]) : float(w3[i & 3]))); sad[64 * ib + 8 * ly + alx] = TS(dd * q); } \
    { short ib = 4 * bsx + bsy; *(threadgroup TS4*)(sbd + 64 * ib + 8 * bly) = TS4(v0); *(threadgroup TS4*)(sbd + 64 * ib + 8 * bly + 4) = TS4(v1); } \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    if (kb + 1u < nb) { device const uchar* blk = xrow + (ulong)(kb + 1u) * 34ul; ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); dd = float(as_type<half>(hh)); device const uchar* qs = blk + 2u + il0 * 16u; \
      w0 = char4(*(device const packed_char4*)(qs)); w1 = char4(*(device const packed_char4*)(qs + 4u)); w2 = char4(*(device const packed_char4*)(qs + 8u)); w3 = char4(*(device const packed_char4*)(qs + 12u)); \
      device const float4* yp = (device const float4*)(yrow + (ulong)(kb + 1u) * 32ul); v0 = yp[0]; v1 = yp[1]; } \
    threadgroup const TS* lsma = sad + 4 * 64 * (sg % 2); \
    threadgroup const TS* lsmb = sbd + 2 * 64 * (sg / 2); \
    for (short ik = 0; ik < 4; ik++) { \
      for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); } \
      for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); } \
      lsma += 8 * 64; lsmb += 4 * 64; \
    } \
  } \
  if (r0 + 64u <= rows && r1 + 32u <= ntok && ystride == rows) { \
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 16u * (sg >> 1)) * (ulong)rows; \
    for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)rows * (i / 4), rows, 0, false); } \
  } \
}
MM_LLD(lab_mm_lldf, float, float4)
MM_LLD(lab_mm_lldh, half, half4)

// ---- lab_mm_llt64f: ggml's block-major tiles, FLOAT, a 64-row x 64-token tile: four simdgroups in 2x2, each 32 rows x 32 tokens in sixteen accumulators
// (4 + 4 operand loads for 16 multiplies). Tokens-fast group order. Full tiles store directly; the name carries t64 so the lab launches (rows/64) x (ntok/64) groups.
#define MM_LL64(NAME, TS, TS4) kernel void NAME(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) { \
  threadgroup TS sa[2048]; threadgroup TS sb[2048]; \
  uint nTok = (ntok + 63u) / 64u; \
  uint r1 = (gr % nTok) * 64u; uint r0 = (gr / nTok) * 64u; \
  uint nb = cols / 32u; \
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u; \
  uint lr1 = min(lt >> 1, ntok - r1 - 1u); \
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul; \
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + il0 * 16u; \
  simdgroup_matrix<TS, 8, 8> ma[4]; simdgroup_matrix<TS, 8, 8> mb[4]; simdgroup_float8x8 mc[16]; \
  _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); } \
  for (uint kb = 0u; kb < nb; kb++) { \
    device const uchar* blk = xrow + (ulong)kb * 34ul; \
    ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh)); \
    device const uchar* qs = blk + 2u + il0 * 16u; \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { \
      short sx = 2 * il0 + i / 8; short sy = (lt >> 1) / 8; short lx = (lt >> 1) % 8; short ly = i % 8; short ib = 8 * sx + sy; \
      sa[64 * ib + 8 * ly + lx] = TS(dd * float((char)qs[i])); \
    } \
    { device const float4* yp = (device const float4*)(yrow + (ulong)kb * 32ul); \
      short sy = (lt >> 1) / 8; short ly = (lt >> 1) % 8; \
      _Pragma("clang loop unroll(full)") for (short h = 0; h < 2; h++) { short sx = 2 * il0 + h; short ib = 8 * sx + sy; \
        float4 v0 = yp[2 * h]; float4 v1 = yp[2 * h + 1]; \
        *(threadgroup TS4*)(sb + 64 * ib + 8 * ly) = TS4(v0); \
        *(threadgroup TS4*)(sb + 64 * ib + 8 * ly + 4) = TS4(v1); } } \
    threadgroup_barrier(mem_flags::mem_threadgroup); \
    threadgroup const TS* lsma = sa + 4 * 64 * (sg % 2); \
    threadgroup const TS* lsmb = sb + 4 * 64 * (sg / 2); \
    _Pragma("clang loop unroll(full)") for (short ik = 0; ik < 4; ik++) { \
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); } \
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 4; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); } \
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); } \
      lsma += 8 * 64; lsmb += 8 * 64; \
    } \
  } \
  if (r0 + 64u <= rows && r1 + 64u <= ntok && ystride == rows) { \
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 32u * (sg >> 1)) * (ulong)rows; \
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)rows * (i / 4), rows, 0, false); } \
  } \
}
MM_LL64(lab_mm_llt64f, float, float4)
MM_LL64(lab_mm_llt64h, half, half4)

// ---- lab_mm_t64e: lab_mm_llt64f made whole: any rows, any ntok, any output pitch (ystride). Full tiles store straight to device memory from the
// accumulators; a partial tile goes through threadgroup memory, a column a thread, guarded. This is the candidate for the lane.
kernel void lab_mm_t64e(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) {
  threadgroup float sm[4096];
  threadgroup float* sa = sm; threadgroup float* sb = sm + 2048;
  uint nTok = (ntok + 63u) / 64u;
  uint r1 = (gr % nTok) * 64u; uint r0 = (gr / nTok) * 64u;
  uint nb = cols / 32u;
  uint lr0 = min(lt >> 1, rows - r0 - 1u); uint il0 = lt & 1u;
  uint lr1 = min(lt >> 1, ntok - r1 - 1u);
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul;
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + il0 * 16u;
  simdgroup_float8x8 ma[4]; simdgroup_float8x8 mb[4]; simdgroup_float8x8 mc[16];
  _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); }
  for (uint kb = 0u; kb < nb; kb++) {
    device const uchar* blk = xrow + (ulong)kb * 34ul;
    ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh));
    device const uchar* qs = blk + 2u + il0 * 16u;
    threadgroup_barrier(mem_flags::mem_threadgroup);
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) {
      short sx = 2 * il0 + i / 8; short sy = (lt >> 1) / 8; short lx = (lt >> 1) % 8; short ly = i % 8; short ib = 8 * sx + sy;
      sa[64 * ib + 8 * ly + lx] = dd * float((char)qs[i]);
    }
    { device const float4* yp = (device const float4*)(yrow + (ulong)kb * 32ul);
      short sy = (lt >> 1) / 8; short ly = (lt >> 1) % 8;
      _Pragma("clang loop unroll(full)") for (short h = 0; h < 2; h++) { short sx = 2 * il0 + h; short ib = 8 * sx + sy;
        float4 v0 = yp[2 * h]; float4 v1 = yp[2 * h + 1];
        *(threadgroup float4*)(sb + 64 * ib + 8 * ly) = v0;
        *(threadgroup float4*)(sb + 64 * ib + 8 * ly + 4) = v1; } }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    threadgroup const float* lsma = sa + 4 * 64 * (sg % 2);
    threadgroup const float* lsmb = sb + 4 * 64 * (sg / 2);
    _Pragma("clang loop unroll(full)") for (short ik = 0; ik < 4; ik++) {
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); }
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 4; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); }
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); }
      lsma += 8 * 64; lsmb += 8 * 64;
    }
  }
  if (r0 + 64u <= rows && r1 + 64u <= ntok) {
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 32u * (sg >> 1)) * (ulong)ystride;
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)ystride * (i / 4), ystride, 0, false); }
  } else {
    threadgroup_barrier(mem_flags::mem_threadgroup);
    threadgroup float* temp = sm + 32 * (sg & 1u) + (32 * (sg >> 1)) * 64;
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 16; i++) { simdgroup_store(mc[i], temp + 8 * (i % 4) + 8 * 64 * (i / 4), 64, 0, false); }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    uint col = lt & 63u; uint trow = lt >> 6;
    for (uint t = trow; t < 64u; t += 2u) { uint tt = r1 + t; uint n = r0 + col; if (tt < ntok && n < rows) { y[(ulong)tt * (ulong)ystride + (ulong)n] = sm[t * 64u + col]; } }
  }
}

// ---- lab_mm_t64g256e: a 64-row x 64-token tile, 256 threads: eight simdgroups, each 32 rows x 16 tokens in eight accumulators (ggml's per-simdgroup shape,
// 4 + 2 loads for 8 multiplies), twice the resident simdgroups of lab_mm_t64e for the same tile. Four threads a weight row / token, 8 values each.
kernel void lab_mm_t64g256e(device const uchar* qb [[buffer(0)]], device const float* x [[buffer(1)]], device float* y [[buffer(2)]], constant uint& rows [[buffer(3)]], constant uint& cols [[buffer(4)]], constant uint& ntok [[buffer(5)]], constant uint& ystride [[buffer(6)]], uint gr [[threadgroup_position_in_grid]], uint lt [[thread_index_in_threadgroup]], uint sg [[simdgroup_index_in_threadgroup]]) {
  threadgroup float sm[4096];
  threadgroup float* sa = sm; threadgroup float* sb = sm + 2048;
  uint nTok = (ntok + 63u) / 64u;
  uint r1 = (gr % nTok) * 64u; uint r0 = (gr / nTok) * 64u;
  uint nb = cols / 32u;
  uint lr0 = min(lt >> 2, rows - r0 - 1u); uint q4 = lt & 3u;
  uint lr1 = min(lt >> 2, ntok - r1 - 1u);
  device const uchar* xrow = qb + (ulong)(r0 + lr0) * (ulong)nb * 34ul;
  device const float* yrow = x + (ulong)(r1 + lr1) * (ulong)cols + q4 * 8u;
  simdgroup_float8x8 ma[4]; simdgroup_float8x8 mb[2]; simdgroup_float8x8 mc[8];
  _Pragma("clang loop unroll(full)") for (short i = 0; i < 8; i++) { mc[i] = make_filled_simdgroup_matrix<float, 8>(0.0f); }
  uint sy = (lt >> 2) / 8u; uint lx = (lt >> 2) % 8u;
  for (uint kb = 0u; kb < nb; kb++) {
    device const uchar* blk = xrow + (ulong)kb * 34ul;
    ushort hh = (ushort)blk[0] | ((ushort)blk[1] << 8); float dd = float(as_type<half>(hh));
    device const uchar* qs = blk + 2u + q4 * 8u;
    threadgroup_barrier(mem_flags::mem_threadgroup);
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 8; i++) {
      sa[64u * (8u * q4 + sy) + 8u * (uint)i + lx] = dd * float((char)qs[i]);
    }
    { device const float4* yp = (device const float4*)(yrow + (ulong)kb * 32ul);
      float4 v0 = yp[0]; float4 v1 = yp[1];
      *(threadgroup float4*)(sb + 64u * (8u * q4 + sy) + 8u * lx) = v0;
      *(threadgroup float4*)(sb + 64u * (8u * q4 + sy) + 8u * lx + 4u) = v1; }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    threadgroup const float* lsma = sa + 4 * 64 * (sg & 1u);
    threadgroup const float* lsmb = sb + 2 * 64 * (sg >> 1);
    _Pragma("clang loop unroll(full)") for (short ik = 0; ik < 4; ik++) {
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 4; i++) { simdgroup_load(ma[i], lsma + 64 * i, 8, 0, false); }
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 2; i++) { simdgroup_load(mb[i], lsmb + 64 * i, 8, 0, false); }
      _Pragma("clang loop unroll(full)") for (short i = 0; i < 8; i++) { simdgroup_multiply_accumulate(mc[i], mb[i / 4], ma[i % 4], mc[i]); }
      lsma += 8 * 64; lsmb += 8 * 64;
    }
  }
  if (r0 + 64u <= rows && r1 + 64u <= ntok) {
    device float* C = y + (r0 + 32u * (sg & 1u)) + (ulong)(r1 + 16u * (sg >> 1)) * (ulong)ystride;
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], C + 8 * (i % 4) + 8 * (ulong)ystride * (i / 4), ystride, 0, false); }
  } else {
    threadgroup_barrier(mem_flags::mem_threadgroup);
    threadgroup float* temp = sm + 32 * (sg & 1u) + (16 * (sg >> 1)) * 64;
    _Pragma("clang loop unroll(full)") for (short i = 0; i < 8; i++) { simdgroup_store(mc[i], temp + 8 * (i % 4) + 8 * 64 * (i / 4), 64, 0, false); }
    threadgroup_barrier(mem_flags::mem_threadgroup);
    uint col = lt & 63u; uint trow = lt >> 6;
    for (uint t = trow; t < 64u; t += 4u) { uint tt = r1 + t; uint n = r0 + col; if (tt < ntok && n < rows) { y[(ulong)tt * (ulong)ystride + (ulong)n] = sm[t * 64u + col]; } }
  }
}
