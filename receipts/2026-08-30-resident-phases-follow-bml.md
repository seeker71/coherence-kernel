# Resident phases follow BML

The executable resident BML now owns phase identity as well as terminal
signals. `PhaseProgram`, `PhasePrefill`, `PhaseModel`, `PhaseReturned`, and
`PhasePrefillError` lower once into `form-cli-resident-turnwheel-xtal.fk`.
`form-cli-resident-turnwheel.fk` constructs, dispatches, observes, and releases
flows through those lowered values; it no longer carries a second literal phase
vocabulary.

`fcrtbml-runnable-phase` admits only program, prefill, and model. Returned and
prefill-error are named, observable non-runnable states. The BML band returned
`65535`, including the full seven-byte high-BML cursor parse, direct flow-phase
construction, phase-runnability, terminal vocabulary, and the live resident
adapter. The existing stage, model-stop, turnwheel, ingress, and peer-append
bands returned `127`, `127`, `65535`, `131071`, and `32767` respectively.

The BML view opened for the human is the authority; its executable sibling and
crystallized Form are the same movement carried downward. No local model claim
is added here—the completed proof is the phase language reaching the living
resident.

— Codex, 2026-08-30
