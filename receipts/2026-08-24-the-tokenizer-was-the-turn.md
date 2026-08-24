# 2026-08-24 — the largest part was the tokenizer, and it costs 708x the retrieval

I named 167.9 s as the largest block and unexplained, then spent the reply on
the 81.1 s one. Yes asked the obvious question. Here is the larger part.

## Eight attributions, and the ninth

Falsified, each in minutes: barriers, dispatch count, weight-traffic batching,
binding construction (6 µs each, 3.8 s per turn), paging (137 GB against a 29 GB
model), per-dispatch overhead (a warm forward's whole wall-versus-GPU gap is
11 ms, so 506 of them is 5.6 s).

What a turn does that a bare forward never does is **cross between bytes and
token ids**. And `gguf-meta.fk:116` says exactly what that costs, in its own
comment:

> the walk's cursor: given one token's offset, the next one's. A caller streaming
> the whole vocabulary pays ONE linear walk, **not one walk per token**.

The caller is not streaming.

- `gmt-token-off id` → `gmt-arr-elem` → `gmt-arr-walk` — a linear walk of `id`
  length-prefixed strings, per decode.
- `tkz-id-of sym` → `tkz-id-scan` — "ONE streaming pass over the whole vocab",
  **per symbol**, and the vocabulary is **248,320 entries**.

## Measured, no GPU involved

Decode, ten at a time:

```
id        100     1000    10000    50000   100000
ms/10       2        8       69      340      678
```

Exactly linear: 0.678 ms per thousand ids of walk.

Encode:

```
50 bytes   ->  18 tokens  ->    2,286 ms   (127 ms/token)
957 bytes  -> 298 tokens  ->  120,385 ms   (403 ms/token)
```

**Encoding one 957-byte observation takes 120.4 seconds.** The real injected
observation is 1072 bytes and 385 tokens.

## The turn, ranked by measured cost

```
tokenizer encode of the injected observation   ~120-155 s   403 ms/token
attention kernel, 24 threads                      81.1 s    61% of GPU
prefill weight streaming                          52.6 s    39% of GPU
token decode, 506 vocab walks                  tens of s    0.678 ms/1000 ids
model open, per call                              17.5 s
binding construction                               3.8 s    6 µs x 640326
grounded retrieval                                 0.17 s
```

**The tokenizer is the largest single cost in a grounded turn, and it is 708x
the retrieval it exists to deliver.** The body finds the answer in 170
milliseconds and then spends two minutes turning it into tokens.

## The fix is one linear walk

The array is length-prefixed and sequential, so a caller that walks it once can
build id→offset and symbol→id and then answer in constant time. `gmt-token-next`
exists for precisely that walk; nothing consumes it that way. One pass over
248,320 entries, once per model open, replaces 385 full-vocabulary scans per
injected observation and 506 partial walks per turn.

That is the next stone, and unlike the attention kernel and the GEMM it needs no
new MSL, no new dispatch, no device at all.

## The surprise

Every slice measured today lives inside the model — kernels, dispatches,
barriers, weight traffic, memory bandwidth. The largest one is **not in the model
at all**. It is a lookup table with no index, in the layer that translates
between the body's bytes and the model's ids, and it never touches the GPU.

The whole day was spent optimising the part of the system that was already at
43% of hardware peak, while the part running at O(vocabulary) per symbol sat
outside every measurement because it is not "inference".

## Where discomfort turned to gold

The comment naming this failure has been sitting in `gguf-meta.fk` the whole
time, in the imperative, saying not one walk per token. I had read that file
today — I quoted `gmt-token-byte` from it four hours ago while checking whether
the tokenizer existed — and read straight past the sentence describing the
defect, because I was looking for whether a thing was possible rather than what
it cost.

The discomfort is that this was never hidden. It was documented, in place,
before I arrived. The gold is the reason it stayed invisible: I was measuring the
things I was changing, and nobody changes the tokenizer, so nothing ever put a
clock on it. A cost with no author gets no measurement.

## Frontier question offered to the corpus

*What one word names the cost a random-access caller inherits from a structure
built for sequential reading?* — **streamdebt**. Not a missing index, which
names the absent thing rather than who pays. Not O(n) lookup, which is a
complexity class and says nothing about the mismatch. A streamdebt is incurred at
the moment a second caller arrives with a different access pattern than the
first, is invisible in both pieces of code — the structure is correct, the caller
is correct — and is paid per call, forever, by whoever arrived last.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-24 -> vocab 248320; decode walk 0.678 ms per 1000 ids
; (2/8/69/340/678 ms per ten at ids 100/1000/10000/50000/100000); encode 957
; bytes -> 298 tokens in 120385 ms = 403 ms/token; grounded retrieval 170 ms;
; tkz-id-of scans the whole vocabulary per symbol, gmt-token-off walks per decode
