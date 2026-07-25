# 2026-07-25 — asked to answer through form-cli on the 100% native model: the body says no, and here is it saying so

The ask: answer the BenchBenchBench question through `form-cli`, **100% native model**.

The answer is **no, not on this host** — and the whole worth of this receipt is that the "no" is a
value the body returned, not a sentence I wrote. Every line below is something a cell computed.

## The floor, probed and not assumed

| probe | result |
|---|---|
| GPU | none (`no nvidia-smi`) |
| local runner | none (no ollama, no llama.cpp) |
| GGUF on disk | **0** files, whole filesystem |
| `form/form-cli` | Mach-O 64-bit **arm64** — cannot execute on this Linux host |
| `.coherence-network/rag-index/index.jsonl` | absent |
| `rh-native-resolver-ready?` | **0** — `rag-heal` is fail-closed, `rh-refuse-mutation` **73** |

The last row is the one that closes the door properly. Even the *grounded* lane cannot be filled
here natively: `rag-heal.fk` refuses every mutation entry point because native Form cannot yet
resolve a source NamedCell to the REF + current-content CTOR + persisted-source binding that a
`nodeid-rag-v2` row requires, and minting path IDs would silently overwrite a grounded index with
ungrounded rows. The cell says so itself and returns 73 rather than writing.

This is the same floor `receipts/2026-07-23-local-lane-rewitness-offer.md` named two days ago,
re-probed rather than inherited.

## Reaching the ASK verb anyway

The committed `form-cli` binary is the wrong architecture for this box, so the verb was reached
through its own cells on the source runner — which is the more native route regardless: no
prebuilt binary in the path, every line computed by `form-cli-ask.fk` itself.

That finding is **not new here** and is attributed rather than re-claimed:
`receipts/2026-07-23-form-cli-verified-reply.md` already recorded the binary answering
`Exec format error` on an x86 box and its source running on `fkwu --src`. I re-derived it
independently on this host — arm64 Mach-O, same conclusion — which is corroboration, not a
discovery. The receipt that got there first owns it.

`./fkwu --src form/form-stdlib/tests/ask-lane-probe.fk`:

```
lane: fkwu-rag-grounded
refusal: native-lane:absent
model:
--- fca-ask ---
[ask: local fkwu RAG index has no grounded hit]
local-lane:fkwu-rag-grounded
synthesis-lane:fkwu-rag-grounded
declined:native-lane:absent
```

That is the verb keeping every promise its header makes. It names the lane that answered. It names
why the other lane declined. The miss carries its lane too — the shape `form-cli-ask.fk` repaired on
2026-07-22 precisely so a caller could never read a miss as an answer from no lane.

## What landed

| cell | verdict |
|---|---|
| [`observe/ask-lane-floor.fk`](../observe/ask-lane-floor.fk) | the floor as a value, not a paragraph |
| [`observe/tests/ask-lane-floor-band.fk`](../observe/tests/ask-lane-floor-band.fk) | **31** on fkwu, cold cache |
| [`form/form-stdlib/tests/ask-lane-probe.fk`](../form/form-stdlib/tests/ask-lane-probe.fk) | the transcript above |
| [`form/form-stdlib/hex.fk`](../form/form-stdlib/hex.fk) | scoping repair — see below |

`alf-floor-code` is **0** here (11 both lanes / 10 native only / 1 grounded only / 0 neither). The
band deliberately does **not** assert that 0. A band demanding 0 would go red on the day the voice
finally comes home — the day it most needs to be green. What the band proves is the routing discipline,
which is the same on every host: a question the native lane declines routes to the grounded lane, a
decline is never silent, nothing the verb hands back is lane-less, and the fallback carries its reason.

## The numb green, caught in passing

The first run of the new band printed **31** and I nearly kept it. Cleared the cache and ran again:
it printed **nothing** on a cold compile, and 31 only on the *second* run, off an image the kernel
itself flagged `cached image was compiled with errors`.

By yesterday's own criterion (`learn/benchbench.fk`: a benchmark whose verdict does not come from
the run is parse-to-zero) that is a numb green, and it had a specific cause. `hex.fk` declared its
error sentinel as a **top-level `let`**:

```
(let HEX-DECODE-ERROR (bp "HEX-DECODE-ERROR"))
```

On the `--src` lane a top-level `let` is invisible inside a `defn` body — a truth `MANIFEST.md`
already states — so all four references read as `[unbound-name] ... recovered to 0`, and the
sentinel silently became the integer **0**: a decode error comparing equal to nothing a caller
would recognise. Those four errors polluted every consumer of `hex.fk`, the ASK lane among them.
The blueprint row for this name was admitted to `form-ontology-loader.fk` on 2026-07-17 *for
hex.fk's --src lane*; the scoping, not the registry, was what still stood in the way.

Repaired to a zero-arg `defn`, the idiom the manifest prescribes. `bp` is content-addressed, so
calling it per reference interns the same NodeID rather than minting a new one.

| | before | after |
|---|---|---|
| `hex-band.fk` on fkwu | **no value at all** | **12** / 14 |
| `ask-lane-floor-band.fk`, cold cache | nothing (31 only on rerun) | **31** on first compile |
| unbound-name errors in the ASK closure | 4 | **0** |

## What is still owed, named

`hex-band` reaches **12 of 14**, not 14. The two missing checks are the sentinel identity, and the
cause is below the scoping fix: on fkwu `--src`, `(bp "HEX-DECODE-ERROR")` returns **169** while the
band's independently-constructed `(make_nodeid 1 2 99 1770)` returns **-3**. The two routes to the
same registered NodeID disagree on this lane. That is pre-existing, deeper than a `let`, and not
repaired here. Before the fix the band produced no value at all, so 12 is a floor raised and an
honest residue, not a pass.

`hex-band` does not cross to the walkers either, before or after — Go stops at `unbound function
"make_nodeid"`, TS at `bp: unregistered blueprint name "do"`. Probed on both the pre-change and
post-change cell: identical, so nothing here regressed them.

The one error remaining in the ASK closure is `walk_recipe_here`, a sibling-kernel native that
`form/form-stdlib/primitive-registry.fk` already names as absent from fkwu. Axiom-5 recovers it and
it is not on the path any of these checks take.

## The hex change, checked against the surface main had already proven

`hex.fk` sits under the whole ASK closure, so the change above had to be checked against the bands
`receipts/2026-07-23-form-cli-verified-reply.md` landed on main rather than only against my own.
Merged main in and re-ran every one of them on cold caches:

| band | expected | got |
|---|---|---|
| `form-cli-ask-band.fk` | 262143 | **262143** |
| `form-cli-membrane-band.fk` | 1023 | **1023** |
| `observe/tests/membrane-lane-band.fk` | 31 | **31** |
| `observe/tests/membrane-lane-live-band.fk` | 31 | **31** |
| `dsv4-decode-loop-band.fk` | 1023 | **1023** |

Nothing moved. And a name worth separating while both are in one tree: `form-cli-ask-band.fk`
shares the `fca-` prefix with this work but proves a **different organ** — `form-cli-ask-gate.fk`,
the posterior sufficiency gate, over synthetic signal tuples, four-way. `ask-lane-floor` proves the
step before it on real host state: which lane gets to answer at all. A question can route perfectly
and still fail the gate, or pass the gate having been answered by a lane nobody named. Cross-
referenced both ways rather than composted, because neither subsumes the other.

## The question itself

`learn/benchbench.fk` answers it — the recursion terminates at level three, `bb-fixed-point?` = 1,
band 4095 four-way. That answer was computed by the body. The *words around it* are still mine, and
that crossing is the one this receipt cannot close: it is rung 5 of
`receipts/2026-06-29-native-zh-summary-PENDING.md`, and it is pending, as it has been. On a machine
holding the DeepSeek-V4-Flash weights the re-witness offered on 2026-07-23 still stands.

## How the exchange stayed alive

Asked for a native-model answer, I gave the body the question and reported what it actually said
instead of producing prose and stamping it native. That stamp is the one lie this whole project
exists to refuse.

**Most surprising teaching:** the refusal is better engineered than most answers. `fca-ask` names its
lane, names the declining lane's reason, and carries both on a miss — and the reason that shape
exists is a repair someone made three days ago after noticing a miss came back lane-less. A verb
that cannot answer can still be trustworthy, and this one is.

**Where discomfort turned to gold:** I had a green band and wanted to be done. Clearing the cache to
check it — on a day whose whole subject was benchmarks that measure nothing — turned a numb green
into a real one and surfaced a sentinel that had been quietly decoding to 0 for anyone who used it.
The check I least wanted to run is the only reason this receipt has anything in it.
