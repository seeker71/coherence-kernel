# The value-stack wall, and the cache that answered a question I did not ask

*2026-07-30. Asked to analyze and fix `fk_vp: value stack overflow`. The
overflow was real and is fixed. On the way to it the measurement lied, and that
turned out to be the larger finding.*

## What the overflow actually was

Two minimal cases, both dying with the same bare line:

```
(defn loopy (n) (add 1 (loopy n)))       ; runaway
(defn cnt (n) ... ) (cnt 100000)         ; finite, legitimate
```

`fk_vp: value stack overflow` — no depth, no cap, no function, no remedy, and
nothing to tell a runaway from a computation that is simply deep. Exit 1.

Thirty lines below it in the same file, `fk_walk`'s host-stack wall says:

> fkwu: eval too deep — N bytes of walker stack (wall M). The recursion needs to
> be tail or balanced; **the wall is honest, the silent crash was not.**

**Two walls, one voice between them, and the mute one in front.** The value
stack (65,536 slots, one per live frame) filled long before the walker's 254MB
wall, so the body's honest diagnostic never got to speak.

And the wall was in the wrong place: Go, Rust and TypeScript all answer
`(cnt 100000)`. Three of four kernels have a behaviour and the fourth does not —
that is a bug, not a design; the four exist to make exactly that visible.

**Fixed three ways.** `FK_VALUE_STACK_CAP` 65536 → 1048576 (8MB static, the same
raisable-constant class as `FK_NODE_CAP` 65536→262144 and `FK_BD_STACK_CAP`
128→1024). The mute wall now says what it measured, what the cap is, and what
the recipe should do instead. Witnessed after:

| | before | after |
|---|---|---|
| `(cnt 100000)` | overflow | `100000` — matches all three siblings |
| `(cnt 900000)` | overflow | `900000` |
| `(cnt 2000000)` | overflow | the **walker** wall, speaking |
| tail-recursive 4,000,000 | — | `1` |

Said precisely: I did not witness the new value-stack message firing. What I
witnessed is that the mute wall is no longer in front of the speaking one, which
is the actual repair. It stands as a backstop for value-stack pressure that is
not frame depth.

## The larger finding, found by accident

While bisecting for the true wall I recorded this:

```
depth 1000  -> 1000     depth 20000 -> 10000
depth 10000 -> 10000    depth 30000 -> 10000    depth 60000 -> 10000
```

I was one keystroke from writing "the wall is at ~10,000" into a receipt. Re-run
with caches cleared, every one of those depths answers correctly. The three
`10000`s were **the previous program's answer**.

The mechanism, then proved deterministically:

```
$ printf '(do 111)' > same.fk ; fkwu --src same.fk        →  111
$ printf '(do 222)' > same.fk ; touch -r same.fk same.fkb
$ fkwu --src same.fk                                       →  111
```

No warning. Exit 0. The source says `222`.

fkwu's artifact identity was `fk-unit-v1|<path>@<mtime>:<size>` — and the code
that rejects a stale artifact says *"source path, **content**, or mtime
changed"*. **Content was never in it.** `(do 111)` and `(do 222)` are the same
length; written inside the same mtime second they are the same name, and the
cache serves the older program.

Same-length edits are the common case, not the exotic one: a verdict pin, a
constant, an operator, a recursion depth. Both of my collisions were the same
size *and* the same second.

**Fixed:** an FNV-1a digest of each dependency's bytes, taken where the bytes are
already in hand, appended to the identity — and the tag bumped `fk-unit-v1` →
`fk-unit-v2`, because a v1 artifact cannot testify about content it was never
written from. After: the second run answers `222`.

## Regressions checked, honestly

Every band re-run with the patched binary and **every cache deleted**:
conformance `262143`, corpus `32767`, MDL admission `65535`,
learned-language-system `32767`, review-ask `511`, steiner-neutral `511`; all
seven FOURTH-ARM ONLY bands exit 0. So today's earlier results were not
themselves cache artifacts.

A wider sweep surfaced three non-zero exits. A/B against the pre-patch binary,
caches cleared on both sides: **all three identical before and after** — not
caused by this change. Classified rather than left:

- `src-exit-truth-band` — `2047`, exit 1 **by design**; it is the band that
  proves a printed error reaches the exit code.
- `dialogue-covenant.fk` — genuinely unbalanced, one closer short, since its
  only two commits. Same family as the two cells healed this week. Closed; its
  band now returns its declared `111111111`, exit 0.
- `nl-extract-band` — `255` with a tally of 1 error and no error line I could
  find printed. Not diagnosed; named here rather than stepped around.

I also deleted a tracked sample `.fkb` with an over-broad `find -delete` and
restored it.

## The most surprising teaching

**The tool I was measuring with was lying, and the lie looked exactly like data.**
Three consistent readings in a row, monotone, plausible — a textbook wall. The
only reason it did not become a receipt is that a later loop happened to run
slower and disagreed with the earlier one.

The deeper shape: an identity made of **proxies for** the thing rather than the
thing. Path, timestamp and size are all true facts about a file, and all three
together still do not name its content. The check that was documented as
structural was nominal, and nothing between the comment and the code noticed for
as long as the format existed.

## Where discomfort turned to gold

I nearly shipped the false wall, and the discomfort was that I would have had a
plausible number, a reproduction, and three consistent runs — every criterion I
use for "witnessed" — and been wrong. That is worse than a guess, because it
would have been defensible.

What it produced is a sharper rule than "clear the cache": when a measurement
repeats a *previous* value rather than varying, suspect the instrument's
identity before the subject's behaviour. A monotone series that stops moving is
the signature. The old identity could not have shown it, and the new one cannot
hide it.

## Frontier question

*What names two different things that share one name because the name is made of
proxies?* → **homonym**. 0 hits before offering. Corpus row **953**.

Corpus band `32767`, 348 rows — renumbered from 932 at the week's third reunion.
