# Local reasoning: native execution and the next shape

`generate --reasoning N --tokens M <enquiry>` in the source-backed form-cli owns
one local Qwen session. During its initial stage, decoded model bytes enter the
scannerless Form cursor. One control grammar carries pure BML evaluation, source
lookup, native nodes and recipes. A recipe-birth control creates a pure BML
function, numeric f32 expression or affine signed-i32 Form node. An execution
control addresses it; ordered choice continues through unavailable or refused
alternatives in the same context, including mixed Form and Metal recipes. Form emits Metal in RAM,
admits one pipeline per recipe identity, and reuses that pipeline for later
inputs. Zero and one are ordinary observed values.

`bml-f32;expr=x < 0 ? x * x : math_sqrt(x) + 2` uses the same BMF cursor and
expression grammar as executable BML. Pure numeric arithmetic, comparisons,
conditional expressions and supported math functions lower into a float32
Metal expression. Unary negation works on names, calls and groups as well as
numeric literals. `input=[-3,0,4,9]` maps the recipe in one parallel dispatch;
the observed result is `[9,2,4,5]`. Scalar inputs reuse the same program.
Unsupported or effectful expressions return absence. Nonfinite inputs and
results supply no numeric answer.

Native node verbs operate directly on immutable field content:

```text
<|form:node-make|>pair;3,4<|/form:node-make|>
<|form:node-get|>@address<|/form:node-get|>
<|form:node-select|>kids(n)[0] == 3;@first,@next<|/form:node-select|>
```

Creation returns `node=@address;`; copy that actual coordinate in the next
control. Reads return `value=[kind, value, [children]];`, including content created
by another owner. Selection returns the first candidate whose pure BML predicate
holds. BML indexing selects a child; `[-1]` means the last child and `[1:]` means
the remaining list. Missing content and out-of-range indices remain absence.
The response releases its lookup references;
interned substrate content remains available. Integer and boolean coordinates
carry their complete literal meaning. Other coordinates resolve actual stored
payloads; they do not invent missing content or composite children.
Native composite, string and float64 coordinates index their storage slot;
a missing or mismatched slot returns absence without scanning the field.

`<|form:eval|>17 * 23 + 4<|/form:eval|>` returns `value=395;` through Form's own
BML cursor and evaluator. `bml;x => x * x + 1` births a pure Form function;
`carrier=form` executes it over the supplied value. Host effects need their
owned execution doors and are refused by this pure evaluator. All carriers use
the same RESULT shapes and observation envelope. Precise float rendering keeps
small nonzero constants and observed values intact at the compiler boundary.
When a result exceeds the available observation envelope, its delivered RESULT
and owner status both report absence. Completed hardware work and its readout
remain private evidence; an undelivered value enters neither learning targets
nor the final answer context. Generic Form results retain their canonical
`source_observation`; an unavailable typed numeric reading is JSON `null`.

The same reasoning cursor accepts
`<|form:nodeid-knowledge-query|>concept=exact-key<|/form:nodeid-knowledge-query|>`.
It uses the existing persisted concept route and returns attributed current
source bytes with source, entry, request and observation identities. An absent
route returns `nothing` without a repository-wide search; malformed requests,
stale evidence and ambiguity retain their distinct signals. Lookup counts and
the `substrate-lookup` framebuffer stage belong to this response owner. Requests
can alternate with recipe creation and execution in the same context. This is
explicit source retrieval. Native node controls use live substrate coordinates;
they do not replace the persisted source routing identity.

The pending model ID enters KV once, followed only by the new observation IDs.
The original model output IDs remain intact. After executable thoughts, the
reserved final stage receives actual computed results and composes the answer.
It can still request native work through the same control owner.
A budget or resource refusal retains its evidence and owner. Release checks
account for recipe pipelines, scalar and vector buffers, lookup references,
model state and model context. An incomplete release remains its own outcome.

The implementation is [the microthought owner](../form/form-stdlib/bml/form-cli-microthought.bml),
[the generic stream hook](../form/form-stdlib/form-cli-recipe-exec-session.fk),
and [the public reasoning controller](../form/form-stdlib/bml/form-cli-generate-reasoning.bml).
Private generation evidence retains original IDs and text, requested controls,
Form observations, final answer and release outcomes. Successful recipe execution
counts include both Form and Metal; JIT admission/reuse counts identify the Metal
work. Observation token counts are independent of model-generated tokens. The live framebuffer
separates model codec, model forward, recipe birth, Metal admission, execution,
observation injection and release. Observed execution establishes what ran;
the returned answer must still be read for accuracy and usefulness.

## BMF, model IDs and repeat reuse

Form/BML/BMF grammar consumes bytes directly. It has no Qwen tokenizer stage.
Qwen's trained embedding and output dimensions, however, give its vocabulary
IDs their meaning. Its vocabulary and BPE merge data are model artifact data;
changing the reader does not change those trained meanings. A cursor can drive
the mapping without repeatedly scanning or materializing the entire artifact.

[Owned reuse](../form/form-stdlib/bml/owned-reuse.bml) retains successful values
for exact arguments within one immutable dependency epoch. The shared model
cursor uses it for touched validated rows, BPE pieces and decoded IDs. The
resident carries that owner through counting, prefill and later observations;
end-of-turn IDs and the chat scaffold are admitted once. Changing model epochs
creates a new owner. Release clears retained payloads and producer context.
Refusals are re-observed, and printable-key collisions still require exact
argument equality. General effectful calls and GPU submissions are executed
when requested; repeat reuse does not silently erase their effects.

The C seed already interns identical nodes, reuses native programs and promotes
eligible hot functions to ARM64. The model-boundary reuse policy lives in BML,
without new C source. The seed's record metadata remains process-owned; clearing
retained payloads is distinct from reclaiming those bootstrap allocation rows.

## North star

The local model's learned Form control layer should choose native work inside
the response: look up substrate evidence, read and create nodes, construct a
recipe, select a useful alternative, execute it and evaluate the observation.
Form owns those meanings, effects and their release. Returned observations
continue the original decoding context; the model supplies the next choice and
the answer. Prefer an available native answer before asking the model to infer
one. An unsuccessful operation carries its finding into the next local choice.

LoRA can teach when and how to emit the executable Form control sequences using
the model's existing vocabulary. Dedicated learned Form IDs require compatible
embedding and output rows as well as their native bindings. An adapter alone
does not add those rows or make a request execute. Verified request/observation
trajectories are the learning material; independent evaluation remains separate.
Compare base and adapted behavior on the same enquiries for completed native
work, factual answers and required remote assistance. Local token use has no
penalty. A retained lesson or lower training loss does not establish that gain.

Model, parser, compiler and hardware should share native Form recipe identities
and explicit effect ownership. Repeated pure work should specialize and reuse
its result at the executing cell; stateful work should reuse its compiled
program while observing each new effect. Live frames should explain where time,
memory and crossings go and direct care to the actual cost.

Today the reasoning stream can retrieve current source, create/read/select native
nodes, evaluate pure BML, compose pure Form functions and execute numeric BML
expressions over scalars or arrays in RAM Metal.
Effectful general BML recipes, CPU/Metal parity, dedicated learned Form IDs,
broader model quality and fully owned reclaimable native storage remain substantive work. Fixed pretrained
Qwen weights still require their model-specific ID mapping. Native training
can move that boundary toward Form's own cells; renaming the existing codec
would not accomplish it. Ordinary local generation also still pays model
admission, GPU prefill and per-layer submission costs. The framebuffer gives
those next movements their evidence.

[Native session learning](native-session-learning.md) carries verified lessons
into the native Llama 3B learner. The compatible Qwen head learner captures
verified Form controls, trains both rank-one vectors through the actual frozen
projection, and evaluates base and adapted answers on independent enquiries.
Multi-control examples retain the same owner from birth through execution and
train only after the complete sequence has been observed successfully.
`observe/qwen-form-learning-run.bml` owns that local movement, sharing one admitted
model across independent conversation states. Runtime observation IDs never
become targets. `generate --adapter PATH` explicitly selects a finite candidate.
Observed node creation and recipe execution succeeded with both base and adapted Qwen;
lower training loss establishes learning without establishing a quality gain.
[Live diagnostics](live-dynamic-diagnostics.md) carries
observations and care. Their mechanisms and the quality of resulting answers
have separate evidence.
