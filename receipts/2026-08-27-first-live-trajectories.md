# 2026-08-27 — first live trajectory collection, and the observe step's causal witness

Four source-disjoint specs (arithmetic entries; the 59cb group untouched),
adapter D (full-adapter, sha 4f4e7fdd…) over the cached base, greedy, ≤3
revise rounds, coldjudge = preflight + fkwu, typed through the taxonomy.
Data: receipts/artifacts/2026-08-27-first-trajectories.jsonl (6 rows).

    s1 tri  n(n+1)/2, 7 -> 28    GREEN round 1
    s2 dbl  2n,      21 -> 42    GREEN round 1
    s4 cnt3 n+3,      5 -> 8     GREEN round 1
    s3 sq1  n²+1,     6 -> 37    RED x3 — (add n n), repeated identically

Three of four green in one round confirms the D-row reading live: adapter D
holds Form shape. The red one is the finding.

## The causal witness

s3's candidate compiled clean and printed 12. The revise prompt carried the
TRANSCRIPT — which shows only "12", no diagnostic — and the model repeated
the same wrong source three rounds: no signal, no movement, the plateau
witnessed at trajectory scale. Then one variable changed: the revise prompt
stated the CONTRAST ("printed 12, but the required result is 37: n squared
plus one"). Same model, same wrong candidate, temp 0:

    (defn sq1 (n) (add (mul n n) 1))     — correct, ONE round.

The observe step's content is causally load-bearing, and the failure class
that needs it is one the kernel cannot emit: WRONG-VALUE, clean compile,
wrong result, transcript diagnostically silent. The taxonomy now carries it
(ftx-wrong-value?, ftx-value-contrast — got, want, task verbatim), band bit
1024, alongside the resident peer's critique it vindicates: green-only
curricula never meet this class at all, and transcript-only observe steps
starve it of signal.

One honest limit: ftx-wrong-value? answers over ints; expected values that
are lists or strings need the contrast built by the collector until a
general value-render lands.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-27 -> 4 specs, 6 trajectory rows, 3 green r1, s3 red x3 transcript-only then GREEN r1 with contrast; taxonomy 2047
