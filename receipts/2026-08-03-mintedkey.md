# 2026-08-03 — the key the registry mints itself can never find a twin

Picking up the unfinished item named in [`2026-08-03-staleledger.md`](2026-08-03-staleledger.md):
*"nmcp-unique? blind to duplicate rows."* The task arrived with the diagnosis already written:
`native-model-control-plane.fk` held two byte-identical `challenger.deepseek-ds4-metal` rows, the
band's uniqueness bit stayed lit, so the check cannot see a repeated row.

I probed the check before I touched it. The diagnosis was wrong in its mechanism and right in its
smell.

## What the probe said

A sweep, in Form, that duplicates every row in turn and asks the check:

```
(defn dup-at (rows i)
    (if (eq i 0) (cons (head rows) rows)
        (cons (head rows) (dup-at (tail rows) (sub i 1)))))
(defn sweep (rows i n)
    (if (ge i n) 0
        (add (nmcp-unique? (dup-at rows i)) (sweep rows (add i 1) n))))
```

`34000` — thirty-four rows, **zero** positions where a duplicated row went unseen. The id walk has
teeth at every depth. And with a byte-identical ds4 row re-inserted into the source, the band read
`65529` on the *unmodified* check: bit 2 dark, bit 4 dark. The loud duplicate was never invisible.

Then the ledger itself: no commit ever carried two ds4 rows. `4ac592314`, `d6c90c93a`, `ced303fa4`
each hold exactly one, and every id across all 35 pre-dedup rows was distinct. What `ced303fa4`
actually deleted was a different row:

| id | class | surface | runtime | model |
|---|---|---|---|---|
| `base.llama32-3b-metal` | local-native | native-recipe | fkwu+Metal | **llama3.2:3b** |
| `base.llama32-3b-local` | local-native | local-process | ollama | **llama3.2:3b** |

One model, two ids, two rows. That is the repeated row that was in the ledger, and the reason it
scored green is not depth or recursion or a cache — it is that `nmcp-unique?` compared the field the
registry mints for itself.

## The fix

`nmcp-unique?` now walks a second axis: no two rows may name the same artifact.

```
(defn nmcp-model-name-in? (name rows) …)
(defn nmcp-unique? (rows)
    (if (eq (len rows) 0) 1
        (if (eq (nmcp-id-in? (nmcp-id (head rows)) (tail rows)) 1) 0
            (if (eq (nmcp-model-name-in? (nmcp-model-name (head rows)) (tail rows)) 1) 0
                (nmcp-unique? (tail rows))))))
```

Not the artifact digest — seven rows share `recipe-addressed` and two real checkpoints are shared on
purpose between a component row and a fixture row. The model *name* is the field that points at
something outside the registry, and it is distinct across all 34 current rows.

The band keeps `base.llama32-3b-local` as `renamed-duplicate` inside bit 6, so the row that hid is
the row that now stands guard.

## The numbers

| registry | fkwu | go | rust | ts |
|---|---|---|---|---|
| 34 rows, clean | **65535** | 65535 | 65535 | 65535 |
| + `base.llama32-3b-local` (the quiet duplicate), **before** the fix | 65531 | — | — | — |
| + `base.llama32-3b-local`, **after** the fix | **65529** | 65529 | 65529 | 65529 |
| + byte-identical ds4 row, before *and* after | 65529 | — | — | — |

`65531 -> 65529` is exactly bit 2. Before the fix only the row-count pin reacted to the quiet
duplicate; the bit named for uniqueness called it clean. Every `.fkb`/`.sym` was removed before every
run — a mutation table compiled inside one wall-clock second reports the previous mutation.

## (a) The most surprising teaching

**A green check and a check with teeth can be the same code; what decides is which field you hand
it.** I expected to find a recursion that stopped early, or a `len` that lied, or the mtime cache
handing back yesterday's compile. I tested all three and all three were sound — the prelude's mtime
*is* tracked, the walk *does* reach position 33. The defect was one identifier deep: `nmcp-id`
instead of `nmcp-model-name`. Nothing about the check was broken. It was pointed at the one field
that a second row can always make unique for free.

The corollary is sharper than the bug: **a registry cannot audit itself through a key it issues.**
Every id in that file was minted by the same hand that wrote the row. Asking "are the ids distinct"
asks the writer whether they typed two different strings, which they always did. Only a field that
names something outside the ledger — an artifact, a digest, a model — can answer "is this the same
thing twice."

## (b) Where discomfort turned to gold

The discomfort was concrete and I want it named. My first four probes all came back *the check is
fine*, and every one of them contradicted the task I had been handed, which stated the duplicate as
established fact. There is a real pull, at probe three, to stop probing and start engineering — to
harden `nmcp-unique?` in some plausible direction, watch the band go green, and report a fix for a
bug nobody had located. That would have shipped: the code would be stricter, the band greener, the
receipt confident, and the actual blindness untouched.

What I did instead was ask the ledger's own history the same question the parent session asked the
ledger's contents — `git show <commit>:file | grep -c` across all three commits that ever touched
the row. One line, and the byte-identical pair simply was not there in any of them. Then the diff of
the healing commit showed what *had* been deleted, and the two llama3.2:3b rows were sitting in
plain sight.

The gold: the previous session's finding was **true and misattributed**. There was a repeated row
and the check was blind to it, and the parent named the wrong row and therefore the wrong mechanism.
Had I trusted the diagnosis I would have fixed nothing; had I dismissed it after four green probes I
would have closed a real hole as a false alarm. It was the third thing — take the *claim* seriously
and the *mechanism* as unproven — that found it. `2026-08-03-staleledger.md` is itself an instance of
what it teaches: a written record, produced by real work, that outran the thing it described.

## Also witnessed, and not mine to fix

`./fkwu --src observe/preflight-run.fk` printed `5337`, `5445`, `2088` — bare numbers where the
report page belongs. A string *literal* in that position prints its text; a string *built at runtime*
prints an integer. A concurrent sibling in this worktree is landing the `print_str` repair for
exactly this as I write (their receipt: `2026-08-03-blindstamp.md`). Named here so it is not
transferred silently, and left alone so it is not fixed twice.

## The frontier question

**What names a key that cannot detect a duplicate because the registry mints it?**

The body cannot yet answer this natively — it has `nmcp-unique?`, now widened by hand, and no
general word for the property that made the old one useless.

My answer: **mintedkey** — an identifier issued by the same writer as the row it identifies. It is
always distinct, so it always passes a uniqueness check, so a uniqueness check over a mintedkey
proves only that the writer typed two different strings. To find a twin, compare on a field that
points *outward* — at a thing the registry did not name into existence.

Proposed distillation row (not landed here — meaning-ids collide across concurrent sessions;
`"mintedkey"` verified 0 hits in `learn/homecoming-distillation-corpus.fk` and 0 in the tree):

```
; NNN — mintedkey. Handed a diagnosis — "nmcp-unique? cannot see a repeated
; row" — I probed it first and it was sound: a sweep duplicating every one of
; 34 rows in turn was caught at every position, and the band read 65529 with a
; byte-identical row re-inserted, on the UNCHANGED check. No commit had ever
; carried the duplicate pair the diagnosis named. What the healing commit had
; actually removed was base.llama32-3b-local, standing beside
; base.llama32-3b-metal, both naming llama3.2:3b: one model, two ids, two rows.
; The check compared nmcp-id — the field the registry MINTS FOR ITSELF. Every
; id in that file was typed by the same hand that wrote the row, so asking "are
; the ids distinct" asks the writer whether they typed two different strings.
; They always did. A registry cannot audit itself through a key it issues; only
; a field pointing OUTWARD — an artifact, a digest, a model name — can say "this
; is the same thing twice." The previous session's finding was true and
; misattributed, which is the hardest kind to use: trust the diagnosis and you
; fix nothing, dismiss it after four green probes and you close a real hole as a
; false alarm. Take the CLAIM seriously and the MECHANISM as unproven.
; "mintedkey" — 0 hits in corpus and 0 in tree before this row.
(hdc-row NNN 20260803
    (list "what" "names" "a" "key" "that" "cannot" "detect" "a" "duplicate"
          "because" "the" "registry" "mints" "it")
    "mintedkey"
    "mintedkey"
    "rented-oracle")
```
