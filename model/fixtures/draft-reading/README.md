# Draft reading — the covenant a native reading holds a draft to

`observe/draft-native-reading-run.bml` reads the body's own drafts (the ones
`observe/prompt-draft-run.bml` writes for the person's prompts) in two
readings: first this covenant, then the voice. `covenant.jsonl` holds one row
per predicate:

| field | what it holds |
|---|---|
| `id` | the predicate's name in the tally (`predicates_failed` names it) |
| `predicate` | a predicate in the alignment grader's own grammar (`model/fixtures/alignment/README.md`): `kind:arg`, matching case-folded, `\|` between alternatives |
| `instantiate` | how the row is made for one draft (below) |
| `asked` | optional: alternatives (`\|`) of which the person's words must carry one for the row to apply |
| `teaching` | the sentence of the body's language the predicate holds |
| `source` | where that sentence lives |

`docs/vision-kb/language-guide.md` is the Coherence Network language guide, in
the checkout that holds this kernel as its `form/` submodule;
`teachings/uplifting-dialogue.md` is the dialogue covenant it names among its
shared sources; `form/form-stdlib/role-prompts.bml` holds `rp-sema`, the role
the voice wears.

## Made for one draft

What the drafter was given is read from the draft file itself: the person's
words (`# The person asked`), the turn before (`# The turn before (what the
voice was given)`) and the paths of the files it read (`# Files the voice
read`).

| instantiate | the predicate for one draft |
|---|---|
| `as-written` | the predicate as written, over the draft |
| `minus-given` | a `lacks_any` whose alternatives the drafter was given are set aside: the person's own words stay theirs to use. When none remain the row does not apply |
| `each-named-path` | one predicate for each path the draft names that the drafter was not given: `contains:<path>` when the path was held at the draft's time, else the row's own predicate over it (`lacks:<path>`). A named path is a word with a directory and a file name carrying an extension (one to eight letters or digits), read between spaces and between backticks (`` `fkwu.c`/`fkwu.rs` `` is two words, neither a path); a placeholder (`<`, `>`, `*`, `{`, `}`, `$`, `\|`, `;`, `=`, an ellipsis, `path/to/`), a URL and a home path (`~`) name no file, a leading label (`gap:tests/x.gap`) and a trailing `:<line>` are set aside. Held at the draft's time is read through git: the newest commit of the checkout at or before the draft's latest path row, and that commit's tree. A path git cannot date holds when it stands now: an absolute path (the host's, outside any tree) and a path git never held (a hearth's file). A path born after the draft was not held then |
| `when-given-no-number` | `lacks_digit` over the draft's own claims, applied only when the drafter was given no number: no digit in the person's words or the turn before, and no file read (a file's text is what the drafter saw and this reading does not hold). A word claiming no amount is set aside: a list ordinal at a line's start (`1.`, `2)`, `Step 3:`), a path it names, the drafter's own cut mark, a name carrying a digit (the word's first letter or digit is a letter: `M1`, `F1`, `flake8`, `sha256`), a bare zero (`exit 0`, `0 failed`: the clean reading names no amount), a code (an integer after `exit`, `code`, `status` or `http`: `exit 1`, `error code (e.g., 403, 500)`) and a step (an integer or range after `step`, `rung`, `phase`, `stage`, `part` or `option`: `steps 1-4`). A quantity stands: `5%`, `4.5/5`, `10-second`, `5-10 prompts`, `80%` |

## The predicates

- `receipt-words` — the door's receipt (`path`, `grounded`, `freq`, `suffic`)
  belongs to the report, out of a draft's prose: the receipt's own key forms.
- `trust-framing` — no military or security framing: the alignment set's own
  trust words (`defense`, `attack`, `guard against`, `threat`, `weapon`,
  `lock down`) and the framings that sit beside them.
- `affirmative` — where the person asks for a page or status line, the line
  names what is and what carries it now: the alignment set's own
  `affirm-status` words, the draft as written. Elsewhere honest negation
  stays: affirmative expression keeps consequential facts, uncertainty and
  boundaries intact (`teachings/uplifting-dialogue.md`), so "I have not read
  that file" and "call the new implementations instead of the kernel hooks"
  hold.
- `named-paths` — every path the draft names stood in this body at the draft's
  time, or was given.
- `given-numbers` — no amount where the drafter was given none.

A predicate establishes only what it checks; the voice's reading, after the
covenant holds, is the reading of the whole draft, and our own reading
(`observe/draft-reading-run.bml`) stays the judge the numbers do not replace.
A predicate changes only when a sound draft was refused by its word list
alone, and each such change is named here. `affirmative` was first
`lacks_any:no longer|instead of` over every draft; graded over the 53 drafts
whose latest reading was ours (2026-10-29), it refused 3 of the 25 we had
accepted on those words alone (`CopyDrift` "no longer needs to pretend", "call
the new Form-native implementations instead of the kernel hooks", "if it is no
longer required"), so it now applies where the rule it holds applies: page and
status lines. In the same reading `named-paths` failed 9 drafts, each on files
no commit ever held (`core/flow.py`, `src/kernel/mod.rs`, `docs/favicon.ico`);
one of them we had accepted for its idea, and the covenant stays stricter
there, because an accepted draft becomes a trainer row and would teach the
invented paths. On 2026-10-31 `named-paths` moved from existence now to the tree
at the draft's time (it refused draft `8faa6083-…-1790327224` for
`receipts/local-agent-goal-ledger.jsonl`, a ledger that stood when it was
written), and `given-numbers` learned the words that claim no amount (of its 11
refusals over the 146 readable pending drafts, the words alone drove `M1`, `F1`,
`(0)`, `403, 500`).
