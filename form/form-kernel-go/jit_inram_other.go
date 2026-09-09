//go:build !(darwin && arm64 && cgo) && !windows

// jit_inram_other.go — the in-RAM JIT executor is darwin/arm64 + cgo only
// (it needs MAP_JIT + pthread_jit_write_protect_np). On every other target the
// `jit_leaf_inram` native is absent. The proof interpreter remains available;
// native compilation and target selection belong to Form running on fkwu.
// This keeps the kernel pure-Go and cross-buildable on Linux (CI, the VPS).

package main

// registerInRAMJIT — no-op: no in-process arm64 executor on this target.
func (k *Kernel) registerInRAMJIT() {}
