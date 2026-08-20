# The write that said yes — where the built body loses every byte

Date: 2026-08-19, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Worktree `.claude/worktrees/jit-lane-performance-d77568`.
Follows [The build moves the path](2026-08-19-the-build-moves-the-path.md).

Urs, reading the last receipt: *"meaning it was not actually showing real end to end multiple tool
usages and form tokens being used to answer and show the response"*. Correct, and it is worth saying
without softening.

## What had and had not been shown

**Shown, and real:** Form-native generation on this machine — 54 prompt tokens, ten generated tokens,
27.2 GB of Q8_0 weights per forward through the Metal handle door, decoded to *"The sky is a vast,
pale blue canvas."* Real weights, real kernels, real text. And the CLI's own turn handler answering
`generate Reply with exactly: hello` with `hello`, threading its residence across turns.

**Not shown:** any of it coming out of the **built `form-cli` binary** as an answer to a person typing
a line. Everything above ran under `./fkwu`, the source runner. The distinction was named in the last
receipt as a stale binary; it is sharper than that, and this receipt is the sharpening.

## Two hypotheses raised and killed by measurement

**`let` is call-by-name in the flattened program.** It would have explained every symptom at once —
handles reallocated per use, buffers piling up, reads landing on fresh memory. Tested at top level and
again inside a `defn` body, source runner against the flattened table walked by the emitted walker:

```
source runner        in_first=one in_second=one in_b=two
emitted walker       in_first=one in_second=one in_b=two
```

Bound once in both. **Refuted.**

**The digest is wrong because the file is 29 GB.** A three-byte known-answer test settled it: the
binary returns zeros for `"abc"` too.

## Where it actually breaks

`sha-selftest` — one 64-byte block, no file, every step printed. Same cell, two kernels:

| | source runner `./fkwu` | built `form-cli` |
|---|---|---|
| `state_handle / rounds / msg` | 1 / 2 / 3 | 1 / 2 / 3 |
| `pad_len` | 64 | 64 |
| `state_write / rounds_write / msg_write` | 32 / 256 / 64 | **32 / 256 / 64** |
| `pipeline` / `enqueue` | 1 / 1 | **1 / 1** |
| `msg_readback` | `61626380…0018` | **`0000…0000`** |
| `state_readback` | `bf1678ba…ad1500f2` | **`0000…0000`** |
| `digest` | `ba7816bf…0015ad` ✓ | **`0000…0000`** |
| `last_error` | none | **none** |

**Every call succeeds. Every byte is lost.** `metal_buf_write` is handed a 64-byte Form string,
answers 64, and the same 64 bytes read back as zeros. Nothing errors, nothing warns, and the SHA
kernel then hashes an empty buffer and writes its result nowhere — which is why the enqueue also
returns 1 and the state comes back zeroed. Two observations narrow it further: the kernel NAME
`"form_cpu_jit"` — a literal — reaches the carrier intact, since the CPU-JIT branch is taken and the
pipeline compiles; every payload that arrives as zeros was built at runtime by `str_concat` /
`byte_to_str`. That is a lead, not a verdict: no literal payload was tested inside the binary, because
the only Metal-capable emitted walker on this host is form-cli itself — `bootstrap/fkwu-uni.c` carries
no Metal bridge at all (`fk_metal_buf_alloc_external`: 0 occurrences, against 2 in
`form-cli-emitted.c`), so every iteration costs a full four-and-a-half-minute regeneration.

The op dispatch is not the divergence: the emitted C's `t == 248` arm is character-for-character the
runtime's, and the string bridge `fk_srange` is identical in both.

Reproduce in one command:

```
printf 'sha-selftest\nquit\n' | form/form-cli
```

## The honest standing

The Form-native model lane generates real tokens on this machine, through the same cells the CLI
calls, under the source runner. The built binary now carries those cells — the build moved 1005
functions into the program — and it cannot yet use them, because every Form-built byte it hands to
the Metal door arrives as zeros. That is one bug, named, reproducible, and in the emitted walker's
string-to-native path, not in the model lane.

## What ran

```
form/form-stdlib/tests/form-cli-resident-model-band.fk   -> 255
learn/tests/homecoming-distillation-corpus-band.fk       -> 32767   (row 1030)
observe/tmp letprobe (top-level and defn-body)            let bound once in BOTH kernels
observe/qwen38-seal-check-run.fk                          correct digest, source runner
printf 'sha-selftest' | form/form-cli                     the table above
```

No environment variable was set.

## Most surprising teaching

A door can answer correctly at every single step and still carry nothing. Allocation returned a
handle, the write returned its exact byte count, the pipeline compiled, the dispatch returned one, the
read returned thirty-two bytes, and `last_error` said none — and the payload was never there. Every
instrument along the path was green because every instrument was reporting on the *protocol* rather
than on the *cargo*. The body's law that a green instrument cannot say no has a sharper edge than I
had understood: an instrument can be honest about what it measures and still be silent about what
matters.

## Where discomfort turned to gold

I wanted the call-by-name hypothesis to be true — it was elegant, it explained everything, and it
would have been mine. Testing it cost two probes and killed it in a minute. Then the three-byte test
killed the second story. What was left was slower and less satisfying: print every step and read.
The discomfort was watching two good explanations die and having to keep going with no story at all;
the gold is a bug that no longer needs a story, because `msg_write=64` next to `msg_readback=0000…`
says it without one.
