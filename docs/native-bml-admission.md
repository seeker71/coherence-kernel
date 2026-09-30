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
recipes, preserving evaluation order and short-circuit behavior. `+ * / %`
read the same with or without spaces: `a+b*2` is `a + (b * 2)`. A hyphen
subtracts only with space on both sides, so `a-b` stays one Form name.
`a ++ b` joins strings through `str_concat`, associates right and binds like
addition. `xs[i]` reads position `i` of a list. Parentheses select grouping.
`!` negates an expression.

The executable reader and cursor grammar share the operator table in
`grammars/form-bml.fk`. From lowest to highest precedence: `|>`, `??`, `||`,
`&&`, equality, ordering, `..`, addition/subtraction/`++`,
multiplication/division/remainder. Binary operators associate left; `++`
associates right. Conditional expressions associate right, bind below these
operators, and evaluate only the selected branch.

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

`let [a, _, c] = expr;` binds the named positions of a list and skips `_`.

`try e catch h` (also `try e else h`) attempts `e`; an absent answer or a
caught runtime stop selects `h`. `choice { e1; e2; ... }` (also `choose`)
attempts options in order and returns the first non-absent answer, including zero;
write the block when there are two or more options. `x ?? y` evaluates `y` only
when `x` is absent; it does not add an attempt.
Unselected branches remain unevaluated. Each caught runtime stop emits its
existing organ-health signal.

```bml
def size(x) = try str_len(x) catch 0;
def first-reading(x) = choice { str_len(x); len(x); 0 };
def label(name) = name ?? "unnamed";
def middle(xs) {
  let [_, b, _] = xs;
  b;
}
```

## Owned resources

`defer expr;` runs `expr` when its block ends. Several defers run in reverse
order (last registered, first released), on a normal exit and on a stop.

A class that owns a handle names its release once:

```bml
class Slot : Disposable<Int> {
  def dispose(h) = release-slot(h);
}
```

`Disposable<H>` (in `form-stdlib/bml/disposable.bml`) hoists `Slot_dispose`;
`disposable-conforms?(Slot)` reads whether a class carries the release. Pair
the acquire with a `defer` so every path releases: `let h = acquire(); defer
Slot_dispose(h);`.

## Waiting and retrying

`form-stdlib/bml/retry.bml` supplies the two bounded shapes a waiter needs.
`wait-until(ready, deadline, every)` polls the lambda `ready` every `every`
milliseconds and answers 1 when it is true, or 0 once `host_monotonic_ms()`
passes `deadline`. `retry-first(count, every, once)` calls `once(i)` for `i`
from 0 up to `count` and answers the first present result, or `nothing()`; a
stop inside an attempt counts as no result. The `repeat`/`again`/`retry`
intrinsics below belong to native queued events; a plain poll or retry uses
these helpers.

```bml
def second-try() = retry-first(4, 0, i => i == 0 ? nothing() : 100 + i);
def ready-soon(ready) = wait-until(ready, host_monotonic_ms() + 50, 0);
```

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
| `store(memory)`, `restore(checkpoint)`, `undo(ack,checkpoint,memory)` | `control/choice-lane-core.fk` | Hold immutable memory; select the checkpoint on an absent acknowledgement. `undo` evaluates all three arguments before it selects, so give it values, not effects it should skip. Host effects are not rolled back. |
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

The bands in `form/form-stdlib/tests/` run the compiler on private inputs and
read the actual child status, so a successful process exit alone establishes
nothing: a band names the effects, results and refusals it observed.

Custom BMF sections keep their rule data, including bare `do` and `end` names.
Their quote/comment/brace reader does not assign executable BML scope to those
names. Raw Form before and after sections remains passthrough source. Within
BML, an adjacent `do(...)` call and a whitespace-separated `do (...) ... end`
block retain their respective interpretations.

## Control patterns

Absence is `nothing()`. Zero, an empty list and an empty string are present
answers. A pattern starts by letting one boundary turn "did not arrive" into
`nothing()`; after that every primitive below composes with the rest. Each
snippet ran under `./fkwu`; the spot is where the body already stands on it.
Import the owners through `preludes:` (`control/offer-ack-core.fk` for
`choice`, `control/choice-lane-core.fk` for `cut`, `store`, `undo`,
`oac-backtrack-walk` and `oac-timeout-walk`).

| Primitive | Asks | Owner |
| --- | --- | --- |
| `x ?? y` | is this reading here, else that one? | core |
| `choice { }` | which offer arrives first? | `oac-choice` |
| `try e else h` | did the call stop? | core `attempt` |
| `oac-backtrack-walk` | which option lands, given what earlier ones left? | `choice-lane-core.fk` |
| `cut(alts,args)` | commit to the head, whatever it answers | `choice-lane-core.fk` |
| `store` `undo` | keep this value unless the answer is absent | `choice-lane-core.fk` |
| `oac-timeout-walk` `oac-hold` | did the budget or the alternatives run out first? | `choice-lane-core.fk` |
| `repeat` `again` `retry` | run this again on the queue | `native-events-darwin.bml` |
| `defer` `Disposable` | release on every exit | core, `bml/disposable.bml` |

**`??` and `choice` walk past absence.** A gate maps a blank reading to
`nothing()`, and the walk moves on:

```bml
def arrived(v) = nothing?(v) || str_len(v) == 0 ? nothing() : v;
def tongue(lang, live) = choice { arrived(lang); arrived(live); "no-tongue" };
def verdict(want) = choice { rung(0, want); rung(1, want); rung(2, want) };
def first-from(i, count, once) = i >= count ? nothing() : choice { once(i); first-from(i + 1, count, once) };
def label(name) = name ?? "unnamed";
def wait-quiet(pid) = (pid ?? 0) <= 0 ? nothing() : pid * 2;
def wait-wire(pid) = wait-quiet(pid) ?? -1;
```

`tongue("", "en")` is `en`, `tongue("", "")` is `no-tongue`. `choice` keeps a
zero: `choice { nothing(); 0; 9 }` is 0. `verdict` reads a sentinel ladder
(`rung` answers 1 healed, -1 refused, absent otherwise) and the caller says
`unresolved` for an absent walk. `wait-wire` keeps the `-1` sentinel at the one
edge that needs it; `wait-quiet` stays absent inside. `trim(text ?? "")`
is the quiet default for a line that may not have arrived.
Spots: `bml/retry.bml` (`retry-first-from`), `form-glass-live.bml`
(`fgl-accepted-row-in`), `form-cli-microthought.bml` (the `selected` walk over a
list of offers), `model-registry.bml` (`mr-find`, `mr-for-role`),
`bml/gpu-lower.bml` (`gl-find-self-kids`), `bml/host-walk.bml` (`hw-line`).
The function form `choice(offers, args)` takes a list of one-argument lambdas;
`form-cli-microthought.bml` is its one caller.

**`try` reads a stop as a value.** Put one strict call under it:

```bml
def typed(x) = try str_len(x) else "stopped";
def caught(x) = try str_len(x) catch "caught";
```

`typed(5)` is `stopped`, `typed("abc")` is 3. Only a genuine stop selects the
handler: `x / 0` is the integer 0 and `len(5)` is 0, so neither stops;
`str_len(5)` and `str_concat` over a non-string do. Spots:
`bml/bml-compact-rewrite.bml` (`bcr-reading`), `observe/bml-cursor-coverage-run.bml`
(`bcc-typed-or`).

**Backtrack walks with memory.** Each option receives `list(args, memory)`
(`oac-option-args`, `oac-option-memory`). An absent answer is skipped, a node
answer appends its payload to memory and the walk goes on, and any other answer
(one or zero) ends it. `oac-backtrack-ack` and `oac-backtrack-memory` read the
result:

```bml
def counts(offered) = oac-one(intern_trivial_int(40 + len(oac-option-memory(offered))));
let w = oac-backtrack-walk(list(decline, counts), 7, list());
```

With a `decline` that answers a node, the walk lands on 41 with one payload in
memory: the option read how many refusals came before it. A zero ends the walk
too. Spots: `bml/live.bml` (`live-option`: every declined option leaves one
finding, so the memory length is the index of the repair the next option
offers), `observe/native-turn-run.bml` (`nt-walk`: the second option receives
what the first saw), `bml/native-events-darwin.bml` (`nve-continue` starts
`nve-care-walk` from the memory a held continuation kept and stores the walk's
memory back), `control/session-recording-lifecycle.fk` (`oac-census`).

**`cut` commits to the head.** It offers only the first alternative and answers
that acknowledgement, whatever it is:

```bml
def ready(lanes, n) = lanes |> filter(l => l[0](n)) |> map(l => l[1]);
def pick(n) = n |> cut(ready(list(list(x => x > 9, big), list(x => x > 0, small)), n));
def routed(req) {
  let ack = req |> cut(routes());
  nothing?(ack) ? "unrouted" : node_value(oac-payload(ack));
}
def steady(alts, n) = (n |> cut(alts)) ?? "held";
```

`cut(list(declines, lands), 9)` is absent: the silent head is the answer and
`lands` is never offered. `pick` filters lanes by a cheap guard first, so the
head is the first lane that is ready (`pick(20)` reaches `big`, `pick(5)`
reaches `small`); `routed` lets the first route own the request and answers
`unrouted` when that route declines; `steady` gives a stopped head one value
and never a second lane.
`oac-cut-with-receipt` answers the acknowledgement and how many lanes it
pruned (`oac-cut-receipt-ack`, `oac-cut-receipt-pruned`); it counts them and
does not name them. The engine's `cut` inside `choose` is positional: a `fail`
before it falls to the next branch, after it fails the whole choose. The
fixture `tests/fixtures/bml-choose-cut.bml` pins both, and reads as
filter-then-`cut`. Spots: `control/choice-lane-core.fk` (`oac-cut`),
`control/tests/choice-lane-core-band.fk` (a silent head, a live head).

**`store` and `undo` decide by the answer.** `store(memory)` and `restore(saved)`
are the identity, so a checkpoint is a value the caller holds:

```bml
let saved = store(list(1, 2));
def grown(watch, held) = undo(watch, held, cons(watch, held));
```

`undo(ack, checkpoint, memory)` is the checkpoint when `ack` is absent and
`memory` otherwise, so `grown` adds a watch only when one arrived. A present
zero keeps `memory`. `undo` evaluates all three arguments first; give it
values, never a call that must not run on the absent branch. A host effect
comes back only when the caller writes the saved value back. Spots:
`bml/bml-demand-jit-events.bml` (`bdje-watch-paths`), `observe/native-turn-run.bml`
(`nt-texts` holds each file's text before a lane, `nt-keep` returns it after),
`bml/fkwu-binding-care.bml` (`fkbc-restore` writes the original bytes back).

**Timeouts count attempts and alternatives.** `oac-timeout-walk(alts, args,
budget)` answers `list(ack, budget-left, alts-left)`; `oac-timed-out?` is true
only when the answer is absent and alternatives remain, which tells a spent
budget from a walk where every alternative declined. `oac-hold(recipe, args,
ripen, patience)` re-offers one recipe with ripened arguments and reads
exhaustion as absent:

```bml
let t = oac-timeout-walk(list(declines, declines, lands), 9, 2);
let held = oac-hold(once, list(0, 0), ripen, 5);
```

With a budget of 2 the walk ends absent with one alternative left
(`timed-out`); with a budget of 5 it lands and leaves 3. Here `oac-hold` lands
once the ripened argument reaches 3 at patience 5 and is absent at patience 2.
A clock-shaped wait
uses `wait-until` and `retry-first` (see Waiting and retrying). Spots:
`bml/router-proof-cases.bml` (`rpc-wait-listener`), `bml/router-proof-io.bml`
(wait for exit, then force the stop), `bml/form-cli-heal-native-io.bml`
(`retry-first` over a new directory name).

**`repeat`, `again` and `retry` ride the event queue.** A pass returns `again(next)`
to yield to peer work, a value to finish, and `retry(offered, next)` to propose
a changed checkpoint from local care:

```bml
def nrw-trace(context, event) {
  let at = nrw-cursor(event);
  at < record_get(context, "until") ? again(at + 1) : 0;
}
def nrw-repair(offered) = retry(offered, nrw-cursor(oac-option-args(offered)[2]) + 1);
```

`observe/native-repeat-witness.bml` runs 25 rows through actual queued work
(each row prints `[name, actual, expected]`): two repeats interleave as
`A0B0A1B1A2`, an unchanged checkpoint rests in care, a cancelled retry never
admits its cursor, twenty thousand passes run without recursive reentry, and a
resource event resumes the original context. Spots:
`observe/native-repeat-witness.bml` (`nrw-trace`, `nrw-count`, `nrw-unchanged`),
`form-stdlib/tests/bml-repeat-runtime.bml`.

**`defer` and `Disposable` release on every exit.** Acquire, defer the release
by the class's name for it, and use the handle:

```bml
def with-slot(h) {
  defer Slot_dispose(h);
  print_str("using " ++ int_to_str(h));
  h * 2;
}
```

`with-slot(21)` prints `using 21`, then `released 21`, and answers 42. Two
defers run last-registered first. A stop inside the block still runs them: a
`defer print_str("cleanup ran")` above `str_len(5)` prints, and the block reads
as absent. Spots: `native-affine-metal.bml` (`NativeAffineBuffer_dispose`),
`native-session-learning.bml` (`nlg-close`, `ohs-close`, `nsl-cache-drop`),
`learn/bml-execution-observation.bml` (`beo-generate-owned`).

`stop` and `fail` are the bare spelling of `nothing()`; production code writes
`nothing()`. `backtrack` and `timeout` have no surface word, and their owners
stay `oac-backtrack-walk` and `oac-timeout-walk`. The two-argument
`oac-backtrack` reads only the acknowledgement of a walk that starts with empty
memory. A `Disposable` class is a sibling class: the cursor refuses a class
nested inside another.

## Bands pin with `; Verdict`

A band pins the number `./fkwu` must answer with `; Verdict <n>` in its head
comment. `; Expected: <n>` is prose: `validate.sh` reads only the first
`Verdict`. `observe/band-pin-run.bml` reads a band's own direct answer and
writes the pin (`check` reads, `apply` writes). A band ends with its answer as
the final value; `(print x)` inside a `do` prints and then answers 0. The
direct `./fkwu` door forgives a stray `)`; `validate.sh` refuses it with
`[unbalanced-source]`, so count closers after any ending edit.

A band's answer must not depend on where the checkout lives. Roster names in
shared memory are bounded at 119 bytes; a longer `root|publisher` name folds
its root to `ggf-name(root)` (`fgtm-space`), so a long `TMPDIR` under
`validate.sh` reads the same as `/tmp`.

## Local lift

`form/form-stdlib/form-lift.bml` supplies lifting guidance to the public
native `code` controller whenever the supplied sources contain executable
BML. The controller retains the original sources, writable paths, checks and
checkpoint. Its tools read and edit resident document values in-process;
filesystem loading/publication belongs to the caller, and live telemetry uses
shared memory. No model-selected shell command, network tool or provider is
admitted by this path. The host capability catalog remains a separate inventory.

The guidance names the compact surface (`++`, `xs[i]`, `match`, `let [..]`,
`choice { }`, `defer`, `Disposable<H>`, `wait-until`, `retry-first`) so the
native `code` controller and the local model write it. Use lexical lambdas and
the existing sequence functions before introducing another walk. `table.bml` provides generic row operations and compatibility
adapters for existing carried-argument callers. `tb-first(rows,predicate)`
returns `nothing()` on a miss while preserving zero and empty matching values.
Named accessors own tuple layouts; domain constants belong to their semantic
owner. Classes group APIs. Templates describe parameters and members; instance
accessors, interface checking and specialization are not implied.

Run a defined lift through `code_entry: "direct"` with the actual callers and
helper contracts supplied. Inspect the candidate, execute its behavior and read
its frames before publishing. Compact source, preserved semantics and measured
runtime cost are separate observations. [Native coding](form-native-coding.md)
describes the request and checkpoint interface.
