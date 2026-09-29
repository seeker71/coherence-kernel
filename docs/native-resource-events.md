# Native resource events

Form generates Darwin ARM64 event-queue calls in RAM and owns their registrations,
buffers and release. Filesystem notifications, one-shot timers, process exit and
borrowed descriptor readiness arrive in one batch. No C change, linked adapter or helper
process supplies this event carrier.

[`native-events-darwin.bml`](../form/form-stdlib/bml/native-events-darwin.bml)
uses the host SDK's `kqueue` / `kevent64` interface and 48-byte event layout.
[Apple's interface description](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html)
explains vnode notifications, coalescing and descriptor lifetime. The current
SDK supplies the timer and `kevent64` contracts; their execution is observed
on this Darwin ARM64 host.

## Event ownership

`nve-open` creates an owner. `nve-path` registers a path;
`nve-timer` registers a one-shot delay in milliseconds; `nve-readiness` borrows
a descriptor for read (`0`) or write (`1`) readiness. The caller keeps a borrowed
descriptor open until `nve-unwatch`. Owned path descriptors and the queue carry
close-on-exec. Registration tokens are monotonic Form integers and are never
recycled within an owner, including when host descriptor numbers are reused.
Diagnostic flows bind process birth and the shared native owner-construction
clock before adding the subscription token. Separate event owners retain
separate readings even when their local token numbers agree.
`nve-process` observes one process exit; its owning process organ still performs
the actual wait/reap. Readiness is level-triggered so a partial read retains
its next wake. Timers and exit subscriptions report once.

`nve-wait(owner, milliseconds)` returns owned registration references, event
flags, vnode notes, any event error and the signed event data. A data value
outside Form's integer range signals a need and remains absent. An empty list is an observed quiet wait;
`nothing` retains a refusal in the owner's `reading`. Queue waits, event counts,
native calls and current registrations remain in the owner. Its memory owner
has separate native-call accounting. The complete batch is read before a caller
changes registrations. Event storage grows with the actual registration set;
released buffer records leave that owner.

`nve-unwatch` removes a registration and closes only an owned path descriptor.
`nve-close` settles registrations, queue, mapped storage and native admissions
in order. Incomplete release retains ownership for another attempt. The owner
is cooperative Form state; callbacks run in the receiving context. This is not
preemptive scheduling or a carrier for other operating systems.

## One queue, retained receiving contexts

`nve-bind(events, watch, receiver, context)` binds a subscription to
`receiver(context, event)`. `nve-turn(events, milliseconds)` reads a complete
batch, dispatches each still-live subscription and returns the event count.
A batch captures each receiver and its context. Rebinding a live subscription
selects the receiver for later batches; a captured event keeps its original
receiver. Bind before waiting. Raw event rows carry the binding after their
signed data field.
A receiver may release or replace registrations; later stale entries in that
batch cannot act on their replacements. Unbound events remain available through
`nve-take`, including their original registration references.

Pipe and JIT owners can borrow the same queue through `npp-open-with(events)`
and `bdje-open-with(owner, root, events)`. A turn advances only the receiving
operations. Child exit, stream data, cancellation deadlines and cache-resource
changes retain their separate owners and original context. Closing a consumer
withdraws its subscriptions and preserves the queue. Its caller closes the
queue after its consumers have settled. Whole-batch dispatch is cooperative;
a receiver's execution time remains part of the turn. A nested wait or dispatch
signals its active receiving turn without consuming another batch.

## Cooperative Form slices

`nve-post(events, receiver, context, options)` offers local work to the same owner.
It returns an owned subscription without a host registration, timer or descriptor.
Its filter is 0, the local Form offer, and it shares the existing continuation,
care and release lifecycle. Host event buffers count actual registrations.

Inside a receiver, `nve-yield(events, event, next, context, options)` proposes
the next slice and its checkpoint. The receiver must return; the call does not
unwind its stack. A normal return commits that next slice at the queue's tail.
A native stop discards the proposal and brings the interrupted step into care.
Original event data and accumulated findings travel with each next binding.

`nve-run(events, slices)` grants up to that many ready slices without a host wait
and returns the number executed. The two-list queue retains arrival order, so
a yielding job gives its peers their turn. `nve-turn-slices(events, milliseconds,
slices)` first dispatches the complete host batch and its resource care, then
serves the granted ready slices. Ready work makes the host wait nonblocking.
`nve-turn` grants one slice; its return remains the host event count. The owner's
`steps` reading counts ready slices separately. `nve-dispatch` remains batch-only.
A zero grant executes no ready work; the caller can change its grant each turn.

A care option can propose a next slice and acknowledge ONE. Only that accepted
option's proposal survives; findings from earlier options remain. The resource
wakes retire before the new slice becomes ready, and the delivery stays pending
until its last step completes. Correlated readings follow that movement through
yield and completion. A yielded native receiver stays paused between slices;
pausing it again adds no host call. Completion restores its native subscription.

Local completion retires its token. `release-event` withdraws queued work through
the same correlated care door. Stale queue entries cannot replay it. Removing
a subscription after proposing a next slice leaves that checkpoint held for
care; it never enters the ready queue. Closing the owner retains unfinished work; nested turns, grants and self-care cannot consume
the active slice. Slice grants bound the number of cooperative calls, not their
wall time. Structured repetition uses the shared BMF decision below.

## BMF repetition and local retry

`nve-repeat(events, body, context, cursor, options)` offers a BML body one or
more times on that queue. It returns the existing owned subscription. The body
returns `nve-repeat-next(nextCursor)` for another pass, or an ordinary value
to finish. Both zero and `nothing` are valid final values. The repeat has no
fixed attempt limit; each turn's slice grant still controls how much work runs.

BMF `Repeat` / `multi-match` and this BML path share
[`bmf-repeat.bml`](../form/form-stdlib/bml/bmf-repeat.bml)'s decision:
an advancing match offers another pass, a missing match ends repetition, and
an unchanged checkpoint reports no progress. BMF retains its minimum, maximum,
captures and cut semantics. An absent maximum means unbounded; zero means an
empty allowance. BMF's synchronous matcher retains its tail-call driver.

A stopped body unwinds into its existing local care options. After repairing
the cause or choosing another path, an option returns
`nve-repeat-again(offered, nextCursor)`. That acknowledgment proposes a queued
retry. It never calls the body. The new cursor is admitted when the next slice
starts; declined options and cancelled tickets cannot commit it. Original
context, event, owner, completed effects and accumulated findings remain held.
Use an immutable cursor naming the actual changed choice or resource observation;
changing a counter alone does not establish a repair. An unchanged retry
contributes a finding and rests in care when no other choice responds. Resource
events can resume it through `nve-follow`.

The subscription's `repeat` record exposes `cursor`, `attempts`, successful
`passes`, `done` and the final `value`. `done` records a normal body completion;
a care acknowledgment or cancellation has its own continuation outcome.
[`native-repeat-witness.bml`](../observe/native-repeat-witness.bml) executes
repetition, failed-then-repaired attempts, retained findings, cancellation,
resource wakes, peer progress and physical release. BML bodies use this repeat
door without explicit self-calls or yields. Arbitrary source loops still need
compiler lowering to become cooperative; non-returning bodies are not preempted.

## Interruption stays with its receiver

Each receiver runs through `oac-offer`. A native value stop returns to that
offer; later receivers in the batch continue. An ordinary return, including
`nothing`, completes delivery. The interrupted event, captured binding and
original context stay on the subscription as a continuation. Persistent
readiness is disabled while held, so unchanged readiness does not repeat the
receiver or spin the queue. Pause and enable preserve the subscription's original
filter notes and configured data. `nve-pending(events)` exposes the held subscriptions;
`nve-care-reading(watch)` exposes each correlated need. Payload and context
remain private; the health signal carries subscription identity and disposition.

`nve-bind-care(events, watch, receiver, context, options)` additionally offers
local options immediately after an interruption. The existing
`oac-backtrack-walk` supplies each option with original arguments
`[events, context, event]` and prior findings. A finding travels as `oac-node`;
`oac-one` acknowledges the offered step. A proposed next slice keeps the delivery
pending; without one, it completes. Silence or `oac-zero` keeps it held. Options
checkpoint completed effects in their own context and resume from that point. The event owner never implicitly repeats the receiver.
Completion here describes the supplied continuation's acknowledgment; the
receiving organ still owns verification of its actual result.

A resource change can resume the held work in the same process:

```text
let response = oh-response(nve-care-reading(watch), "continue-receiver");
nve-hear(events, watch, response, list(finishFromCheckpoint));
```

The offered options are native function values supplied by the caller.
Each attempt emits an applied action and a fresh correlated observation, retaining
findings for the next choice. Old responses and recursive care cannot replay
the action. If the continuation completed but enabling readiness failed, the
next response only re-enables it. A replacement binding does not take over
the held context.

## Resource changes bring attention back

`nve-follow(events, held, resources, options)` transfers dedicated, unbound
subscriptions to a held continuation. Path, timer, process and descriptor
notifications share the same queue and original receiving context. Existing
bindings and other owners' subscriptions remain theirs. One follow owns the
resource set until completion or explicit withdrawal. Its returned record
exposes active state, retained resources and actual attempt count.

```text
let resource = nve-path(events, resourcePath);
let follow = nve-follow(events, held, list(resource), list(finishFromCheckpoint));
```

Enrollment reactivates offered paused subscriptions and offers one immediate
observation after binding, covering a change
that arrived before notifications were attached. When enrolled inside a care
option, this observation follows that option's completed attempt and retains
its findings. Subsequent quiet waits do no care work. Resource notifications
queue attention; all ordinary receivers in the batch run before that attention.
Multiple wakes for the same continuation become one attempt, with every event
available in delivery order through `nve-wake-events(held)`. The enrollment
observation has an empty wake list. Options observe their actual resource need
and keep effect checkpoints in the original context.

Care-owned descriptor readiness is renewed with `EV_CLEAR` at registration,
so unread but unchanged readiness stays quiet and new data can wake it. The
enrollment observation covers the renewal interval. Ordinary pipe receiving
registrations keep their level behavior. Borrowed descriptors remain with
their original owner. A registration failure keeps its resource owned and
signals the incomplete admission.

A delivered one-shot subscription retires. When no enabled, bound wake resource
remains while work is held, the continuation signals `event-wake-source` as an
additional need; it does not create another timer or repeat unchanged work.
`nve-unfollow(events, held)` withdraws the wake subscriptions while preserving
the original continuation and findings. A new resource set can then be offered.
Completion and explicit event release withdraw their wake subscriptions before
settling the continuation. Incomplete physical release keeps ownership and
retains any already completed receiver acknowledgment for the next care attempt.
Captured wakes from a released follow cannot replay its action.

Unwatching releases the registration and preserves any held continuation.
After the receiving organ settles its resources, an explicit `release-event`
response to `nve-hear` releases that continuation without claiming its receiver
completed. Queue close retains ownership while continuations remain. This
boundary handles native value stops; fatal process exits, host faults and
non-yielding work retain their separate lifecycle requirements.

## JIT care

[`bml-demand-jit-events.bml`](../form/form-stdlib/bml/bml-demand-jit-events.bml)
connects these notifications to the existing JIT owner:

```text
let events = bdje-open(bdjo-owner(epoch));
let request = bdje-request(events, source, cache, program, root, argument);
let progress = bdje-progress(events);
let actions = bdje-wait(events, milliseconds);
let owner = bdje-owner(events);
let released = bdje-close(events);
```

Progress handles compilation and observation delivery. Waiting cache care uses
host events. Quiet waits do not poll its filesystem metadata. The owner watches
surviving ancestors when a path is absent, renews registrations after replacement,
and re-observes after enrollment to cover changes during registration. Unrelated
directory writes leave care unchanged. Care completed during enrollment returns
immediately in the wait's action count. Attribute and replacement events supply
information that size/mtime observations cannot carry. Lease renewal replaces
its timer; expiry resumes the same operation. Explicit holds withdraw watches.

The response runs publication and readback against the retained image and
runtime identity. A failed attempt keeps its new finding in the same choice
flow. Delayed telemetry retains ordered evidence without delaying this care.
Closing the event lifetime preserves the caller's JIT owner and native pages.
With a borrowed queue it also preserves the other event consumers.
`form-cli-jit.bml` uses this path through its demand request, progress, wait and
close operations.

An inaccessible watch or unavailable event resource remains a signalled need.
Volume free-space changes still require an offered response from a resource
observer; vnode events do not establish capacity. Full source-unit admission,
other host targets and preemptive Form execution retain their separate work.
