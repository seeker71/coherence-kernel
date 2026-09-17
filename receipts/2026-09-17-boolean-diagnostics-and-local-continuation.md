# Boolean diagnostics and a retained local coding continuation

Signed: Codex. Native Form/BML on the C-bootstrap and existing Metal carrier.

## Compiler feedback that names the fault

The source compiler now rejects unqualified `and`/`or` calls whose argument
count differs from two at the BML call boundary. Its diagnostic includes the
actual count and call expression. This prevents extra arguments from reaching
the source runner as misleading unbound names and stray delimiters. The
compiler does not rewrite those calls; repair remains an explicit source edit.

The repair is eight lines in the existing Form source compiler. Loading a new
BML implementation into the compiler that lowers BML would introduce a
bootstrap cycle. There is no C change or additional runtime dependency.

The new native integration band compiles eight invalid calls (both operators,
zero/one/three/five arguments) and checks nonzero exit plus the precise
diagnostic. Five actual executions preserve valid binary results,
short-circuiting and an operator-like string. All thirteen cases pass; the
band returns **1**, exit 0. It uses isolated temporary files and native fkwu
children. The existing native text boundary band passes **1023**, compiler
health **57343**, and boolean repair preservation/refusal band **1**.
Fresh preflight is clean for each.

The new band's first preflight exposed missing `core.fk` definitions, then
the mistaken name `now_ms`. Adding the existing prelude and using
`now_unix_ms` repaired those causes. Neither failed preflight was accepted.

## What the local continuation actually did

The retained previous candidate contained duplicated function-closing code
after an actual Qwen edit. A fresh native compiler check confirmed that fault
before admission. The continuation preserved the original goal, document
history, writable boundaries and unchanged 32-case behavior contract. Its
reply allowance increased from 384 to 1,536 tokens, context from 8,192 to
12,288 positions, with a cumulative controller limit of 20 from retained turn 8.
These are changed experimental conditions, not a same-budget improvement.

The first continuation enabled native reasoning for its initial repair reply.
It consumed **1,536 generated tokens in 461,758 ms**, correctly identified
the duplicated lines, repeatedly revisited that diagnosis and reached the
ceiling before a final-channel response. No tool action occurred, no source
changed, and the workflow remained at attention. The private unfinished
output is retained; it is not treated as an actionable response or learning
target. The model released successfully.

Private evidence: `.hearth/response-parity/bml-native-code-resume-v1` and
`.hearth/code-memory/replies/41904-1789622748487.txt`.

The ordinary continuation started from the same retained source and request,
with the same larger limits and no initial reasoning stage. Fresh check
diagnostics make the two bootstrap byte strings different; this is not an
identical-admission comparison. Native assessment confirms the starting
source bytes match and the executed behavior-test bodies are unchanged.

It produced a complete syntax diagnosis, read the source and correctly
removed the duplicated function ending. Verification compiled successfully
and failed on reversed UTF-8 span order. It correctly described the missing
terminal reversal, but edited the recursion instead. The check failed again.
It repeated its diagnosis, read the source and reverted that last edit.
Finally it requested review and reached the controller limit at attention.
The final source matches the first behavior-failing snapshot byte for byte;
the compiler recheck passes. No candidate replaces the working public utility.

| Observation during this admission | Initial reasoning | Ordinary |
| --- | ---: | ---: |
| Elapsed ms | 461,758 | 1,013,865 |
| Generated token IDs | 1,536 | 1,245 |
| Injected observation token IDs | 0 | 5,470 |
| Final controller turn counter | 9 | 20 |
| Additional model tool calls | 0 | 7 |
| Additional behavior checks | 0 | 2 |
| Full behavior passes | 0 | 0 |
| Final compiler exit | 2 | 0 |
| Model release | complete | complete |
| Provider subprocesses | 0 | 0 |

Both admissions also made one native pre-admission compiler check, separate
from the model counters. Neither used an adapter or automatic training.
Private ordinary evidence lives under `bml-native-code-resume-v2`; native
assessment is `bml-native-resume-comparison.json`, all under
`.hearth/response-parity/`. The ordinary branch records a correlated outbound
observation, inbound branch selection, actual native admission and subsequent
check/failure/release counters through the framebuffer. Prompt and reply bytes
stay outside that channel.

The final ordinary source digest is
`d3f5b99041813e49e8de041781bcad256f37705e0e9d331b902aa80200bf91ee`,
the same as its first compiled, behavior-failing snapshot. The intervening
failed edit has a different digest. Repeated failure here is not evidence of
stale verification. More allowance and source identity let the structural
repair finish but did not establish a correct semantic repair or parity.

The assessment helper initially omitted the semicolon after an expression
definition; its fresh diagnostic named an unconsumed body suffix. The
terminator was restored and fresh preflight and assessment passed, exit 0.

## Instruments

Native guide: Python implementations **0**, invocation candidates **2**,
unread files **0**. Counsel: orphans **0**; **11/12** lanes unobserved because
there is no standing hearth. Those missing readings do not establish health.
Glass's first frame arrived in **28 ms**. Its owned viewer was interrupted
after reading; no Glass process remained. Drift gates pass **8191**.
Share remains declared/unmeasured, with the percentage withheld while the
completed-turn evidence cursor is still locating its start coordinate.

The recorded parent-output meter reads **2,301,094 cumulative tokens** and
the goal meter **11,266,519 cumulative tokens**. They measure different
boundaries and are not isolated local-run costs. Rented coordination is still
expensive. These observations do not meet the overall efficiency goal.

Verified compiler procedure was retained through native session learning as
event `2026-09-17-boolean-arity-diagnostics-v1`, session
`native-arrival-bootstrap`; its worker launched after Qwen released. This is
procedural retention for the Llama learner, not a Qwen update. Evaluated model
answers and the utility's desired repair were excluded from the teaching.

The useful surprise is how directly native diagnostics can remove ambiguity
before another model call. The difficult observation is a correct diagnosis
that still never reaches an action. Keeping the partial output and unchanged
candidate visible makes that boundary available for repair.
The next useful attempt is native assistance that can test the semantic
effect of a proposed edit. Existing bounded expression search is a starting
point to inspect, not an established repair for this function's block body.
