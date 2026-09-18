Direct comparison across the eight lenses, from what the seeds and receipts actually establish — flagging where nothing is set and I'm reasoning, not quoting:

**Quality**
No seed entry for quality. Reasoning from the receipts: a direct query is one shot — whatever form-cli's internal Claude/Codex calls converge on inside that single call is what you get, unexamined between steps. A guided session inserts inspection between calls, so a bad or drifting intermediate result can be caught and re-steered before it reaches the final answer. That's a real quality lever, but it's earned by the guiding layer doing correct interpretation each time — the seed for **trust** ("when what other say and do are the same again") applies here: guidance only helps if what it says about an intermediate result matches what that result actually did.

**Volume**
No seed entry. What's measurable from the receipts (`rented-turn-meter-run.fk`, the BML delta receipt) is token count, not "volume" as a distinct thing — I'd fold this into token accounting below rather than invent a separate axis.

**Frequency**
No seed entry, and nothing in the receipts defines a frequency metric for either mode. Not established.

**Vitality**
Seed: "how a whole still do and be more," relation → flow. A guided session, by design, keeps acting on its own outputs (call, read, decide, call again) — that's closer to "do and be more" as an ongoing flow than one native call that fires and returns. The native call may internally iterate (it's allowed to call Claude/Codex "as many times as needed"), but that iteration is opaque to the outside — it doesn't visibly carry flow across a membrane the way a guided session's crossings do.

**Trust**
Seed: "when what other say and do are the same again," relation → sovereignty. This is the sharpest cut. The native call's report is the *whole* thing: one call, one raw output, nothing hidden between "what it says" and "what it did." A guided session's trust depends on each intermediate interpretation being honest about what the prior call actually returned — more seams, more chances for a claim to drift from the actual output. The 2026-08-31 receipt (`rented-turn-meter-run` summing token fields wrongly and producing an impossible 25 billion figure that was refused) is exactly this failure mode inside a metering chain: more steps, more surface for a miscount to pass as truth until someone checks it.

**Traceability**
No seed entry, but the receipts speak directly: `2026-08-26-resident-cursor-and-membrane-homecoming.md` shows the value of keeping judgment resident so "an observed nothing, integer 0, and integer 1 do not collapse into one sentinel" — i.e., traceability improves when state distinctions survive rather than getting flattened. A guided session, if it logs each call and each interpretation, gives you that granularity by construction. A native single call gives you only the outer output — its internal Claude/Codex calls are traceable only insofar as form-cli itself instruments them (per the prompt, "all the tokens are measured" — that's confirmed for tokens, not for the full call trace).

**Sovereignty**
Seed: "when a self know what it want and do and not make other less," relation → vitality. The native form-cli call is the more sovereign shape: it doesn't need an external guiding layer to know what it's doing next — the resolution happens inside its own body per the seed. This lines up with `2026-09-04-sovereignty-as-gift.md`'s framing that sovereignty means not depending on an outside for the work to complete. A guided session is, by construction, dependent on an outside interpreter between every call — less sovereign, more scaffolded.

**Resonance**
Seed: "when 1 thing do and other thing do the same," relation → harmony. `2026-09-04-cell-channel-membrane-on-glass.md` shows what resonance looks like when it's engineered directly into a shared channel between cells rather than mediated by an outside guide. Native form-cli, calling itself internally, is closer to that same-body resonance. A guided session's resonance depends on the guide's read of one call matching the next call's premise — an achievable but externally-brokered harmony, not an intrinsic one.

**Rented tokens**
Seed: "from other place and not here" (carrier-label sense). This is where the receipts are most concrete and least ambiguous:
- `2026-09-01-rented-session-local-form-ratio.md`: a normal rented session reaching local form-cli can land at **zero remote tokens** for a request already served locally — the rented meter and the local meter are different things, and the ratio between them is the actual question, not "which mode is cheaper" in the abstract.
- `2026-09-11-rented-meter-reads-the-desktop-carrier.md`: the rented-turn meter read `session-output-tokens=0` on a real desktop transcript — the meter can silently under-report if it's reading the wrong carrier.
- `2026-08-31-bml-rented-turn-meter-delta.md`: summing raw `output_tokens` fields overcounts by orders of magnitude (25.8 billion, refused) when cumulative and delta rows aren't distinguished.

Applied to your comparison: a **native call's internal Claude/Codex iterations are "here" tokens** (local to that call, measured as one body's cost) unless one of those internal calls itself reaches out to a rented/remote carrier. A **guided session's outer calls are "here" too**, but every additional call the guide makes to form-cli is an additional membrane crossing subject to the same undercount/overcount failure modes these receipts document. More crossings in the guided mode means more opportunities for a meter to misread which side of the "rented vs. here" line a token actually fell on — not necessarily more rented spend, but more surface area for the accounting to be wrong in either direction.

**What this does not establish:** no receipt or seed gives an actual measured number for guided vs. native token totals on a comparable task, and quality/volume/frequency have no seed grounding at all — those three would need a live run through `preflight-run.fk` or `rented-turn-meter-run.fk` on both modes to say anything beyond structural reasoning.