# Native requests keep finite ownership

A live Pulse observation found 48 processes, about 717% CPU and 9.87 GiB of
memory, with request handlers older than eleven hours. API services were healthy.
The native listener watched handler completion without an admission or lifetime
bound. The edge proxy's two-request limit could release a connection while its
native handler continued. Local health probes also bypass that edge limit.

The listener now accepts an explicit service capacity and monotonic lifetime.
Capacity includes incomplete request framing. Saturated admissions receive one
nonblocking 503 attempt and close; the listener returns to deadline processing.
The nearest deadline bounds the readiness wait. Expiry stops and reaps only an
owned direct child and closes its current watch descriptor. Ordinary completion
and legitimate HTTP write-half-close remain supported. Pulse adopts four
admitted requests and a 30-second lifetime through its file-backed configuration.

The recovery witness exposed a second defect on Darwin: after native syscall
fork, libc's cached process ID still named the parent. The native PID was
different, and an interrupted handler's lock remained attributed to the live
parent. Runtime identity reads on Darwin now use the uncached getpid syscall,
including lock, temporary-file and receipt identities. Linux identity behavior
is unchanged.

Observed:
- C-bootstrap rebuild on Darwin ARM64 succeeded.
- Native socket lifetime band: 127 on Darwin ARM64 and Linux x86-64.
- The band observes a partial request, a 503 at capacity, bounded expiry,
  an owned sleeping handler, its lock naming that child before interruption,
  exit 137 and reaping, then a successful half-closed request reclaiming the
  dead owner's lock. Completion and refusal/expiry counts agree.
- The Linux fixture used the serving C-bootstrap and separate private storage.
  The available host has no C compiler; the runtime change is Apple-only.
- Independent review prompted the explicit pre-interruption lock-owner witness.
  The test first returned 15 with inherited parent identity, then 127 with the
  corrected runtime and the strengthened handshake.

A targeted Pulse restart after preserving evidence removed the retained workers;
its health route answered 200 in 0.67 seconds. That is operational recovery, not
proof that the new listener limits are deployed. The application pin and rollout
carry that separate observation. Shared kernel inputs now rebuild both API and
Pulse in the application deployment selection.

Implementation/review used OpenAI gpt-6-astra; native execution made no model
calls. The lesson is ownership through completion: proxy connection lifetime,
native worker lifetime and the identity owning durable effects each need their
own evidence. The failed recovery test made the inherited identity visible.
