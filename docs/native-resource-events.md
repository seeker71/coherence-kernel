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
A receiver may release or replace registrations; later stale entries in that
batch cannot act on their replacements. Unbound events remain available through
`nve-take`, including their original registration references.

Pipe and JIT owners can borrow the same queue through `npp-open-with(events)`
and `bdje-open-with(owner, root, events)`. A turn advances only the receiving
operations. Child exit, stream data, cancellation deadlines and cache-resource
changes retain their separate owners and original context. Closing a consumer
withdraws its subscriptions and preserves the queue. Its caller closes the
queue after its consumers have settled. Whole-batch dispatch is cooperative;
a receiver's execution time remains part of the turn.

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
