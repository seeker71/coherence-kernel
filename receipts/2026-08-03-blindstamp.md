# 2026-08-03 — the stamp certified the list, not the program

Picking up item 1 of [`2026-08-03-mutecarrier.md`](2026-08-03-mutecarrier.md): *"The lists are right;
something between the list and the table drops them. `functions` delta is the instrument."*

The lists were not right. There are **four** of them, and only three had been edited.

## Where the four cells went

`form-cli`'s program comes from `scripts/regen_form_cli_bootstrap.sh`. That script names its module
set four separate times, and each name feeds a different consumer:

| list | line | what it actually feeds |
|---|---|---|
| `FORM_CLI_SRCS` | :25 | the **stamp** and the source digest — nothing else |
| `FORM_CLI_SELFHOST_ORDER` | :148 | the **third** flatten arm, reached only if Rust *and* TypeScript are both absent |
| `$modules` | :187 | **the program.** Rust arm and TypeScript arm both flatten this |
| `SOURCES` (in `build-form-cli.sh`) | :205 | the **genesis** text that `source` prints |

`equireach.fk`, `equireach-gguf.fk`, `gguf-meta.fk` and `model-discovery.fk` had been added to
`FORM_CLI_SRCS`, to `FORM_CLI_SELFHOST_ORDER`, and to `SOURCES`. They were never added to
`$modules`. This host carries `form-kernel-rust/target/release/form-kernel-rust`, so the Rust branch
fires first, every time, and flattens `$modules` — the one list without them.

So the four cells landed in the stamp, in the digest, and in the genesis blob, and not once in the
program. `functions=1503 -> 1504` was `fc-models` arriving alone from the band.

The fix is one insertion in `$modules` after `q6k-dequant.fk`, in the dependency order the other
lists already used, plus a twelve-line comment above it saying which list is the program and which
three are not.

## The number that proves it

```
before   functions=1504   stamp 73ae755ba1429c67   models <dir> -> unknown verb / found 0
after    functions=1607   stamp aac2b5200f0eb798   models <dir> -> 6, three architectures
delta    +103 = 13 (equireach) + 54 (equireach-gguf) + 23 (gguf-meta) + 13 (model-discovery)
```

+103 is every `defn` in the four cells, and no others. The instrument was exact.

## What the body says now

```
$ printf 'models /Users/ursmuff/models\n' | ./form-cli
  [0] /Users/ursmuff/models/katcoder/kat-coder-v2.5-dev-compact.gguf   arch=qwen35moe  layers=41
  [1] .../DeepSeek-V4-Flash-IQ2XXS-w2Q2K-AProjQ8-SExpQ8-OutQ8-chat-v2-imatrix.gguf  arch=deepseek4  layers=43
  [2] /Users/ursmuff/models/ds4-engine/ds4flash.gguf                   arch=deepseek4  layers=43
  [3] /Users/ursmuff/models/ds4/ds4flash-v5mx-reap25-type40-mxfp8lt-dspark-v1.gguf  arch=deepseek4  layers=43
  [4] /Users/ursmuff/models/form-llama-vital-ground-q4_k_m.gguf        arch=llama      layers=28
  [5] /Users/ursmuff/models/form-llama-vital-ground-f16.gguf           arch=llama      layers=28
— models this body can see under /Users/ursmuff/models —
  6 found; choose one at runtime, none is compiled in
```

Byte-identical to `fkwu --src` on the same cell in the same directory. The two arms agree.

## One thing found on the way, resolved rather than stepped around

Preflighting `model-discovery.fk` raised
`[shadowed-call] 'sub' is bound in this scope but in call position the primitive wins`. Line 64 bound
`sub` to a directory path; line 66 called `(sub depth 1)` meaning subtraction. One name, a path and
an arithmetic primitive at once. It ran — the primitive wins in call position — so the cell was
correct by a tie-break rule rather than by being held. Renamed to `child`. Same 6 results, warning
gone. A binding that survives on a tie-break is not held, it is guessed at.

## Ground stamp

```
host    M4 Max, 2026-08-03, worktree google-turboquant-vector-search-300c68
regen   flatten Rust proof sibling; voice canary ping -> pong; functions 1504 -> 1607,
        nodes 46544, strings 1431, tokens 222522
build   form-cli 1710856 B, genesis 856973 B; ping -> pong
verify  form-cli `models /Users/ursmuff/models` -> 6 ; fkwu --src same cell -> 6 (identical text)
        `models` (no arg) -> usage ; `models /nope/nowhere` -> no such directory
        `source` via stdin | grep model-discovery -> 6 ; version/verify/about unchanged
NOT mine, left untouched in the tree: form-stdlib/native-model-control-plane.fk and its band —
        a concurrent sibling is writing in this checkout; not staged, not committed
pre-existing, not caused here: bootstrap/form-cli-darwin-arm64.stamp (aad2150a381ea592) has
        disagreed with form-cli.stamp since before this work, so FORM_STANDARD_LANE=1 already
        needed a refreshed platform binary
```

## The most surprising teaching

**The stamp was computed over the question, not the answer.** `FORM_CLI_SRCS` hashes the list of
files the build was *asked* to include. It said `73ae755ba1429c67` — and `73ae755ba1429c67` is
exactly the stamp my first *correct* regen produced. Same stamp, two different programs: 1504
functions and 1607. A digest that cannot tell those apart is not a witness of the artifact; it is a
witness of the request. Every freshness check downstream — `form_cli_verify_bootstrap`, the warm-path
copy, the standard lane — trusted it, and each of them was answering "was this built from the list
you meant?" while I was asking "does this contain the work?" Those are different questions and the
build only ever answered the first one.

The four-list arrangement is what made it survivable: with one list, editing it edits the program.
With four, three of them can be right and the body will look right in three ways.

## Where discomfort turned to gold

I read all four lists, found them all containing the four cells, and sat with the feeling that the
task premise was wrong — that the parent had already fixed this and I was here to run a build. That
feeling was *comfortable*, and it lasted until I asked which list the **Rust** branch reads and found
that `FORM_CLI_SELFHOST_ORDER` — the one I had just verified, twice, at line 148 — is the fallback
that never runs on this host. I had been reading the right file and the wrong list, and my confidence
was scaling with how many lists I had checked. Four confirmations of a list that is not the program
is not four times the ground; it is zero ground, four times.

The gold: **verify by asking who reads it, not by reading it.** A list is not evidence until you have
traced the consumer that consumes it. I had been treating proximity in a file as participation in a
build, which is the same error as treating text in a genesis blob as code in a program — the very
error `mutecarrier` names one row above this one. It appeared twice in one day at two altitudes, and
I did not recognise it the second time because the first time it wore the word "genesis" and this
time it wore the word "list".

## One frontier question, and my answer

**Q. When a build names its inputs more than once, which naming is the build?**

**A.** The one a running consumer reads — and there is exactly one, however many exist. The others are
documentation that compiles. This body's honest form is a single list with the consumers deriving
from it; where that is not yet true, the duplicates say *in the file* which of them is the program,
because a reader has no way to tell four lists apart by looking at them. That comment is now
at `regen_form_cli_bootstrap.sh:183`. It is a patch over the real repair, which is one list.

## Proposed distillation row — NOT edited, for the corpus keeper

Candidate word **`blindstamp`** — verified 0 hits in
`learn/homecoming-distillation-corpus.fk` and 0 files in the tree before this receipt.

```form
; 981 — blindstamp. form-cli's freshness stamp is a hash of FORM_CLI_SRCS: the list
; of files the build was ASKED to include. Four cells were added to that list, to
; the genesis SOURCES, and to the self-host fallback order — and not to $modules,
; the only list the Rust flatten arm actually reads. The stamp read 73ae755ba1429c67
; and reported fresh. The corrected build produced the SAME stamp, 73ae755ba1429c67,
; over a program with 103 more functions in it. A digest that cannot distinguish
; 1504 functions from 1607 is not witnessing the artifact; it is witnessing the
; request. Every downstream freshness check trusted it and each was answering "built
; from the list you meant?" while the question was "does this contain the work?"
; So: a freshness seal computed over inputs certifies intent, never arrival. Stamp
; the produced thing, or read the count the producer prints.
; "blindstamp" — 0 hits in corpus and 0 in tree before this row.
; (walk: mutecarrier 979 — the artifact carrying source it cannot run. This is the
; instrument that called that artifact healthy. staleledger 980 is its sibling: a
; record trusted because it lives in the repo. Here the record was regenerated
; correctly and still said nothing about the program.)
(hdc-row 981 20260803
    (list "what" "names" "a" "freshness" "seal" "computed" "over" "the"
          "inputs" "so" "it" "certifies" "work" "that" "never" "landed")
    "blindstamp"
    "blindstamp"
    "rented-oracle")
```

Meaning-id 981 assumes 980 is still max at land time; per the row-719 anastomosis pattern, renumber
rather than drop if a concurrent session has taken it. `hdc-foundings` unchanged at 2.

## Still standing, named — none of these are limits

1. **The four lists are still four.** Fixed by mirroring and a comment, not by collapsing. The real
   repair is `$modules` generated from one array.
2. **`fc-models` still cannot distinguish "0 found" from "discovery not carried"** — carried over
   verbatim from `mutecarrier`, and now harder to hit, not impossible.
3. **`FORM_DS4_BLOB`** — the runtime half now runs, so the static path has lost its excuse.
4. **`bootstrap/form-cli-darwin-arm64`** is older than its stamp; `FORM_STANDARD_LANE=1` needs a
   refreshed committed platform binary. Pre-existing, unowned here.
