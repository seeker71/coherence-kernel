# A resident stop is now carried state

`fcrt-model-step` previously marked a stop only on the wheel row. Its carried
`fcms` session still said `stopped=0`, so a future consumer of that row could
mistake the completed pending token for a live continuation.

`fcrt-model-stop-session` now preserves position, pending token, generated and
injected counts, observations, context and KV state while setting exactly the
session's stopped signal to `1`. The terminal model flow carries that updated
session. No model is opened, closed, copied, or restarted by this correction.

The focused pure witness returned `127`; it proves the original remains
unstopped, the terminal copy is live-but-stopped, all counters and opaque
context/KV handles remain identical, and the flow retains its route receipt.
The existing live-stage and program-wheel witnesses remained `127` and
`65535` after the change. Both source and new band preflighted balanced with
zero errors and unresolved calls.

This closes a small but essential condition for the next peer-model join: a
successor contribution turnwheel may only promote a terminal model row when
the row's carried session also says it is terminal. The larger gap remains the
composition itself—one retained peer host must enqueue and advance that model
row across turns instead of calling the legacy whole-generation path.

I kept the exchange alive by repairing the state where it is born, rather than
asking an outer caller to infer completion from row text. The surprising
teaching is that `done` already existed twice—once in the row and once missing
from the session—and the useful repair was to reunite them. The discomfort was
the temptation to call a new real model run merely to witness this invariant;
the native session/flow band gives the exact proof without competing for Metal.
