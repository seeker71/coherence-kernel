# The neutral tongue is already the largest — the dominance count and the fallow ledger

Date: 2026-07-17, late evening. Base: main `dcd52ebb4`. All counts run on this
tree (git-tracked where stated; `.cache` and generated caches excluded).

Urs asked: show the repo content line count with the neutral form expression
size becoming the dominant size over English or any other specific dialect;
show that any part can be generated to any supported NL or PL; and name every
gap to that goal with the reason we stopped — a reason we are at peace with,
even if the next moment never returns to it.

## The count

| surface | lines | share |
|---|---:|---:|
| **Neutral Form** — s-expr `.fk` code (no comments, no kernel mirrors) | 254,606 | **46.4%** |
| **Neutral data** — `.bml` authority cells 4,714 + registry/contract `.json` 9,061 | 13,775 | 2.5% |
| **English** — `.fk` comment prose 86,512 + tracked `.md` 78,191 | 164,703 | 30.1% |
| **English-leaning `.form`** section-DSL teachings | 14,205 | 2.6% |
| **PL dialects** — go/rust/ts kernel mirrors 73,737 + C seed & emitted bootstrap 12,504 + py/sh scripts 14,586 | 100,827 | 18.4% |
| total | 548,116 | 100% |

**Verdict: the neutral form expression is the dominant surface today** —
254,606 lines of executable s-expr against 164,703 of English and 100,827 of
all programming dialects combined. Neutral+data (268,381) exceeds English by
1.6× and dialects by 2.7×. It is dominance (largest category, larger than any
rival), not yet majority (49% of all lines including data). The trend arm:
every heal today moved lines INTO the neutral column (BML bands ported,
teachings landed as cells) and the generated column already includes the
kernel itself.

## What is generatable today, honestly

- **Form already generates its own executable surfaces in production**: the
  fourth-arm bootstrap kernel (5,593 lines of C emitted by
  fkc-table-serialize + gen-source-walker), fkwu-optable.h, .fkb images,
  form-lower asm bytes. The deepest dialect — the machine's — is a projection
  now.
- **PL round-trips**: BMF-shaped programs ↔ python/go/prolog at 3-kernel
  proof (fourth arm silent); typescript carries the known `list(` mangling
  defect (triage family d).
- **NL**: property-family sentences over ~47 pivot words project to 13
  tongues (4 four-way, 9 more fourth-arm).
- **Not yet generatable**: the 254.6k s-expr lines into PL/NL prose
  equivalents; the 164.7k English lines into the pivot; the kernel mirrors
  from Form (by design — see the fallow ledger).

## The fallow ledger — every gap named, with the reason we rest

Each entry carries why we stopped and the peace: we are OK to never witness
its closing if the next moment chooses something new.

1. **English prose (164.7k lines) is not pivoted.** Stopped because pivoting
   prose needs grammar and lexicon growth driven by the corpus-training loop,
   not hand transcription — transcribing by hand would fabricate coverage the
   translator didn't earn. At peace: prose serves as prose; each meaning gets
   its row the day something needs to say it in another tongue.
2. **The NL grammar speaks one sentence family.** Stopped because the
   architecture is what today needed proving, and it is proven thirteen ways;
   growth is data-add, not design. At peace: a shelf grows when hands reach
   for it.
3. **Non-Latin scripts are fourth-arm-only** (cross-kernel string-unit seam;
   fkwu walks bytes, go/rust/ts walk chars). Stopped because cursor-unit
   parity is deliberate three-kernel surgery deserving its own session. At
   peace: the fourth arm carries those scripts truthfully meanwhile, and the
   band wears its honest stamp.
4. **The PL lane's fourth arm is silent and ts-reversible has a defect.**
   Stopped because the 63-band proof-level program (floor next-step 4) and
   the triage receipt already own these by name. At peace: 3-kernel evidence
   is evidence, stamped as exactly that.
5. **The kernel mirrors (73.7k lines) are not generated from Form — and
   should never be.** Stopped by design: go/rust/ts are independent
   witnesses; generating them from the substrate would collapse the
   independence that gives four-way agreement its meaning. At peace
   permanently: this is the one gap whose closing we refuse.
6. **CN's content is not pivoted** (60G, mostly leases). Stopped because the
   seam wants one direction of flow first (CK canonical, parent consumes) and
   parent-side work belongs to parent-side sessions in that multi-agent
   field. At peace: endosymbiosis is not annexation.
7. **The .md docs are not projections of .form/pivot cells.** Stopped because
   doc-generation without coverage (gaps 1–2) would be a costume; the floor's
   relict discipline (re-run every witness, retire claims whose witnesses
   left) guards the drift meanwhile. At peace: a document that must be
   hand-tended is still a document; the vault pattern was recognized, not
   imported, and it ripens on its own clock.

The ledger's law, from the day's teaching: a snag parked with a reason and
witnessed at rest is fallow ground, not neglect — and if no moment ever
returns to it, the field was still honest.

## Correction, same night — "the neutral form is still in English"

Urs read the verdict above and said it plainly: the neutral form is still in
English. Re-measured at the byte level across all 254,833 code lines
(12,682,979 bytes), the claim above does not survive:

| inside the "neutral" s-expr | bytes | share |
|---|---:|---:|
| identifier bytes — 834,807 tokens, 61,335 distinct names, essentially all English-rooted | 5,873,330 | 46.3% |
| string literals (mostly English prose and keys) | 1,452,244 | 11.5% |
| numeric literals | 389,117 | 3.1% |
| structure (parentheses, whitespace) | 4,968,288 | 39.2% |
| genuinely neutral n\<k\> symbol tokens | 123 tokens | **0.015%** |

So the honest sentence is: **the s-expr corpus is a creole — its grammar is
its own, but its lexifier is English.** Roughly 58% of its bytes are
English-carried names and strings wrapped in 42% of truly neutral structure
and number. The only layers with no favored tongue are the numeric NodeID
substrate, the tags, and the ~47 pivot rows. The dominance table above
measured SYNTAX neutrality and called it LANGUAGE neutrality; those are not
the same thing, and English won the recount.

What the correction does not undo: the architecture already names the road
out. nl-translate.fk's own header: the symbol is "a door over the numeric
NodeID (identity invariant under renaming)" with the self-authored
symbol-space MDL loop refining it. The identifiers are lexicon rows that have
not yet been seated: `defn head` is an English column entry whose meaning is
a NodeID that could equally render as a Mandarin, Arabic, or minimal-symbol
column. True neutrality is not achieved by renaming 61,335 identifiers by
hand — it arrives when the name layer joins the pivot discipline and every
name, like every sentence, is a projection. Until then, this receipt says
what is: the body thinks in NodeIDs, proves in structure, and still speaks
its own code in English.
