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
      if enabled then do { base + 2; } else 0;
    }
  }
  answer(1);
}
```

Arithmetic, ordering and boolean operators lower to the existing native Form
recipes, preserving evaluation order and short-circuit behavior. Binary operators
are separated by whitespace so hyphenated Form names remain whole. Parentheses
select grouping. `!` negates an expression.

A stop is a backtrack point, and BML names the choice in its own words.
`try e else h` attempts `e`; when `e` stops (the length of nothing, a branch on
nothing, arithmetic over a non-number, a recursion past the walker's wall) or
answers nothing, `h` answers. `choose { e1; e2; ... }` attempts each option in
order; the first that lands wins, and none landing is nothing. `x ?? y` is `x`
unless `x` is nothing; it binds loosest of the operators. Each lowers to
`(attempt ...)` and `nothing?` with the next option in the `if`'s branch, so an
option that is not needed is never walked, and each caught stop is voiced as one
organ-health line:

```bml
def size(x) = try len(x) else 0;
def first-reading(x) = choose { len(x); str_len(x); 0 };
def label(name) = name ?? "unnamed";
```

`form/form-stdlib/tests/bml-recover-surface-band.fk` reads 1023 on all four
kernels.

`==` and `!=` use exact value equality by default. Declared comparison contracts
select another relationship without runtime wrappers:

```bml
def same-text(a: String, b: String) = a == b;
def measured(): Number = 1;
def same-number(a: Number, b: Number) = a == b;
let exact: Value = measured();
```

`String` retains native string/nothing validation. `Number` and `Comparable`
select native scalar numeric promotion and the existing general comparison
relationship. `Value` selects exact value identity, including the distinction
between integer and floating values. These are comparison contracts, not a
general static type checker or a representation conversion. A `(Type)`
ascription can state the relationship at a polymorphic boundary. Conflicting
String and Number contracts refuse compilation; an explicit Value contract
selects exact comparison. Parameters, local bindings and return declarations
carry contracts; imported return declarations follow the admitted source order.

Dependencies and selected function homes are read from the same emitted Form
surface. Native primitive names in call position retain their built-in meaning;
the same names in value position can select a declared function home. Registry
selection belongs to the entry unit and travels through its dependency graph.
Comment directives start with `preludes:` or `import`; quoted source and ordinary
prose do not introduce dependencies. Raw Form imports remain active beside BML
sections, including imports of `.fk` files that themselves carry BML sections.

The executable door lowers through `bml-floor-compile.fk` in a RAM pipe. Form
owns the source manifest for the optional `.lowfk` memo: the exact owner and
working context and selected registry, imported source and compiler bytes, and absent earlier lookup
candidates. The checkout carrier checks the packet, publishes it atomically and
rejects incomplete or changed observations. The native `.fkb` image retains its
own dependency identity checks. Changing an erased source annotation can change
the emitted caller and therefore invalidates the lowering memo.

Source admission still compiles a whole unit synchronously. On-demand
specialization and complete portable native emission remain
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

Successful witness children have empty stderr. Deliberately refused children
retain their actual nonzero status and diagnostic. The witness's own refusal
retains three correlated observation, response and applied health rows through
the shared process reader. The outer runner retains cache-renewal warnings
separately from those child streams. Its report names the current case count,
source identities and exact observations; a successful process exit alone does
not establish those behaviors.

Custom BMF sections keep their rule data, including bare `do` and `end` names.
Their quote/comment/brace reader does not assign executable BML scope to those
names. Raw Form before and after sections remains passthrough source. The
[section observations](evidence/fkwu/bml-section-dialects.json) check emitted
rule structure, actual closing-byte positions and raw-source preservation.
Within BML, an adjacent `do(...)` call and a whitespace-separated
`do (...) ... end` block retain their respective interpretations.
