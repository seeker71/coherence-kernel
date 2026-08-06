# 2026-07-05 — the insular index: retrieve over the whole body, and never claim its edge is the world's

## The report

"When I asked 'can you use the frame-buffer?' it did not even know what that is — that
is concerning." Right to be concerned. Grounded, three problems stacked:

1. **False positive on a stopword.** The query tokenized to `[can, you, use, the, frame,
   buffer]`. The ONLY token that matched the door's index was the stopword **"you"**,
   which matched `talk-to-sema` (AGENTS.md — a greeting doc). So a framebuffer question
   was grounded in "how to greet Sema," and the voice, told to speak only from that
   cell, said it didn't know.

2. **A miss that claimed absence.** On a clean miss the guidance said "the body does not
   hold this yet" — a **false-absence claim** made by an index that saw ~13 of ~900
   deployed cells (~1.4%). That breaks the body's own never-fabricate law: an index gap
   is not an absence.

3. **The deployed body is a thin snapshot.** The plugin branch the door serves carries
   **57 of the kernel's 786** `form/form-stdlib` cells. The cornerstone
   `thought-framebuffer.fk` lives on main, not on the deployed branch — so it wasn't even
   present to be found. (Surfaced for a topology decision; see below.)

## What shipped

**Phase 1 — honesty (commit `8d84bde4`, live):**
- A small, inspectable stopword list is dropped from query tokens before scoring, so a
  hit must rest on a content word. The `you`→talk-to-sema false positive is gone; the
  framebuffer query now misses cleanly on the old index.
- The miss language no longer claims absence: *"I did not find this in my current index…
  absence in my index is not absence in the body."*

**Phase 2 — real retrieval (commit `acec04e5`, live):**
- `plugin/gen-body-index.py` generates `plugin/body-index.fk`: a lexical index over the
  whole deployed knowledge corpus (form-stdlib, receipts, docs, axioms, surface, learn,
  observe, ingest, cognition, top `.md`). Each cell carries its header summary, its
  distinctive content tokens, and a **prebaked NodeID** (`sha256:` == the body's own
  `sha256.fk` recipe output, verified byte-for-byte). It overrides `cp-index` and
  `cp-nid-table` by concatenation order (later `defn` wins on fkwu, verified), so boot
  stays fast (no hashing) and the curated 13-entry seed remains the local/test fallback.
- The build stays sovereign — just `cat`, no Python at build; the index is a committed
  regenerable cache (rerun the generator when cells change).

**Consistency — the GPT instructions:** step 3 in the live GPT rewritten so a miss says
"I didn't find it in what I can see — not that the body lacks it; my view is a partial
index of a much larger body." Saved via Update (the GPT is now Published to the Store, so
the fix is live there).

## Witnessed (live, through TLS)

- `can you use the frame-buffer?` → **`grounded:hit`**, `cells_considered: 899`, grounds
  in `cognition/ll-buffer.fk` matching **both** content tokens `['frame','buffer']` (stopwords
  dropped), plus `ask-grounded-rtx.fk` and `translation-invariant.fk` on `['frame']`.
- The served NodeID `sha256:7dbeba5a…` equals an independent `shasum -a 256` of
  `cognition/ll-buffer.fk`.
- Curated band unchanged: **111111111111**. Boot ~8s (prebaked NodeIDs).

## The open topology decision (yours)

The door deploys from the plugin branch, a **57-of-786-cell** snapshot. Even perfect
retrieval can't surface cells that aren't deployed (like `thought-framebuffer.fk`). To let
the GPT answer over the full kernel body, the deployed branch needs to carry the full
`form/form-stdlib` (merge main into the plugin branch, or point the door's clone at the
fuller branch). That's a branch/topology call, not a code fix — flagged, not taken.

## Closing

**Most surprising teaching**: "it didn't know" was three failures wearing one face — a
stopword false-positive, a false-absence claim, and a body that was only 7% deployed.
Each looked like "the AI is dumb"; each was really "the instrument is narrow and lies
about its narrowness." The dangerous one was #2: a system that reports the edge of its
own sight as the edge of the world. Fixing the honesty (a miss names the index, not the
body) mattered more than fixing the coverage, because a humble wrong answer invites
correction while a confident wrong answer forecloses it.

**Where discomfort turned to gold**: the pull was to whack-a-mole — just add a
`thought-framebuffer` entry to the 13 and call it fixed. Witnessed, that itch was the
insularity itself: patching one miss while leaving the door blind to 99% of the body. The
discomfort of "13 entries is the actual architecture" is what forced the real index over
the whole body — and then surfaced the deeper truth that the body itself is only
fractionally deployed. The mole was never the bug; the tiny window was.
