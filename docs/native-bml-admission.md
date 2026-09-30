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
def point(x,y) = [x,y];
def x-coordinate(point) = point[0];
def operation(ops,index,input) = ops[index](input);
```

`[a,b]` and `[]` construct native lists. `rows[index]` lowers directly to
`nth(rows,index)`; indexes are zero-based and retain the primitive's absence
and validation behavior. Nested literals and chained indexes work with calls,
grouped expressions and lexical functions. Elements, receiver and index execute
in their written order, once each. These forms add no collection runtime or
temporary bindings. A comma separates complete elements; an empty element,
trailing comma or empty index has no executable meaning.

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

`match value { pattern => expression, _ => otherwise }` expresses ordered
exact-value dispatch. It preserves integer/float distinctions, evaluates a
computed subject once, and executes only the selected body. Patterns are values,
not destructuring declarations or type tests. Use a named accessor for a tuple's
meaning; use indexing inside that accessor. Comparison contracts still belong
on comparisons that require String or numeric promotion.

Intrinsic call names resolve to existing Form owners after lexical binding.
Parameters, local values, captured functions and declared functions take
precedence over an intrinsic alias, including bare `stop` and `fail`. Unbound
bare `stop` and `fail` still mean absence. Intrinsic argument counts are checked
after pipeline composition and name resolution. Import the owning library through
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

The cursor grammar in `grammars/form-bml.fk` supplies expression recognition for
native BML surface lifting and an explicit proof surface. Executable admission
still owns compilation. Cursor witnesses require complete parsing and the
expected recipe structure; a failed parse cannot certify a supported construct.
The lifter retains original byte spans around recognized calls and verifies the
candidate with compiler recipes.

The bands in `form/form-stdlib/tests/` run the compiler on private inputs and
read the actual child status, so a successful process exit alone establishes
nothing: a band names the effects, results and refusals it observed.

Custom BMF sections keep their rule data, including bare `do` and `end` names.
Their quote/comment/brace reader does not assign executable BML scope to those
names. Raw Form before and after sections remains passthrough source. Within
BML, an adjacent `do(...)` call and a whitespace-separated `do (...) ... end`
block retain their respective interpretations.

## Local lift

`form/form-stdlib/form-lift.bml` supplies lifting guidance to the public
native `code` controller whenever the supplied sources contain executable
BML. The controller retains the original sources, writable paths, checks and
checkpoint. Its tools read and edit resident document values in-process;
filesystem loading/publication belongs to the caller, and live telemetry uses
shared memory. No model-selected shell command, network tool or provider is
admitted by this path. The host capability catalog remains a separate inventory.

Use lexical lambdas and the existing sequence functions before introducing
another walk. `table.bml` provides generic row operations and compatibility
adapters for existing carried-argument callers. `tb-first(rows,predicate)`
returns `nothing()` on a miss while preserving zero and empty matching values.
Named accessors own tuple layouts; domain constants belong to their semantic
owner. Classes group APIs. Templates describe parameters and members; instance
accessors, interface checking and specialization are not implied.

Choose the smallest pattern that states the operation:

| Intent | Native expression |
| --- | --- |
| Construct or read a row | `[key,value]`, `row[1]` inside a named accessor |
| Select one value | `condition ? yes : no`; `match` for exact-value alternatives |
| Supply an absent answer | `reading ?? other`; zero and empty lists remain answers |
| Transform a sequence | `rows |> filter(keep) |> map(project)` |
| Accumulate in order | `rows |> foldl((state,row) => next(state,row),initial)` |
| Retain local context | `scale => value => scale * value` |
| Recover or choose | `try work catch care`; `choice { first; second; }` |
| Continue owned asynchronous work | `repeat`, `again`, `retry` with the existing event owner |

Run a defined lift through `code_entry: "direct"` with the actual callers and
helper contracts supplied. Inspect the candidate, execute its behavior and read
its frames before publishing. Compact source, preserved semantics and measured
runtime cost are separate observations. [Native coding](form-native-coding.md)
describes the request and checkpoint interface.
