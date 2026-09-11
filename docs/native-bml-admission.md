# Native BML source admission

Form owns executable `section [form.bml]` admission in
`form/form-stdlib/source-compiler.fk`. One lexical boundary reads statements,
declaration headers and balanced bodies from their original source positions.
Line breaks do not define scope. Strings retain their escaped punctuation;
comments do not contribute delimiters or executable statements.

Function and class bodies may share a line. The next declaration or expression
starts after the matching closing brace, including a suffix on that same line.
Class methods and their descriptors use the same body boundaries. Ref resolution
walks class declarations and members while skipping complete method bodies.
Unrecognized members refuse admission.

Both `do { ... }` and `do ... end` lower into a Form `do` recipe. Each statement
keeps its order, lexical bindings and result; the last expression supplies the
body's value. `if` selects one branch. The compiler admits a complete call,
string, grouped expression or body and refuses an unconsumed suffix, mismatched
delimiter, incomplete definition or missing branch.

```bml
section [form.bml] {
  class Example {
    def answer(enabled) {
      let base = 40;
      if enabled then do { add(base, 2); } else 0;
    }
  }
  answer(1);
}
```

The executable door lowers through `bml-floor-compile.fk` in a RAM pipe.
The seed's existing floor digest includes the Form compiler's dependency chain,
so a changed compiler invalidates affected cached BML images. Source admission
still compiles a whole unit synchronously. On-demand specialization, publication
under concurrent work and complete portable native emission remain
[north-star work](fkwu-form-native-north-star.md).

The separate cursor grammar in `grammars/form-bml.fk` is an explicit proof
surface. It does not select executable meaning through a comparison with another
parser. Its witnesses require complete parsing and the expected recipe
structure; a failed parse cannot certify a supported construct.

The [execution witness](../observe/bml-statement-admission-run.bml) creates
private compiler inputs, captures complete output and actual child status, and
retains source and runtime identities. It observes ordered effects, results,
nonselected branches, quoted punctuation, comments and actual refusal. Its
diagnostic flow retains the observation and correlated response and action.
Expected nonzero children are identified individually; an unexpected nonzero
child or mismatched result prevents acceptance.

The [current execution](evidence/fkwu/bml-admission.json) accepts 32 cases and
retains 24 source/runtime identities, checked before and after execution.
Successful children have empty stderr. Deliberately refused children retain
their actual nonzero status and diagnostic. The witness's own refusal retains
three correlated observation, response and applied health rows through the
shared process reader.
The outer runner retains its cache-renewal warning separately from those child
streams.

The [cache renewal execution](evidence/fkwu/bml-cache-renewal.json) holds the
same source and its old cached image, then invokes the ordinary source runner.
Form renews the image and executes the complete body. The source, root binary
and handwritten C remain unchanged; the compiler changes. The expected cache
renewal warning remains in the evidence.

The [integration reading](evidence/fkwu/bml-admission-integration.json) retains
fresh preflight and exact output for ten existing compiler, class/ref, numeric
literal, cursor and health checks. The compiler health owner reports scoped
`observed` or `unobserved` state from complete source reads, ontology coverage,
round trips and actual text emission. It does not certify every construct or
platform. A missing source root is unobserved.

The [retained-source re-observation](evidence/fkwu/dsv4-held-source-rewitness.json)
walks all 81 held DSV4 source rows in order and refuses a changed final hash.
It preserves the original execution identities and does not repeat or relabel
the numerical runs. The [admission baseline](evidence/fkwu/bml-admission-baseline.json)
retains its original, narrower identity scope as separate execution evidence.

Custom BMF sections keep their rule data, including bare `do` and `end` names.
Their quote/comment/brace reader does not assign executable BML scope to those
names. Raw Form before and after sections remains passthrough source. The
[section observations](evidence/fkwu/bml-section-dialects.json) check emitted
rule structure, actual closing-byte positions and raw-source preservation.
Within BML, an adjacent `do(...)` call and a whitespace-separated
`do (...) ... end` block retain their respective interpretations.
