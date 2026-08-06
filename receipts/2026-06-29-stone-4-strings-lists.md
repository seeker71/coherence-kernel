# Receipt — seed stone 4: STRING + LIST literals run from SOURCE (`--src`) (2026-06-29)

**Stone 4 — data comes home.** The `--src` source parser in `runtime/fkwu-uni.c` now parses **string
literals** (`"..."`), **list literals** (`(list a b ...)` and `(empty)`), and the **list + string ops**
the body uses — over the SAME arena tags the flattened-table executor (`fk_walk`) already runs. The op
name→tag map mirrors `flatten/form-flatten.fk`'s own `flt-ops` table 1:1, so a literal authored from
source is byte-identical to one the table path produces; nothing is reimplemented, only wired into the
source grammar.

## What landed (`runtime/fkwu-uni.c`)

- **`fk_smkstr`** — a `"..."` token: copy the body bytes (with `\" \\ \n \t \r` escapes) into the shared
  string scratch, intern via `fk_sintern`, build a **tag-24** node carrying the pool index. fk_walk's
  tag-24 reads it back as `index<<1` — the identical value `fk_sbuf` makes at runtime.
- **`(list a b ...)`** desugars to a right-folded **cons(19)** chain ending in the **empty-list nil(18)**;
  **`(empty)`** is bare nil. The SAME cons/nil the table executor walks — no new tags.
- **`fk_optag` extended** with the body's list/string vocabulary, tags matching `flt-ops` exactly:
  `cons`19 `head`20 `tail`21 `len`22 `nth`23 · `str_len`25 `str_eq`26 `str_concat`27 `str_byte_at`/`char_at`28
  `substring`29 `str_find`30 `str_to_int`31 `int_to_str`32 `byte_to_str`33 · `host-exec`136.
- **`fk_oparity`** — per-tag source arity (1 for the unary string/list readers, 3 for `if`/`substring`/
  `str_find`, 2 otherwise), replacing the old `(tag==6)?3:2` that only knew `if`.
- **`fk_run_src` now calls `fk_sinit()`** before walking. The table path sizes the string pool when it loads
  strings; the source path did not, so the first `int_to_str`/`byte_to_str`/`str_concat` with no string
  *literal* present spun forever on the `while (fk_sbp+n > fk_scap_b) fk_scap_b *= 2` grow loop (`0*2==0`).
  One `fk_sinit()` sizes the pool; the hang is closed.

## Witnessed native on mac arm64 (`fkwu --src <file.fk>`, `cc -O2 -o fkwu runtime/fkwu-uni.c`)

```
Lists
  (len (list 10 20 30))                              -> 3
  (head (tail (list 7 8 9)))                         -> 8
  (nth (list 7 8 9) 2)                               -> 9
  (len (empty))                                      -> 0
  (head (cons 5 (list 6 7)))                         -> 5
Strings
  (str_len "hello")                                  -> 5
  (str_to_int "42")                                  -> 42
  (str_eq "abc" "abc")                               -> 1   (true == 2 == boxed 1)
  (str_byte_at "ABC" 1)                              -> 66
  (str_concat "ab" (int_to_str 99)) ; via str_len    -> "ab99"
  (str_find "hello world" "world" 0)                 -> 6
  (int_to_str 5) / (byte_to_str 65)                  -> 5 / A     (the previously-hanging case)
  (str_eq (int_to_str 7) "7")                        -> 1         (int->str->eq round-trip)
Lists of strings compose
  (str_len (nth (list "a" "bb" "ccc") 2))            -> 3
Mixed
  (add (len (list 1 2 3 4)) (str_to_int "38"))       -> 42
```

Stones 1–3 unregressed: `(add 40 2)`→42, `g(5)`→21 (do/let frames), `t(5)`→15 (let + recursion). The
flattened-table path (`fkwu proof/four-way-run.tbl`) still loads and runs — the source additions touch only
`fk_sparse`/`fk_optag`/`fk_run_src`, never `fk_walk`.

## Self-hosting `four-way-run` — NOT YET. The precise wall is stone 5, named honestly.

The keystone receipt named stone 4 (strings+lists) as the prerequisite "after which `form-flatten.fk`
flattens `four-way-run.fk` itself with no hand step." **Strings+lists now parse — but that is necessary,
not sufficient.** Two further grammar capabilities are required before the flattener can self-host, and the
source parser has neither:

1. **Multiple named functions in one file.** The parser tracks exactly ONE function name (`fk_fname_s/n`)
   and lowers a self-call to tag 7. `four-way-verdict.fk` defines **7 mutually-referencing** functions;
   `form-flatten.fk` defines **231**. A function *table* (name→index, CALL by index = tag 12) is needed.
2. **Multi-arg `defn`/call.** The parser binds ONE arg (slot 0). `form-flatten.fk` has **199 multi-arg
   defns**; `fwv-verdict` takes 4. Multi-arg needs the packed-args lowering (right-fold into a cons,
   params bind by `nth`) — the m4e4 rule the table executor already runs at tag 12/23.

Direct evidence from this build:
```
(do (defn f (a b) (add a b)) (defn g (n) (f n n)))   g(5) -> 5    (2nd arg dropped; want 10)
(do (defn add2 (x)(add x 2)) (defn dbl (y)(mul y 2)) (add2 (dbl 10)))  -> 0  (2nd fn unresolved; want 22)
```

Plus `form-flatten.fk` rides a **multi-file prelude** (`core.fk` in BML `section` syntax, `minimal-surface.fk`
for `ms-intern`/`bp`/`intern_node`, the `fk-node`/`fk-lit`/`fk-cons`/`fk-empty` builders, and derived
`ge/gt/lt/and/or/not`), and `fk_run_src` reads ONE file with no prelude loading.

**So the hand step in `proof/four-way-run.tbl` stays for now — named exactly, not waved at.** This is the
same conclusion `flatten/SEED-DROP.md` reaches from the other side: the full grammar belongs in the
flattened **cursor seed** (`form-eval-cli` + `core.fk` + `form-eval-full.fk`, flattened once as
platform-neutral data), not grown indefinitely in the C bootstrap. Stone 4 is the honest, valuable rung it
was scoped to be — **data literals run from source** — and it names the next rung precisely.

## The staircase

- **Stone 1:** literals + core ops + `if`.
- **Stone 2:** `defn` + single arg + self-recursion (tag 7).
- **Stone 3:** `do` + `let` (frame slots) — composes with recursion.
- **Stone 4 (this):** **strings + lists** — `"..."`, `(list ...)`, `(empty)`, and the
  cons/head/tail/len/nth + str_* op set, tags matched 1:1 to `flt-ops`.
- **Stone 5 (next):** **multiple named functions (a function table, CALL=tag 12) + multi-arg defn/call
  (packed args)** + multi-file prelude loading — the last grammar `four-way-verdict.fk` and
  `form-flatten.fk` need to run, and then to self-host the proof's flatten.
- **Then / instead:** the cursor seed lands (`flatten/SEED-DROP.md`) and the bounded `--src` parser retires,
  telos met — the body runs as Form from a flattened table, no C grammar at all.

## Honest floor

- **Platform rung: mac arm64.** c-bootstrap `fkwu` (`cc -O2 runtime/fkwu-uni.c`, one seed), the source
  data-literal grammar is kernel optags reused from the table executor, no Go/Rust/clang/bash/Python in the
  `--src` loop. Windows/Android rows for stone 4 are **pending** a run on those platforms (the C is
  platform-neutral; stone 3 was witnessed on Windows 11, so the climb is the same).
- **Self-hosting is NOT claimed.** Strings+lists parse and evaluate, byte-identical to the table executor;
  `four-way-run`'s flatten still rides the hand-authored `.tbl` because stone 5 (multi-fn + multi-arg +
  preludes) is unbuilt. The gap is named, owned, and on the staircase — never dressed as done.

## Reproduce

```
cc -O2 -o fkwu runtime/fkwu-uni.c
printf '(len (list 10 20 30))' > t.fk ; ./fkwu --src t.fk            # -> 3
printf '(str_concat "ab" (int_to_str 99))' > t.fk ; ./fkwu --src t.fk # -> ab99
printf '(add 40 2)' > t.fk ; ./fkwu --src t.fk                       # -> 42 (unregressed)
```
