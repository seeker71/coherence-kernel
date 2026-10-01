# Local reasoning: native execution and the next shape

`generate --reasoning N --tokens M <enquiry>` in the source-backed form-cli owns
one local Qwen session. During its initial stage, decoded model bytes enter the
scannerless Form cursor. A recipe-birth control creates an affine signed-i32
Form node; an execution control addresses it; an ordered choice can continue
from an unavailable address to an available recipe. Form emits Metal in RAM,
admits one pipeline per recipe identity, and reuses that pipeline for later
inputs. Zero and one are ordinary observed values.

The same reasoning cursor accepts
`<|form:nodeid-knowledge-query|>concept=exact-key<|/form:nodeid-knowledge-query|>`.
It uses the existing persisted concept route and returns attributed current
source bytes with source, entry, request and observation identities. An absent
route returns `nothing` without a repository-wide search; malformed requests,
stale evidence and ambiguity retain their distinct signals. Lookup counts and
the `substrate-lookup` framebuffer stage belong to this response owner. Requests
can alternate with recipe creation and execution in the same context. This is
explicit source retrieval; it does not expose arbitrary graph mutation or direct
persisted NodeID routing.

The pending model ID enters KV once, followed only by the new observation IDs.
The original model output IDs remain intact. After executable thoughts, the
reserved final stage receives actual computed results and composes the answer.
A budget or resource refusal retains its evidence and owner. Release checks
account for the recipe pipeline, scalar buffer, model state and model context.

The implementation is [the microthought owner](../form/form-stdlib/bml/form-cli-microthought.bml),
[the generic stream hook](../form/form-stdlib/form-cli-recipe-exec-session.fk),
and [the public reasoning controller](../form/form-stdlib/bml/form-cli-generate-reasoning.bml).
Private generation evidence retains original IDs and text, requested controls,
Form observations, final answer and release outcomes. The live framebuffer
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

Today the executable recipe grammar is affine signed-i32, with explicit current
source lookup in its reasoning stream. General BML recipes, arbitrary node
reading and creation, CPU/Metal parity, learned Form control IDs, broader model
quality and fully owned reclaimable native storage remain substantive work. Fixed pretrained
Qwen weights still require their model-specific ID mapping. Native training
can move that boundary toward Form's own cells; renaming the existing codec
would not accomplish it. Ordinary local generation also still pays model
admission, GPU prefill and per-layer submission costs. The framebuffer gives
those next movements their evidence.

[Native session learning](native-session-learning.md) carries verified lessons
into the native Llama 3B learner. The Qwen response owner can take an explicitly
admitted compatible adapter; a serving Qwen adapter that improves these Form
control choices has not yet been demonstrated. [Live diagnostics](live-dynamic-diagnostics.md) carries
observations and care. Their mechanisms and the quality of resulting answers
have separate evidence.
