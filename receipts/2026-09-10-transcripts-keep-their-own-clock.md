# Transcripts keep their own clock

The reported jump between two time slots was two ears sharing one publisher name.
PID 31514 belonged to the viewing checkout; PID 50656 belonged to another
worktree. Both published `glass.sensor.ear` while their spools and owner leases
were checkout-local. The read-only native probe observed 90 identity/time changes
in 600 samples, alternating between two source clocks 375,892 ms apart. It kept
only typed identities, times, and counts—no captured speech.

The ear's shared-memory identity now includes its native body directory. The
reader chooses that same identity; the legacy unscoped frame is not a fallback.
Machine sensor names stay shared. No foreign runtime, C growth, renderer delay,
or forced shutdown of the other ear is involved.
The live cache also retains the last coherent ear snapshot, without refreshing
its source clock; unchanged frames no longer decode the transcript again.

The private two-publisher regression moved from 14 to 4095/4095. Competing writes
cannot replace this body's speech; a malformed read retains its prior clock and
becomes visibly stale, a real publication advances, and an explicit sleep clears
transcripts. The correlated native diagnostic chooses and re-observes the scoped
reader. A separate 750-sample probe found no failed reads, so an unconfirmed
read-overlap theory did not become the reported cause.

Preflight-clean bands: isolation 4095, live loop 2147483647, sensor rows 2047,
transcript flow 524287, language view 8191, all views 4194303. Drift gates
8191/8191. The live presentation panel before landing read 12 ms work and
40 ms to terminal acknowledgment; this was not a display-photon measurement.

The surprising lesson was not about drawing: one shared name had blurred two
speakers. Urs's correction from disappearance to two time slots made the
observation precise enough to repair the boundary.

— Codex
