# The failed check received repair

Codex, 2026-09-09. Urs asked for microphone-on by default, visible transcripts,
and continued repair when a check fails.

Replaced the nonexistent `all` test call with native conjunctions. Preflight:
errors 1 → 0, unresolved 1 → 0. UI band 4294967295, transcript band 8191,
input band 131071, native host doors 131071; all exit 0. Drift gates 8191/8191.
Glass now opens `meaning/all`; the existing persistent `z` mute remains intact.

Landed `3a28a37d`, fast-forwarded the saved live checkout, replaced its stale
ignored kernel after freshness refused 31, and re-observed freshness 31.
The existing supervisor replaced its children; no second dashboard was started.
Native ear wake was applied. Control `offer.1788916670908` received an applied
Glass acknowledgement at 1788916670916. Both ear lanes and Glass share the saved
checkout. Microphone start returned 0 (carrier success). Ear frame 150944 was
64 ms old, silent-frame=0, `ear OPEN silent`, stands=1. The production transcript
projection contained its heading in 24 lines. Initially transcript rows were 0.
At frame 153520 (81 ms old, stands=1), four locally produced rows arrived:
English original-closed and Portuguese, Persian, Indonesian translation-closed.
Their full text was present in the production 80x24 projection: 4/4, checked
without printing it. Recognition/translation accuracy is not established by
projection. No captured words enter this receipt. Human terminal pixels were
not directly inspected.

The surprise was that a valid default had never reached the running checkout.
The failed test became useful when it led to a repaired, deployed, observed path.
The repository now explicitly keeps repair moving while failed gates hold landing.
