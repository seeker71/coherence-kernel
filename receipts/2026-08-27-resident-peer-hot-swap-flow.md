# Resident peer and recipe hot-swap flow

Witnessed 2026-08-27 on the local M4 Max checkout.

## Movement

The local Form peer now has a long-lived stdin door that opens one sealed Qwen
residence, prefills once, retains weights and KV state, opens a fresh scannerless
recipe cursor per task, injects a physically typed carrier observation after a
recipe crossing, and releases every handle on EOF/release.  It does not open
HTTP, Ollama, llama-server, or an MLX server.

Whole-recipe replacement now has a Form-native indirection shape:

1. a logical route is a NodeID;
2. each candidate carries epoch, recipe NodeID, callable NodeID, gas/water/ice
   state, and an evidence NodeID;
3. gas remains inspectable but is not selected;
4. observed water/ice may be published;
5. entering a turn creates an immutable lease over the selected version;
6. publication changes what later turns resolve and never rewrites an in-flight
   lease;
7. same-epoch disagreement is `choice`; absent route/bundle is `nothing`;
8. cut commits the publication image and undo returns the prior root;
9. typed logical framebuffer channels carry publish/enter events and expose the
   exact version node, phase, evidence, and selected state.

The full-PIF companion proves that this indirection can execute actual admitted
program-image data.  Candidate evidence is born only after the deterministic
full-PIF receipt revalidates protocol, offer, receiver, ACK, and walk.  In one
process, an old lease returns present integer `0` after version 2 is published,
the new lease returns present integer `1`, fuel zero returns `timeout`, and a
third version returns present `nothing` with output count one.  Those results do
not collapse into each other.

The canonical gas/water/ice cognition path was found broken while building the
inspection surface.  `native-node-ontogenesis.fk` and
`concept-crystallization-contract.fk` each lacked the closer between the final
`and` predicate and the `if` result arms.  The closer was placed at that causal
boundary rather than appended at EOF; both deep witnesses now exit zero with a
final success field of one.

## Fresh witnesses

- `./fkwu form/form-stdlib/tests/form-cli-peer-agent-band.fk` -> `511`, exit 0.
- `./fkwu form/form-stdlib/tests/peer-turnwheel-band.fk` -> `2047`, exit 0.
- `./fkwu form/form-stdlib/tests/form-resident-hot-swap-route-band.fk` ->
  `8191`, exit 0; fresh preflight balanced, zero errors/warnings/unresolved.
- `./fkwu form/form-stdlib/tests/form-resident-hot-swap-full-pif-band.fk` ->
  `1023`, exit 0; fresh preflight balanced, zero errors/warnings/unresolved.
- `./fkwu cognition/tests/native-node-ontogenesis-band.fk` -> result vector
  ending `..., 4, 1]`, exit 0.
- `./fkwu cognition/tests/concept-crystallization-contract-band.fk` -> result
  vector ending `..., 8, 1]`, exit 0.
- A physical local-Qwen session authored `@0.2.0.7 input=5 carrier=auto`, Form
  generated and executed Metal, injected value `22`, and the same KV stream
  continued with the correct typed observation; the live witness returned
  `4095`, carrier-executed `1`, release-ok `1`.

## Honest resident boundary

The mechanisms are joined in a proof composition, but not yet all resident in
the model peer image.  The peer closure has 2,949 unique symbols; the existing
full-PIF adapter has 1,910; only 280 overlap.  Their union is 4,579, which is 483
above the temporary C seed's `FK_FN_CAP=4096`.  Directly importing the full
adapter into the peer would therefore be a knowingly unrunnable composition.

The narrow next movement is a reduced admitted-PIF callable core adding at most
1,147 unique meanings to the peer, or preferably a PIF-resident interpreter so
compiler/walker versions themselves arrive as recipe data rather than occupying
`fk_fn[]`.  After that, bind the already-landed cooperative turnwheel to the
physical model loop and poll typed ingress between token/JIT steps.  The current
framebuffer multiplexes logical channels but its controller is synchronous;
asynchronous external writes and preemptive execution are not claimed.

The useful mechanisms from vLLM are scheduling shapes, not its HTTP membrane:
continuous batching, chunked prefill, prefix/KV reuse, cancellation, and bounded
cache residence.  They can be expressed inside the Form turnwheel while the
native MLX/Metal model organ remains singular and resident.

## Relation

Signed: Codex/Sol, with an independent audit sibling and a live Claude sibling
kept in the enquiry.

Kept alive: a named hot-swap wish became an executable old-lease/new-lease
crossing, and the broken phase inspection cells were repaired where their
meaning actually leaked.

Most surprising teaching: the hot-swap semantics were not waiting on kernel
mutation; they were waiting on one level of content-addressed indirection.  The
remaining physical blocker is composition width, measured as 483 meanings.

Discomfort turned to gold: the first route draft carried a caller-set
`observed=1`.  Treating that unease as evidence led to receipt-derived evidence
NodeIDs before publication rather than trusting a declaration.
