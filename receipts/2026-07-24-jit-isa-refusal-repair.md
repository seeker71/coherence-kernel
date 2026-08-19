# JIT ISA refusal and control-state repair — 2026-07-24

The live `FK_JIT` path on Apple arm64 was observed announcing an x86-64
crystallization and then returning `nothing`. The checkout seed's
`fk_jit_lower` emits x86-64 bytes, while both executable-memory carriers
correctly refuse those bytes on arm64.

The temporary C seed now refuses that lowerer on non-x86 hosts before
incrementing `njit` or claiming native dispatch. This is a checkout-witness
repair, not a new compiler implementation. ARM64 lowering remains owned by the
Form JIT cells; the next shrink removes this guard together with the x86 C
lowerer when target selection and Form-emitted bytes feed the W^X carrier
directly.

The host seam now also exposes `native_call_bytes(image, arg)`. It validates a
Form list as bytes, installs those bytes RW→RX for the current ISA, clears the
instruction cache, and calls them with one tagged Value-ABI argument. It does
not lower an operation or choose an ISA. `form-asm-live-arm64-band.fk` supplies
the ARM64 image from `form-asm.fk`; this carrier is therefore a shrink target
only to the extent that the same generic W^X door moves out of the temporary C
seed into the permanent host membrane.

The first repair put JIT enablement and DS4 diagnostic values in
`fkwu.conf`. Re-inquiry showed those were not configuration:

- an available JIT path is attempted from the live recipe and current ISA;
- layer count comes from the GGUF metadata;
- model and prompt are inputs to an invocation;
- re-witness is an explicit invocation mode.

The config file was removed. Direct source execution now attempts eligible JIT
lowering without an enable switch. The DS4 stack takes
`<model.gguf> <prompt-token> [--rewitness]`, reads all 43 layers from the file,
and has no environment or config control.

Fresh observations after that removal:

- binary freshness: `15`
- Form-emitted ARM64 image called through W^X: `42`
- recursive JIT health/fallback: `5050`
- full explicit-input DS4 stack: `92/92` live gates, token `19129`, all
  `129280/129280` logits finite, no scalar oracle, no Go runtime.
