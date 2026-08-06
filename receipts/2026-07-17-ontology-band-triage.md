# 2026-07-17 — ontology band triage: 21 divergent named, 353 names come home to the Form table

## Ground

Branch `claude/bp-registry-seating` off origin/main (332352329); `cc -O2 -o fkwu
runtime/fkwu-uni.c` clean, `./fkwu --src bootstrap/ground.fk` → 42 before and
after every change. Gate of record: `form/scripts/fourth-arm-gate.sh` — PASS is
only the PASS-4WAY line (fkwu's form_error is still numb per the axiom-5
lowering receipt, so ts+fkwu-green proves nothing about bp coverage; only the
go/rust legs testify).

Scope: the 96 bands whose preludes include `form-stdlib/form-ontology-loader.fk`
(`grep -l 'form-ontology-loader' form-stdlib/tests/*-band.fk`).

## Baseline sweep (before any change)

Full gate over all 96 stems:

| verdict | count |
|---|---|
| PASS-4WAY | 12 |
| NO-FOURTH (0 divergent, fourth arm silent) | 63 |
| DIVERGENT | 21 |

PASS-4WAY (12): bml-choice-receipt-row, bml-protocol-capability-row,
const-recipe, form-bml-cursor-full, form-bml-cursor-parse,
json-meaning-ingestion, natural-language, nl-reason, nl-translate, shell-awk,
shell-exec, translate-lane.

NO-FOURTH (63): audio-bmf-reversible, binary-symbol-lens,
bmf-bml-compiler-picture, bmf-choice-receipt, bmf-generic-language-scanner,
bmf-source-scanner-rule, bml-action-runtime, context-domain-media-lens,
context-lens-selection, dynamic-adapter-registry, form-action-dialect,
form-action-migrated-bmf, form-action-migrated-core, form-ontology-parity,
generic-reverse-emitter, go-bmf-reversible, image-bmf-reversible,
intrinsic-cast, intrinsic-cast-check, media-container-depth,
media-rich-metadata, program-image-recipe-carrier, program-image-typed-carrier,
prolog-bmf-eval, prolog-bmf-reversible, python-bmf-arithmetic, python-bmf-attr,
python-bmf-class, python-bmf-comprehension, python-bmf-decorator,
python-bmf-eval, python-bmf-evaluator-voice, python-bmf-exception,
python-bmf-from-import, python-bmf-fstring-slice, python-bmf-full-file,
python-bmf-grammar, python-bmf-lift, python-bmf-module-parse, python-bmf-repo,
python-bmf-reversible, python-bmf-scanner-real-syntax, python-bmf-typeann,
python-class, python-export, recipe-capsule-abi,
runtime-dynamic-capability-registry, runtime-real-grammar-capsules,
runtime-shared-native-representation, shell-v2, source-compile-multibyte,
source-compiler-health, source-compiler-multi-dialect,
source-compiler-text-lens, source-corpus-roundtrip,
source-language-match-switch, source-language-route-class-template,
source-runner-admission, source-scanner-escape, typed-literal-carrier,
typescript-bmf-eval, typescript-bmf-lift, video-bmf-reversible.

(The task brief expected ~56 divergent; the gate today answers 21 divergent +
63 no-fourth. The difference is honest: three-leg agreement with a silent
fourth arm is not divergence, and the NL seating that landed in #277 already
healed part of the field.)

## Triage of the 21 DIVERGENT bands

### Family (a) — bp: unreviewed bootstrap name (11 bands)

go and rust both crash in `bp@form-stdlib/form-ontology-loader.fk:349` (the
loudest witness the numb fkwu leg cannot give). First missing name per band,
from the gate's own crash lines:

| band | first missing name | registry coords |
|---|---|---|
| bml | UE-WRITE-ASSIGN | 1/2/16/1 (alias of F-ASN) |
| bml-generics | UE-CLASS | 1/2/30/1 |
| bml-native-mutable-locals | BML-MUTABLE-LOCAL-FRAME | 1/2/99/9320 |
| bmf-to-fk | MS-PAIR | 1/2/99/1300 |
| file-io-shell | CELL-V0 | 1/2/99/1740 |
| lang-convergence | UE-JUMP-RETURN | 1/2/18/1 (alias of F-RET) |
| language-packs-fourth | MS-PAIR | 1/2/99/1300 |
| live-room-shell | CELL-V0 | 1/2/99/1740 |
| markdown-meaning-ingestion | UNKNOWN-FORMAT | 1/2/99/100 |
| shell-cell | CELL-V0 | 1/2/99/1740 |
| yaml-meaning-ingestion | UNKNOWN-FORMAT | 1/2/99/100 |

The first name is only the first domino: the full requirement per band was
recovered statically — every literal `(bp "NAME")` call site in the band's
declared prelude closure (comment lines stripped), plus, for the bml family,
the registry-known upper-case names the bands pass through their `N`/`cat?`
helpers. The big surprise:
`form-stdlib/seedbank/blueprint-symbol-sections.fk` — the intentional home of
Blueprint name literals — calls `(bp "NAME")` at top level for every one of its
~350 declarations, so loading it (markdown/yaml-meaning-ingestion do) requires
the entire section, not a handful of rows.

### Family (b) — as_int: null crashes (2 bands)

| band | exact error |
|---|---|
| document-meaning-ingestion | go `as_int: null`, rust `fatal[type_contract_violation]: as_int: Null` — both at `document-cfbf?` / `document-parse-cfbf-bytes` (source-compiled cache `147e117e1e994afa-….fk:92`) |
| repo-file-ingestion | go `as_int: null` at `image-png-signature?`/`image-detect-bytes`; rust fails *differently*: `as_str: Null` at `str_len < repo_text_meaning` — two legs, two distinct null seams |

### Family (c) — value mismatches (4 bands)

Three reference kernels agree; the fourth arm answers a different number:

| band | go/rust/ts | fourth |
|---|---|---|
| form-bml-cursor-lower | 117 | 123 |
| md-grammar | 7 | 0 |
| shell-parse | 255 | 0 |
| tokenize-grammar | 400 | 100 |

### Family (d) — other (4 bands)

| band | exact error |
|---|---|
| bml-realfiles | go/rust: unbound function `bbc-default-window-size` at `cursor-file@bmf-core.fk:141` (name exists only in the fourth arm's shim world) |
| literals | same unbound `bbc-default-window-size` at the same site |
| runtime-grammar-selector-registry | go: `walk: unbound function ""` / rust: `unbound function: ` — an EMPTY function name reaches the walker |
| ts-reversible | go: `unbound identifier "list("` / rust: `unbound: list(` — a mangled token (`list(`) escapes scanning into resolution |

Families (b)(c)(d) are named and left for dedicated passes, per the brief.

## Seating (family (a) heal)

353 rows added to `FOL-BP-BOOTSTRAP-TABLE` in
`form/form-stdlib/form-ontology-loader.fk`, every coordinate copied verbatim
from `form/form-stdlib/blueprint-registry.json` (name or alias — the registry's
deliberate `reuse` aliases share coords by design: eleven spellings sit on
1/2/99/10 with JSON-OBJECT, and name-level coordinate duplication in the Form
table mirrors that, it is not a bug). No instance number was invented.
**needs-curation: none** — every name demanded by a family-(a) closure resolved
in the registry. (The only `(bp "…")` literals absent from the registry were
`NAME` and `X` inside the loader's own comment lines — doc examples, stripped
before counting.)

The seated names, grouped: UE-* universal-executable family (18: UE-ACCESS-*,
UE-BLOCK-WITH, UE-CALL, UE-CASE, UE-CATCH, UE-CLASS, UE-INTERFACE, UE-JUMP-*,
UE-LOOP, UE-MATH-NEGATE, UE-SWITCH, UE-THROW, UE-TRY, UE-WRITE-ASSIGN);
CELL-V0, MS-PAIR, BML-MUTABLE-LOCAL-FRAME, UNKNOWN-FORMAT, FORM-FILE-ROOT,
FORM-IDENT, FORM-LIST, TRUE-TRIV, FALSE-TRIV; the DOC-*/YAML-*/PNG-*/JSX-*
document shapes; the per-domain JSON aliases (AU-*, CSS-*, E-JSON-*, HTML-*,
IMG-*, MDL-*, SQL-*, VID-*, XML-*, Y-*, JSON-BNF-*); and the language sections
F-*, GO-*/GO-BNF-*/GO-EXEC-*/GOF-*, PY-*/PY-BNF-*/PY-EXEC-*/PY-FULL-*/PYC-*,
RS-*/RS-BNF-*/RS-EXEC-*/RSF-*, TS-*/TS-BNF-*/TS-EXEC-*/TSF-*, TSX-LET,
U-ASSIGN, U-RETURN. Exact rows and coords: see the diff of this commit; table
grew 311 → 664 rows.

## Re-gate after seating (the 11 family-(a) bands)

| band | before | after |
|---|---|---|
| bml-native-mutable-locals | DIVERGENT | **PASS-4WAY** |
| file-io-shell | DIVERGENT | **PASS-4WAY** |
| live-room-shell | DIVERGENT | **PASS-4WAY** |
| shell-cell | DIVERGENT | **PASS-4WAY** |
| bml-generics | DIVERGENT | NO-FOURTH (bp crash healed; 0 divergent; fourth silent) |
| lang-convergence | DIVERGENT | NO-FOURTH (same) |
| language-packs-fourth | DIVERGENT | NO-FOURTH (same) |
| markdown-meaning-ingestion | DIVERGENT | NO-FOURTH (same) |
| yaml-meaning-ingestion | DIVERGENT | NO-FOURTH (same) |
| bml | DIVERGENT | DIVERGENT — bp crash healed; now go/rust/ts = 131595711, fourth = 37752852 (moved into family (c)) |
| bmf-to-fk | DIVERGENT | DIVERGENT — bp crash healed; now go/rust/ts = 3426, fourth = 3036 (moved into family (c)) |

So: 4 × DIVERGENT→PASS-4WAY, 5 × DIVERGENT→three-leg-green (fourth arm silent,
same verdict class as 63 pre-existing bands), 2 × honest next failure
(fourth-arm value mismatch, named above).

Regression check: previously-green bands stay green with the doubled table —
const-recipe, natural-language, shell-exec, json-meaning-ingestion re-gate
PASS-4WAY; form-ontology-parity stays NO-FOURTH; ground.fk still answers 42.

## The most surprising teaching

That `blueprint-symbol-sections.fk` *is* a bp registry written as a prelude:
every declaration is a live top-level `(bp "NAME")` call, so "seat the one
missing name the crash shows" was never the real task — the crash at
UNKNOWN-FORMAT (the file's first let) was the whole section knocking. One
static read of actual call sites replaced what would have been ~350 rounds of
crash-seat-recrash.

## Where discomfort became gold

The bml and bmf-to-fk re-gates refusing to go green. The reflex was to call
seating "done" when the unreviewed-name crash vanished — but the gate answered
DIVERGENT again, with a *different* face: the fourth arm now computes a
different value than the three reference kernels. Sitting with that (instead of
re-running and hoping) is what split the verdict honestly: the bp floor is
healed, and a real fourth-arm disagreement that was always hiding behind the
crash is now visible and named. The crash was anesthesia; removing it let the
band say where it actually hurts.
