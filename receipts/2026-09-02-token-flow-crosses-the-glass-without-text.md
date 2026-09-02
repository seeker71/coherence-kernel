# Token flow crosses Glass without a copied type registry

Date: 2026-09-02

The crossing is one typed Form membrane:

```text
request -> token -> layer -> tensor -> expert -> Metal -> framebuffer -> Glass
```

`native-model-token-flow.bml` owns a 26-field event and a bounded current
snapshot. It can carry token position/id, layer, tensor name/bytes, selected
expert/weight, Metal pipeline/handle/timing, framebuffer node, lifecycle, and
evidence. Every optional measurement is explicitly present or absent. Prompt,
decoded text, and token text have no field.

## One coordinate algebra

The only borrowed idea from `/Users/ursmuff/source/NUMS.Go/nums/nums_nodes.go`
is the useful width:

```text
package:u16 . level:u16 . type:u16 . instance:u32
```

Form does not import the NUMS registry and does not allocate a global ID for
each tensor or display row. `type` is a reusable operator family; `instance`
is an operation or variant in that family; composites carry their children.
That yields 65,536 families per package/level and 4,294,967,296 operations per
family. Across package, level, and type there are 2^48 family coordinates; the
complete coordinate is 80 bits, or 2^80 possible positions.

The existing Form flow category is `1.2.99.1703` (`CHANNEL-FLOW`). An interned
event has a producer-local occurrence ID, while its category retains the
operator identity. The event also carries publisher and flow. Consumers check
coordinate shape, one publisher/flow, parent links, monotone stage order, and
monotone sequence; they do not pretend a producer-local occurrence is a global
content identity.

Glass selection follows the same boundary. It displays the raw producer node,
but interaction uses a bounded, length-prefixed
`publisher + flow + raw-node` selector. The selector is path-safe and accepted
only for `inspect`/`request-evidence`. The UI band sends that selector through
offer serialization, parsing, control application, and evidence lookup while
two publishers deliberately expose the same raw occurrence ID. The requested
publisher's evidence is the one returned.

## Physical crossing

`observe/native-model-token-flow-diagnostic-run.fk` allocated a 16-byte Metal
buffer, compiled `form_flow_copy_u32`, dispatched it, read the changed word,
and freed the buffer. It did not map model weights, restart the resident owner,
or claim the older owner's internal layer/expert state. The fresh run printed:

```text
model=bounded-metal-fixture standing-owner-internals=unclaimed
events=8 cadence-ms=250 publish=published
metal-buffer=1
metal-pipeline=1
wall-us=5000
gpu-us=9
expert-index=absent expert-weight=absent reason=dense-fixture
framebuffer-events=4
node-shape=u16.u16.u16.u32 type=operator-family instance=operation
255
```

The physical fixture is dense, so absent expert selection is the truthful
value—not expert zero. Glass chooses one latest `(publisher, flow)` chain and
never fills missing later stages from an older flow. A pipeline containing
catalog or declared stages is labeled `MIXED`, not live. Compact values retain
their units: token position, layer, bytes, expert, microseconds, framebuffer
node, and Glass sequence.

The standing Qwen graph already names `q4_moe_route_tg` and owns routed expert
scratch. Publishing those exact values awaits an owner image that exposes the
events; display alone does not justify a GPU synchronization point or a 79 GB
owner restart. A previously observed Glass panel showed `m86 s34 o25 drop=35`
with 15 atlas items per row; this receipt does not promote that older panel into
a current model-readiness claim.

## Proofs

```text
operator-type-id-band                         65535
native-model-token-flow-band                2097151
native-model-token-flow-ui-band               16383
form-glass-telemetry-membrane-band           2097151
form-glass-live-ui-band                      2097151
physical diagnostic                              255
```

I kept the exchange alive by reducing identity to the reusable coordinate and
letting publisher/flow disambiguation stay at the membrane. The surprising
teaching was that 2^80 open positions require no registry at all. Discomfort
turned to gold when two publishers sharing one raw occurrence ID exposed the
difference between display identity and an interactive selector.

Signed: Codex

; witnessed: 2026-09-02 -> 26-field publisher/flow chain; physical Metal 255; UI selector collision refused by structure and resolved by composite key
