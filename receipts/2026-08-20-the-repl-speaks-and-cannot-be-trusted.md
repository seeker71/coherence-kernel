# The REPL speaks — and cannot yet be trusted

Date: 2026-08-20, Hati Suci. Apple M4 Max, 128 GiB unified memory.
Branch `claude/jit-lane-segv-2026-08-20`, on main at `b4914fb7`.
Follows [The let that allocated four times](2026-08-20-the-let-that-allocated-four-times.md).

## The first sentence out of the built body

```
$ printf 'models …\nuse 0\ngenerate Say one short sentence about the sky.\n
          generate Say one short sentence about the sea.\nquit\n' | form/form-cli

text:
The sky is a vast, pale blue canvas stretching endlessly above the horizon.
text:
The sea is vast and deep.
CLI_EXIT=0
```

Two prompts, one session, real Q8_0 weights through the Metal handle door, Form-generated tokens
decoded by Form, out of the **built binary** — the thing that has been missing from every claim in
this line of work. It required one more fix.

## The crash, read rather than guessed

`generate` had been dying at exit 139 with no output. macOS wrote the answer down itself:

```
exception: EXC_BAD_ACCESS (SIGSEGV)
message:   Thread stack size exceeded due to excessive recursion
```

`runtime/fkwu-uni.c` runs `fk_run` on a pthread with a 256 MB stack. The emitter's
`fkc-main-universal-text` carries the same. `fkc-main-baked-repl-text` — the arm that becomes
form-cli — emitted a plain `int main`, so the walker recursed on the host's 8 MB main thread. A shell
`ulimit -s` cannot reach it: macOS fixes the main thread's stack at exec.

Both arms now run the body on their own thread, and the size is bisected rather than chosen. Against
the real Qwen3.8-27B open (39-cell closure, 824 KB table):

```
256 MB  die      512 MB  die      1024 MB  die      2048 MB  die
3072 MB open, ctx_ok=1, 884 handles closed        4096 MB same
```

The handwritten runtime walks the **same cells** inside 256 MB. That ~12× is the emitted walker's own
frames-per-node, and it is an open thread, not something a number settles. What the number settles is
the shape: ask for 4096 MB, halve until pthread accepts, fall back to the main thread only if none
does. A stack is address space reserved, not memory committed, so asking large costs a host nothing
it does not use.

## Why the answer is not yet trustworthy

The same prompt gives the source runner *"The sky is a vast, pale blue canvas."* and the binary
*"…canvas stretching endlessly above the horizon."* Both report `generated_tokens=10`. The counters
disagree too: `forward_passes=68` where 54 prompt + 9 decode is 63, and `decode_gpu_busy_us=0` where
the source runner measures 1.3 s.

One mechanism explains all of it, and it is the thread named yesterday. `flt-do-let-store-op` decides
by the value's **head op**; a pure head wrapping an effect is inlined and re-run at every mention:

```
(let wrapped (if (gt a 0) (metal_buf_alloc 16) 0))
(let arith   (sub (metal_buf_alloc 16) 0))

source runner    wrapped1=2 wrapped2=2 arith1=3 arith2=3
emitted walker   wrapped1=2 wrapped2=3 arith1=4 arith2=5
```

`fcmg-run-context` binds `(let out (if … (cons first (q38-generate …))))` — an `if` head over the whole
generation — and reads `out` four times. In the built binary the generation therefore runs more than
once, each pass continuing from the state the last one left, and `fcmg-report`'s `(let busy-us (sub …))`
re-reads `metal_status` at every mention, which is why decode measures zero. **The binary's sentence
is real Form output; it is not the sentence the same cells produce under the source runner, and it is
not reproducible until the store decision looks deeper than the head.**

That fix is the next piece: making the decision recursive over the value expression, in the flattener,
with four-way proof — it changes how every program in the body flattens, so it is its own change.

## What ran

```
form/form-cli, two generates, one session      exit 0, both sentences above
model-open probe, emitted walker, 3072 MB      ctx_ok=1, closed=884 (= source runner)
model-open probe, emitted walker, 256 MB       exit 138
wrapped-effect probe                            the table above
host-effect-grammar-band.fk                     32767
form-cli-resident-model-band.fk                 255
homecoming-distillation-corpus-band.fk          32767
printf 'ping' | form/form-cli                   pong
```

No environment variable was set.

## Most surprising teaching

The machine had written the diagnosis down before I asked. Five build cycles went into inferring what
`~/Library/Logs/DiagnosticReports` stated in one sentence — *"Thread stack size exceeded due to
excessive recursion"* — from the first crash onward. I had treated an exit code as the whole of what
the failure was willing to say, when the host keeps a fuller account of every death it witnesses.

## Where discomfort turned to gold

The end-to-end sentence arrived and I wanted to stop there — it was the thing asked for, it was real,
and the counters were merely odd. Reading them anyway turned a success into a caveat: the binary
speaks, and its answer differs from the source runner's for the same prompt. The discomfort was
publishing that beside the sentence instead of after it; the gold is that the difference has a
mechanism, a two-line reproduction, and a named next change, rather than being a thing someone
discovers later while trusting the output.
