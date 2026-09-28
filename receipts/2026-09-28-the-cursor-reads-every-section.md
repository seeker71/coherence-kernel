# The cursor reads every section

The source compiler reads a `form.bml` section statement by statement: it finds
each statement's end over text and compiles it alone. The cursor reads the same
section whole, with direct backtracking. Today it reads every section the compiler
reads, to the same recipes NodeID for NodeID after the contract pass they share:
1093 of the tree's 1094 sections. The one left is a receipt artifact whose section
the compiler's own finder cannot bound either.

The cursor learned what the body writes, in the order the body showed it:

- classes, generic, attributed (`[public, observable, jit]`) and language classes
  over a template; `field`, `ref` and `thought` members; the class's descriptor,
  hoisted in place as the compiler hoists it
- templates with their members (`member held: Offer<T>;`) and one-line templates
- `import`, top-level `let`, a `;` after a def or class block
- comparison contracts on names, params and returns (`def f(x: Number): String`)
- `match s { pattern => body, _ => default }`
- `-9` and `-1.5e-3`, and `nothing()` as the call it is

`observe/bml-cursor-coverage-run.bml` is the instrument. It walks every `.bml`
with `source_inventory`, counts parity, names the first word the cursor does not
read (descending into a class or def block to the member that stops), prints where
two lowerings part with the compiler's own recipe printer, and carries each
reader's time. `observe/bml-one-comment-run.bml` rewrites a section's `;` comment
lines as `//`, so BML speaks one comment; 36 lines in six files went through it.

Found on the way and healed:

- The compiler read `1e-12` as a name. The recipe held an ident `1e-12`; the text
  round trip printed it and the kernel's reader took it back as a float, so it ran
  right. Both readers now read an exponent as the float it is.
- On fkwu a raised `form_error` ended the process even inside an `attempt`; Go,
  Rust and TS already caught it there. It is a stop now, voiced and unwound to the
  recover point; outside every attempt it still ends aloud with its exact line.
  `control/tests/attempt-band` 4095 four-way.
- `cons` onto nothing stops on every arm (R143 released).
- The prefix engine reads a keyword's name boundary and an exponent as the parser
  does (`bmf-prefix-state-band` 4194303).
- fkwu's runtime moved, so validate asked the siblings, and a four-way sweep of the
  54 bands this work touches found what main never asks: Go's source lens could not
  reach `learn/homecoming-distillation-corpus.fk` or `control/offer-ack-core.fk`.
  A kernel run from `form/` names an owner `form-stdlib/x.fk`, and no dependency
  candidate climbed to the checkout's root. The candidates now end with that root
  as every kernel's read-side doors find it, the first directory up holding `.git`;
  every earlier candidate answers first, and the corpus-closure bands agree
  four-way. Four bands whose subject is fkwu's own surface (fkwu children through
  `host_spawn_at`, in-process machine-code leaves, the gift roster) now declare the
  fourth-arm lane they always were. The 21 python-bmf bands answer exactly what
  main answers.

Pace, read beside a running sweep, so only the ratio counts: rule references go
through an index built once instead of a walk of the rule list; `skip-ws` reads one
byte before it reads a two-byte window; the lowering probes its categories once per
process. The cursor went from 2282 to 852 ms on `form-cli-code-policy.bml` (the
walker 639), from 4127 to 1485 on `form-glass-live.bml` (1325), and reads the
string-heavy ledger in 267 against the walker's 828.

R144 is open: the cursor becomes the one reader and the statement walker leaves,
once a refused section names its line and word as the walker's diagnostics do.

**Surprise.** The ident `1e-12` was a wrong meaning that ran right: the text round
trip re-read the printed name as a number. The text between a recipe and the
kernel was hiding a mistake. Reading source directly into Form is not only faster;
it leaves no second reader to make two wrongs agree.

**Discomfort to gold.** The first whole-tree walk died on a `form_error` its own
`try` could not catch. The instrument dying on the thing it measured felt like a
broken tool. It was the language telling the truth about itself: fkwu was the one
arm where a raised error could not be met with choice. Aligning the gold made the
door survive and the recover point whole.
