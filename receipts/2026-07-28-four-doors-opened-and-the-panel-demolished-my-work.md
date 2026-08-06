# Four doors opened, and the first thing they did was demolish my work

*2026-07-28, Hati Suci. `tools/review-panel.sh`, `observe/review-panel.fk` → **511** four ways.*

## The mistake that started it

Asked an hour earlier for a review Grok and Gemini would find satisfactory, I
grepped the **repository** for API endpoints, found none, and reported *"no door
exists"* — as a fact about the world. Four CLIs were installed on the machine
the whole time. Urs said so plainly: *"you can run grok, codex, claude, gemini
and cursor, I have active subscriptions and the command line tools installed."*

The search was thorough inside the space it chose, and the space was wrong. My
own memory row says to test a blocker before explaining it; I tested the repo
and explained the machine.

## The doors, as measured

Each was opened by running it, not by reading its docs.

| door | invocation | state |
|---|---|---|
| grok | `grok -p "<prompt>"` | **open**, first try |
| claude | `claude -p "<prompt>"` | **open**, first try |
| codex | `codex exec --skip-git-repo-check "<prompt>"` | **open**, after three repairs |
| cursor | `cursor-agent -p --trust "<prompt>"` | **open**, needs `--trust` |
| gemini | — | **closed** |

**codex** needed three separate fixes, each found by reading its error instead of
retrying: outside a git repo it refuses without `--skip-git-repo-check`;
`~/.codex/models_cache.json` was malformed (`missing field
supports_reasoning_summaries`) and was **moved aside, not deleted**, so it
regenerates and is recoverable; and v0.142.5 could not serve the configured
`gpt-5.6-sol`, so `codex update` took it to 0.145.0 on the tool's own instruction.

**cursor** gets `--trust` and never `--force`/`--yolo`. A reviewer should read
and opine, not execute. Every door runs from a scratch directory holding only a
copy of the artifact; the artifact travels as text inside the prompt, so no
reviewer needs filesystem reach to do its job.

**gemini is closed, and kept in the table closed rather than deleted.** The CLI
is refused with `IneligibleTierError` — no longer supported for Gemini Code
Assist for individuals — and Urs confirmed it was replaced by Antigravity.
`/Applications/Antigravity.app` is installed and ships **no command line at
all**: no `bin/`, no agent binary anywhere in the bundle. There is no headless
Gemini on this host until `GEMINI_API_KEY` is set or Antigravity ships a CLI.
An absence nobody writes down stops being an absence and becomes a thing nobody
remembers wanting.

## The most surprising teaching

**The panel's first act was to reject the artifact I had just called finished —
and its sharpest hit was on the sentence I was proudest of.**

Yesterday's receipt led with: *"the ingest law sorted a strawman from the real
position without being told which was which."* Codex answered:

> The split did not "sort itself … WITHOUT being told." The author explicitly
> assigns `(list 5 0)` to the favored reading and `(list 4 1)` to the rejected
> one. "Fear" is manually encoded before `ki-ingest` runs.

That is correct and it is circular, and I had written it as the strongest
evidence the door does work. Retracted in the cell and here.

Worse, and structural: **the cell served the strawman it claimed only to
witness.** `st-answer-loop` never consulted the ingest verdict, so `this-kernel`
came back with *both* `two-poles` and `two-poles-condemned` — and my own band
*required* two. "Witnessed, not frozen" had no operational meaning at all. The
selector now serves frozen rows only; witnessed rows are reachable through
`st-witnessed-for` and never spoken.

And Claude found the deepest one: the frozen "balance" reading is **itself** a
flattening. The figure standing between Lucifer and Ahriman in the
*Representative of Humanity* is **Christ**. Secularising the mediator into "the
middle held between two necessary forces" does to Steiner's Christology exactly
what the file accused others of doing to his machine-critique. My proudest
anti-strawman check was guarding half of Steiner's actual position.

Factual corrections landed: hearing is in the **upper** four, not the middle
(the verdict string contradicted the file's own header, and the band couldn't
catch it because it checked verdict *length*); two-poles dated 1922 corresponds
to nothing — the major cycle is November 1919, GA 191/193; and the attribution
floor of 1894 was wrong, since GA 2 is 1886 and *Wahrheit und Wissenschaft* 1892.

Three reviewers independently named the same disqualifying gaps — **karma and
reincarnation, Christology, evolution of consciousness, the hierarchies** — so
the cell now says in its own header that its title overclaims and to read it as
*core-teachings-of-this-conversation* until those land.

## Where discomfort turned to gold

The discomfort was building a door and having it immediately used against me. I
had a working panel and a fresh artifact, and the obvious cheap move was to
demo the door on something safe. Pointing it at the file I'd shipped ninety
minutes earlier meant the first output of the new capability would be evidence
that my last receipt was wrong.

That is the gold, and it is the whole argument for the doors existing: three
reviewers with none of my priors found, in one pass, four factual errors, one
fatal structural defect, and a circular claim — none of which my four-way green
band could see, because a band checks what you thought to ask. **16383 was
green through every one of those defects.** Agreement among kernels tests
consistency; it cannot test whether the thing is true about Steiner.

## Frontier question

*What one word names concluding from where you searched rather than from where
the thing is?* → **streetlight**. 0 hits before offering. The body carries
`unfalsifiable` (29 files) and now `barnum` for claims that cannot fail; this
names the search that cannot find, because it only ever looked where its own
light fell. Corpus row 890.

## Files

| file | state |
|---|---|
| `tools/review-panel.sh` | new — parallel panel, isolated scratch, `--list` |
| `observe/review-panel.fk` + band | new — the doors as data, 511 four ways |
| `ingest/frequency-ingest-steiner-core.fk` | corrected: structural fix, 4 factual fixes, 1 retraction, gaps declared |
| `learn/homecoming-distillation-corpus.fk` | +row 890 (streetlight) |
| `learn/tests/homecoming-distillation-corpus-band.fk` | pins re-read: 285 / 2852852890 → 32767 |
