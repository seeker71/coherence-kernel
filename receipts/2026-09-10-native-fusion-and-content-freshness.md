# Fusion and current bytes — Codex

The corpus training loop now calls Form's own Metal fusion worker. The actual
16-layer authoring adapter was loaded, 112 projection pairs were baked, and a
standalone 648-tensor model was published with 1,908,160,008 bytes. The same
public cragmoor probe returned `nothing`, with EOS, before and after fusion.
No adapter argument was passed after fusion. Zero GPU buffers remained.

The observed run spent 1,799 ms in admission, input sealing and fusion, then
14,951 ms saving and verifying the model. A second worker call read current
input and output bytes, selected reuse, and completed in 851 ms. Its stdout
names `reused-content-current` and the same generation. These are local run
measurements, not a hardware-floor claim. Projection errors are retained as
actual measured values, not hidden behind the successful text probe.

An existing directory used to bypass fusion even after learning changed the
adapter. Reuse now requires content identity for inputs, relevant native code,
and outputs. Native SHA-256 tree hashing read the full 1,807,496,278-byte base
in 342 ms. SHA compression is emitted and dispatched by Form, with shared
Metal file pages. Its explicitly named tree identity is not ordinary file SHA.
The scalar Form recipe independently witnesses padding and tree construction.

Bands returned 7 (fusion arithmetic and typed reload), 15 (content seals), 63
(freshness and real child failure), and 63 (training-loop dispatcher). The
dispatcher band was also moved to an isolated temporary corpus: it no longer
rewrites the living training files while measuring them. All checks exited 0.
The glass's observed first frame was 37 ms, within its 5,000 ms attention lane.

The remaining MLX execution crossing is Whisper large-v3-turbo. Its actual
checkpoint has 32 encoder layers, four decoder layers, width 1,280 and 128 mel
bins. The existing native tiny ear cannot be substituted as equivalent; that
model geometry and safetensors admission are the next work.

This movement kept the learning loop connected to the bytes it actually
learned. The surprising teaching was that content checking cost less than a
second for this complete model. The stale-directory shortcut was the friction;
an executable mutation witness turned it into a reusable native capability.
