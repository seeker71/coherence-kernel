# 2026-08-26 — session world uses the live carrier

The prior daily session-world runner was not a live-world witness. It selected
fixed July sessions, demanded fixed episode counts, and compared only that
historical carrier. The runner now selects recent Codex sessions whose declared
working directory belongs to Coherence Kernel or Coherence Network, derives a
chronological 80/20 split from the observed action metadata, and reports schema
`native-model-session-world-report-v3`.

The carrier keeps the existing privacy boundary: prompts, responses, commands,
and tool results do not enter durable episodes. Each row holds timestamp,
salted session digest, and tool-action names only; Form retains fixed-shape
counts rather than the corpus.

The first observed v3 carrier had 445 sessions, 109,935 episodes, and a
87,948/21,987 train/heldout split. Its carrier, model, and evaluation-contract
identities are reported by the private daily artifact, not copied here. Its
first score has no prior identity-aligned v3 sample, so the delta is `-1` and
no trend is claimed. The candidate remains challenger-only: this diagnostic
does not grant promotion or authority.

Verification:

```text
./fkwu form/form-stdlib/tests/native-model-session-world-band.fk  -> 4095
./fkwu form/form-stdlib/tests/native-model-control-plane-band.fk  -> 65535
form/scripts/native_model_session_world.sh                         -> world_model_valid=1
```

Signed, Codex — the frozen floor was named and removed; the changing carrier
now carries its own identity.

; witnessed: 2026-08-26 -> v3 live project carrier valid, 4095/65535
