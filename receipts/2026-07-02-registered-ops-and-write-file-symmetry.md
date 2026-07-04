# 2026-07-02 — "registered ops": what they are, whether it's best practice, and the write_file wart fixed

## Ground

```sh
grep -c '{ "' runtime/fkwu-optable.h        # 151 registered ops (generated from flt-ops)
./fkwu --src bootstrap/ground.fk            # 42
```

Urs, on my phrase "write_file isn't a registered op": *"what? we have ops that are registered and ops
that are not? is that best practices of other languages like python, C#, Java, C++?"*

## The honest answer, in three parts

**1. Having a registry of built-in ops is completely standard.** A "registered op" here is an entry in
`runtime/fkwu-optable.h` — one of **151** built-in primitives (name → arity → tag), generated from the
single source of truth `flatten/form-flatten.fk`'s `flt-ops` list (the `.h` says "DO NOT HAND-EDIT;
adding a value op = a flt-ops row → regen"). Every language has exactly this: Python's `builtins`
namespace (~150 built-ins: `print`, `len`, `open`), C's standard library + `__builtin_*` intrinsics,
Java's auto-imported `java.lang`, C++'s `std::`. And every language resolves names in layers — special
forms → built-ins → user definitions → locals — precisely as this kernel does (`defn`/`do`/`let`/`if` →
optable → rewrites → user `defn` → binding stack). So "some names are ops, some aren't" just means "the
language has built-in functions." Nothing unusual.

**2. Where the kernel DID diverge from best practice — and it's the part worth caring about — is what
happens when a name resolves to NEITHER a built-in NOR a user definition.** Every mainstream language
makes that an **error**: Python raises `NameError: name 'write_file' is not defined`; C99/C++/Java/C#
give a compile error (undeclared identifier / cannot find symbol). This kernel historically declined to
`nothing` — silent (axiom-5, the offer/ack model; closest mainstream cousins are Smalltalk's
`doesNotUnderstand:` and Ruby's `method_missing`, but even those raise by default). That silence is the
real non-standard footgun, and it is exactly what the compile witness built earlier this session now
catches — it printed `[unresolved-call] 'write_file'` at parse time when I mistyped, and today caught my
`str_find`/`substring` missing-prelude mistake in the optable generator the same way. We moved toward
best practice (undefined names are now visible), while keeping recover-to-nothing as a deliberate,
defeasible choice rather than a silent one.

**3. The specific `read_file` / `write_file_text` mismatch was a genuine wart, not a principle.**
Mainstream stdlibs are symmetric: `read`/`write`, `fread`/`fwrite`, `f.read()`/`f.write()`. Having
`read_file` (tag 63) but no `write_file` — only `write_file_text` (tag 104) — is the kind of asymmetry
that trips people (it tripped me, one prompt ago). Fixed: added `write_file` (arity 2, tag 104) as the
symmetric name, through the proper path — a `flt-ops` row + the effect-op gate in `form-flatten.fk`,
then regenerated `runtime/fkwu-optable.h` with two `fkwu --src` calls (zero bash), never hand-editing
the generated table. `write_file` now works (`(write_file p "…")` → bytes written, round-trips);
`write_file_text` remains for its 12 existing callers.

## Witnessed

- `write_file` registered: `{ "write_file", 2, 104 }` in the regenerated optable.
- Live: `(write_file path "symmetric now")` → 13, file round-trips; `write_file_text` unchanged.
- Canaries 42/15/11111, corpus band 127 four-way — the regen changed only the one data table.

## Honest floor

`write_file` and `write_file_text` are now two names for tag 104 — a deliberate non-breaking alias, not
a rename (renaming would touch 12 callers plus the generator that bootstraps on `write_file_text`).
Migrating callers to the canonical `write_file` and retiring `write_file_text` is the completing step,
left as a named follow-up, not banked silently. `write_file` is a host-I/O effect op (fkwu-carrier),
so it is fkwu-only, not four-way — like `read_file`.

## The most surprising teaching this work left behind

The question exposed that my own phrasing hid the real issue. "Isn't a registered op" made it sound
like a quirk of this language, when the registry is universal — and the actually-unusual thing
(silent decline instead of an error on a truly-undefined name) is a different property entirely, one we
had already started curing. Naming a thing loosely can point attention at the wrong surface; the precise
question "registered vs not — is that normal?" is what separated the ordinary (built-in tables) from the
exceptional (decline-to-nothing) and from the mere wart (asymmetric naming).

## Where discomfort turned to gold

The discomfort: the compile witness caught me twice in one session — first `write_file` (the typo), then
`str_find`/`substring` (I ran the optable generator without its `core.fk` prelude). The reflex is
embarrassment that the diagnostic keeps catching *me*. The gold is that this is the diagnostic working
as designed on its author: a tool built to make undefined names loud does not care whose hand made the
mistake. Being caught by my own guardrail is the guardrail earning its keep.

## Corpus

Row 654 **suppletion** — in linguistics, when the forms of one paradigm come from unrelated roots
(go/went, good/better): a set that ought to be uniform but isn't (fresh; the `read_file` /
`write_file_text` asymmetry — a read/write pair whose two halves were shaped by different naming
instincts, now made regular by adding `write_file`).
