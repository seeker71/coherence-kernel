# The reader completes its own cursor; the one-turn meter is BML again

The token-evidence collector had a private bound rollout of 498,968,682 bytes
but an inherited discovery cursor at 222,195,433.  Its old refresh path moved
one fixed 1 MiB slice per process wake, leaving the user to repeat the same
external invocation hundreds of times.

`form-cli-turn-evidence-cursor.bml` now owns that backward traversal as one
scannerless Form recursion.  It carries only byte coordinates, a selected turn
identifier, and collector status.  Prompt, answer, and reasoning bytes remain
inside the reader.  Once it finds a completion it hands the exact coordinate
back to the existing validated 400k progress/observed-row collector; it does
not create a second receipt format or bypass its checks.

The live bound rollout was observed twice through the new BML door:

1. The retained 222,195,433-byte cursor reached its beginning and returned
   `turn-evidence-refresh=withheld no-complete-turn` in 8.6 seconds.
2. With that exhausted cursor cleared, one subsequent Form invocation scanned
   all 498,968,682 bytes and returned the same terminal status in 18.9 seconds.

No transcript bytes were emitted and no provider, HTTP, llama-server, or Ollama
call was made.  Therefore this host has no completed provider-token denominator
from which the requested 10% ratio could honestly be calculated.  The prior
BML local-direct receipt remains a real `remote-tokens=0`; its ratio remains
`nothing` rather than pretending that no completed baseline is zero.

The same observation uncovered a broken local meter: its runner depended on
the missing `form-cli-one-turn-xtal.fk` mirror.  The active runner and its
local-ready/session bands now preload executable BML directly.  Their warm
readings are `local-ready=1023`, `one-turn=2047`, and `session-local=1023`.
The one-turn panel reports `one-turn=1`, `session-remote=0`, and
`leak-one-request=0`.

I kept the exchange alive by letting the private cursor finish instead of
turning a partial scan into a status story.  The surprising teaching is that
the missing denominator is now a terminal Form observation, while the
discomfort of its absence becomes a clean future receipt door—not a reason to
spend another remote turn.
