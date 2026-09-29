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

Arithmetic, ordering and boolean operators lower to existing native Form
recipes, preserving evaluation order and short-circuit behavior. Binary operators
and conditional punctuation are separated by whitespace so hyphenated Form names
remain whole. Parentheses select grouping. `!` negates an expression.

The executable reader and cursor grammar share the operator table in
`grammars/form-bml.fk`. From lowest to highest precedence: `|>`, `??`, `||`,
`&&`, equality, ordering, `..`, addition/subtraction, multiplication/division/remainder.
Binary operators associate left. Conditional expressions associate right, bind
below these operators, and evaluate only the selected branch.

```bml
def magnitude(x) = x < 0 ? 0 - x : x;
def weighted(scale) = 0 .. 8 |> map(x => x * scale) |> filter(x => x > 0);
def total(values) = values |> foldl((sum,x) => sum + x,0);
def matching(values) = values |> filter((text: String) => text == "ready");
def affine(scale) = offset => x => scale * x + offset;
affine(3)(2)(4);
```

`start .. end` calls the existing half-open `range`. `x => expression`,
`(x,y) => expression`, and `() => expression` create lexical functions; a braced
body can contain ordered statements and local definitions. Parameter comparison
contracts and captured values travel with the function. A function can escape its
owner and be called immediately or through a returned function. Lambda bodies
associate right. Local named functions use the same capture lowering.

`input |> stage(arguments)` evaluates input once, before the stage arguments,
and supplies it as the last argument. A bare function or grouped lambda is also
a stage. This composes existing `map`, `filter`, `foldl` and other Form functions
without a separate collection runtime. Group a conditional or lambda when using
it as a pipeline operand.

`try e catch h` (also `try e else h`) attempts `e`; an absent answer or a
caught runtime stop selects `h`. `choice { e1; e2; ... }` (also `choose`)
attempts options in order and returns the first non-absent answer, including zero.
`x ?? y` evaluates `y` only when `x` is absent; it does not add an attempt.
Unselected branches remain unevaluated. Each caught runtime stop emits its
existing organ-health signal.

```bml
def size(x) = try len(x) catch 0;
def first-reading(x) = choice { len(x); str_len(x); 0 };
def label(name) = name ?? "unnamed";
```

Intrinsic call names lower directly to existing Form owners. Their argument
counts are checked after pipeline composition. Import the owning library through
`preludes:` as with other native calls.

| Surface | Existing owner | Meaning |
| --- | --- | --- |
| `fail`, `stop`, `fail()`, `stop()` | canonical `nothing` | An absence signal interpreted by its receiving boundary; no imperative abort. |
| `offer(recipe,args)`, `choice(alternatives,args)` | `control/offer-ack-core.fk` | One attempted offer, or the first non-absent offer. |
| `cut(alternatives,args)` | `control/choice-lane-core.fk` | Offer only the first alternative. |
| `store(memory)`, `restore(checkpoint)`, `undo(ack,checkpoint,memory)` | `control/choice-lane-core.fk` | Hold immutable memory; select the checkpoint on an absent acknowledgement. Host effects are not rolled back. |
| `repeat(owner,receiver,context,cursor,options)` | `bml/native-events-darwin.bml` | Schedule a locally owned repeat on the existing event queue. |
| `again(cursor)` | same event owner | Return a changed checkpoint for the next queued pass. |
| `retry(offered,cursor)` | same event owner | Propose a changed checkpoint during local care, retaining context and findings. |

A normal repeat result, including zero or nothing, completes the work. `again`
yields to peer work. An unchanged checkpoint rests in care; a resource event can
resume the original continuation. The owner retains cancellation and release.
The [native repeat observation](../observe/native-repeat-witness.bml) exercises
these expressions through actual queued work, recovery, resource wake and release.

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

## Local lift

A lift stays inside Form and admits every door the catalog offers whose body
is Form and whose carrier is local. File and shared memory are two of those
doors. `host:memory` is the adapter over `shm_offer` / `shm_receive`. HTTP,
audio, video and speech are open on the same terms. A name that is not a door
stays absent. Closing a catalog door is not part of the lift.

The executable face is `form/form-stdlib/form-lift.bml`. It names the word
width, the flag width, and the present zero. `template
LiftCrossing<Protocol, Carrier>` is the generic shape. A class owns admission.
`|> filter` keeps a crossing when the body is Form and the door is local.
`??` restores a named flag when a value is absent. The witness reads 144:
six catalog doors, 129 named bits, six admissions, an absent unknown name,
one restored absence, a full catalog, and file first.
