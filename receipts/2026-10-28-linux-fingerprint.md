# Linux fingerprints use the existing CPU image door

A production build compiled its sealed source successfully, then spent many
minutes in the parent build gate. A bounded CPU sample showed most sampled time
in `fk_walk`; this establishes active Form evaluation, not a blocked wait.
The build source repeatedly fingerprints artifacts during attestation and
publication. Its fingerprint image was ARM-only: an actual Linux x86-64 probe
returned `bfp-lane = 0`, with the correct digest supplied by the reference fold.

The fingerprint organ now emits a 48-byte Linux SysV image through the existing
tagged `jit_cpu_run` door and selects it with the existing ELF host witness.
The FNV-1a algorithm, 64-bit state, file spans, ARM image, witness and reference
fallback remain the same. No runtime seed or host hashing implementation changed.

Observed with C-bootstrapped fkwu:

- `./fkwu form/form-stdlib/tests/body-fingerprint-band.bml`: 127 on Darwin
  ARM64 and Linux x86-64, with acceleration witnessed on both.
- Exact agreement with the existing Form reference for text and binary bytes,
  continued state, file reads and offsets, empty/missing files, and 65,537 bytes.
  The direct machine-code zero-length branch also preserves the state.
- Linux's same `hello` probe changed from lane 0 to lane 1 while retaining
  `a430d84680aabd0b`. The 65,537-byte accelerated fold took less than one reported
  millisecond on both observed hosts; this is a sample, not a timing guarantee.
- Independent read-only review checked instruction bytes, branch offsets,
  caller-saved registers, count width and unchanged fingerprint semantics.
- `./fkwu form/form-stdlib/tests/form-cli-build-band.bml`: 262143; existing
  source identity, attestation, publication and cleanup contracts pass.

Linux proofs used a separate temporary copy of tracked source and private
runtime storage. The active rollout remained with its owner. Full deployment
timing is a separate observation; this proof establishes the missing native lane.
Implementation and review used OpenAI gpt-6-astra; these native checks made no
model calls. The useful lesson is that matching output alone can hide a missed
native path: the acceleration witness must also be read.
