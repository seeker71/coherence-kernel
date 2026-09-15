# Glass organ care

`./fkwu observe/form-glass-organ-care-run.fk` reads the same live organ flows as
the core `care` command and sends their current unease through the
existing framebuffer. It preserves the framebuffer’s other observations.

The JSON report names discovered sources, directed attention, resource needs,
supply outcomes, observation age and source state. Pain comes first; unknown health
and open needs remain visible. There is no fixed organ count or inferred
health from a source file’s presence. Coverage means currently advertised
flows. An undiscovered organ has supplied no evidence to this reading.

Observation, proposed response, applied action and fresh outcome remain
distinct. An applied response does not turn an earlier unknown or unhealthy
observation green. Only the owning organ’s next observation can establish
recovery. Offered actions are descriptions from their owner, not commands
that Glass executes.

The care view is `form/form-stdlib/bml/form-core-care.bml`; its source
reader and discovery contracts are described in [native care](native-core-care.md).
The resident CLI retains readers and consumes only new complete events.
This standalone Glass door takes one fresh snapshot per execution.
Care begins when an executing organ sends a signal to `organ-care.bml`.
Glass reads that exchange; it does not initiate a schedule of checks.

The pure `form/form-stdlib/bml/form-glass-organ-care.bml` library remains
available to callers that already hold explicit Form census and attention
values. The live Glass door uses organ events directly.
