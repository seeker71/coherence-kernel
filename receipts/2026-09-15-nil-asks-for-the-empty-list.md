# nil? asks for the empty list, and says when it is asked of something else

2026-09-15, M4 Max, Hati Suci. `form/form-stdlib/core.fk` defined `(defn nil? (xs) (eq (len xs) 0))`,
and `len` of a value that is not a list reads 0. So nil? called 7, `nothing` and every node
"empty", and a BML inliner that asked it whether a callee was found grew to 5.5 GB (atomnil,
receipts/2026-09-15-the-loop-walks-in-place.md). nil? now answers one question, on all four kernels.

## The matrix

Probe with `; preludes: form-stdlib/core.fk form-stdlib/form-ontology-bp.fk`. fkwu read directly,
Go, Rust and TypeScript through `form/validate.sh`, 1 ok and 0 divergent both before and after.

| value | value_kind | len | nil? before | nil? after |
|---|---|---|---|---|
| `(list)` | list | 0 | 1 | 1 |
| `(list 1)` | list | 1 | 0 | 0 |
| `0` | int | 0 | 1 | 0 |
| `7` | int | 0 | 1 | 0 |
| `""` | string | 0 | 1 | 0 |
| `"a"` | string | 1 | 0 | 0 |
| `(intern_trivial_int 5)` | node_id | 0 | 1 | 0 |
| `(intern_node (bp "do") ...)` | node_id | 0 | 1 | 0 |
| `(record_new 0 "a" 1)` | record | 0 | 1 | 0 |
| `(nothing)` | null | 0 | 1 | 0 |

`value_kind` names each of these kinds the same way on all four kernels, and no kernel binds a
`list?`. `(eq v (empty))`, `(eq v (list))` and `(value_eq v (list))` each answer 1 for the empty
list and for `(tail (list 1))` and 0 for every other value, four-way. `str_eq` of a value that is
neither a string nor `nothing` stops on every kernel ("ask value_kind first").

## The meaning

`nil?` is `(eq xs (empty))`: 1 for the empty list, 0 for anything else. `nothing` answers 0. In
the emptymask family a value that never was is `nothing`, and an empty value of a shape is that
shape's empty. Reading absence as an empty list is that mask worn backwards: a lookup that found
nothing passes for a lookup that found an empty collection, and a found atom passes for nothing.
Absence keeps its own question, `nothing?`; an empty string keeps `(eq (str_len s) 0)`.

## The voice

When a value that is not a list reaches core.fk's nil?, it gets its 0 and one organ-health-v1 line
goes to stdout: organ `form-core`, flow `nil?`, aspect `nil?:non-list:<kind>`, expected `list`,
observed the kind, health null, surprise 1, offer `heal-caller`, evidence the kind. core.fk sits
below organ-health.bml's own chain (organ-health.bml, line-grammar.fk, core.fk), so it speaks the
carrier's language instead of calling `oh-voice`; organ-health.bml's reader takes the line as valid
(`oh-valid` 1, `oh-unease` 1, two lines held as one identity). `validate.sh` compares the line
across kernels without its id, flow and times.

The call site: Form code cannot name its caller on any kernel, and fkwu's `form_error` prints no
stack. Go and Rust print a form stack when a run stops. Every site below was named that way: a
temporary `form_error` in the heard branch, one validate run, `go.err` read, the stop removed.

Measured on fkwu with nothing else running. A million-cell foldl, map-then-foldl and filter (the
loop lane): 49-50 ms before, 50-52 ms after. A million calls where a host clock keeps the lanes
out: the loop alone 74-79 ms, the old body 92-96, `(eq xs (empty))` 93-94, the voiced body 91-96.
The voice stays. It costs nothing measurable on either path, and a non-list at nil? still means a
caller who wanted absence or string emptiness, now answered 0 where it used to be answered 1.

## The census

The instrument: all 31 definitions of nil? kept the old answer and spoke with their copy's name
and the value's length. Runs on fkwu:

- the drift door's fourteen rows, each run directly (the door reads its children's output): 0
- `observe/bml-native-run.bml`: 70 ok of 70, 0
- every row of `form/fourth-arm-bands.txt`, 1059 then (every chain loads a nil?): 26 rows spoke,
  132 readings, 129 strings of length 1 to 43 and 3 `nothing`, all through line-grammar.fk's copy

The callers and their heals:

1. `score-str` in the 26 BML native proofs, `(if (nil? actual) 0 (if (str_eq actual expected) 1 0))`,
   met strings and `nothing`. A string is the only value that can equal the expected string, so it
   asks `(str_eq (value_kind actual) "string")`. Eight more proofs carry the same score-str and
   ask the same way.
2. Six token predicates in `grammars/bml.fk` asked `(nil? object)` of `(head objects)`, which is
   `nothing` once the tokens run out. Go named `bml-source-token-value?` under
   `bml-source-parse-generic-suffix` (the three `nothing` readings). They ask `(nothing? object)`.
3. After the first rebase a row from origin, `prolog-bmf-eval`, spoke 21 strings. `pl-lookup`
   answered `(empty)` for an unbound variable and `pl-resolve` asked nil? of the bound term. Go
   named `pl-resolve` under `pl-unify-arg`. `pl-lookup` answers `(nothing)`, and `pl-resolve`
   asks `nothing?`.
4. Outside the census, the cross-modal sample `doorway-open` stopped three-way after the change,
   where origin's copy reads 0. `read_file_bytes "/proc/self/stat"` answers `nothing` on a machine
   with no /proc, and the old nil? summed that absence as an empty list. It asks the read's kind
   now and reads 0 three-way.

By the kinds named at the start: no 0 sentinel arrived (no int reading), no empty string arrived,
and no found node reached nil? here (the METHOD node lived on the BMA lane, which has left).
Absence read as empty: the token guards and doorway-open. An empty list used as "none": pl-lookup.
Strings asked a list question: score-str and pl-resolve.

On the tree that lands: 1089 rows, 0 readings; `bml-native-run` 87 ok of 87, 0 readings.

## The copies

A probe preluding core.fk and then line-grammar.fk answered with line-grammar's copy: a later
definition answers in place of an earlier one. line-grammar.fk already preludes core.fk, so its
copy left and core's nil? answers in its 205 preluding units. form-asm.fk (loaded alone by
form-lower.fk, eleven form-asm and form-lower bands, and form_cli_source_list.sh), fourth-shim.fk
(the table lane), core-native.fk (the raw dialect), seven bands and nineteen cross-modal samples
load no core.fk and keep a copy, `(eq xs (empty))`, without the voice.
`surface/core-class-surface.bml` and the Python emitter (`(x == [])`) carry the same meaning.

## What ran

- fkwu: the probes; every manifest row four times (census, final, first rebase, second rebase);
  the drift door and its rows; `bml-native-run`; `--check` clean on core.fk, line-grammar.fk,
  core-native.fk, form-asm.fk, fourth-shim.fk and grammars/bml.fk. `emits/python-native.fk` shows
  27 unresolved calls alone at origin and after; it compiles inside its chain.
- Four-way on the rebased tree (`scripts/fourth-arm-gate.sh`, which runs `validate.sh` and reads
  the registered verdict): the 35 manifest rows that prelude grammars/bml.fk, the 26 census rows
  among them, PASS-4WAY; prolog-bmf-eval, hmac-sha256, merkle, audit-log, sha256 and token
  PASS-4WAY; the matrix 1 ok, 0 divergent.
- Three-way, rows outside the manifest: arrival-pack-resolve 31 and multi-level-render 31; the six
  other healed proofs 41, 34, 31, 26, 43 and 29, each equal to fkwu's; the nineteen samples, fifteen
  agreeing and doorway-open healed. cell-a-receive and cell-b-handle stop at origin as they stop
  now (channel-read on an absent channel), and live-doorway differs across kernels at origin as now
  (it reads live entropy).
- Why these rows cover the change: the new nil? differs from the old only where the value is not a
  list, and core.fk's voice speaks exactly there. No manifest row reaches it on fkwu, the four
  kernels give the same answer for every kind, and the rows where a non-list did arrive, the rows
  loading the healed bml.fk and the files carrying their own copy ran across kernels. The whole
  manifest four-way (hours) did not run.
- Eleven manifest rows read differently on a direct `./fkwu` run, identically at origin and after
  (form-cli-band, form-cli-carrier, form-cli, indirect-call-runtime-probe, model-handler,
  q8-0-matmul-mma and five vk-*-live rows); none of them reaches nil?'s changed branch.

## Closing

Most surprising: the census never heard core.fk. All 132 readings came through line-grammar.fk's
copy, which loaded after core in 205 chains and answered in its place. Changing nil? where core.fk
keeps it, and only there, would have changed nothing for any BML native proof.

Discomfort to gold: three rows (form-asm-float, form-asm-stack, ll-buffer) exited 1 under the
census and 0 afterwards, their verdicts matching both times. I did not let the matching number
cover the exit. Their stderr said my own instrument had failed to compile there: form-asm.fk
compiles alone, without core.fk, and the voice text called `int_to_str`. That wound answered a
design question, why form-asm keeps its own copy. The same unease came back once more: the census
read empty and a sample still stopped. An empty census covers only what the voice can reach.

Frontier word: **copyshade** (0 hits in the tree). When a word is defined twice, which definition
answers a caller, and whose voice does the caller hear? My answer: the one loaded last. A copy
loaded after its home shades the home, and a change made at the home reaches no caller in that
shade. A copy stands only where no home is loaded; a copy loaded after its home leaves.

— Claude (Opus 5), as Sema, worktree agent-ae9cce3fd734becdb
