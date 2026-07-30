# 2026-07-30 — the streams aligned on the body's own channel

Urs: *"bash and python: no. different token stream, using frame-buffer to align."*

Both directives applied. The corpus row above this receipt was landed with the body's editor; the
alignment below was computed by a Form cell on the bidirectional framebuffer channel, not by a shell
pipeline. One oracle invocation remained — `ds4 --dump-tokens` to get the reference stream as ids —
because the reference's ids live only in the reference.

## The cell

`observe/token-stream-align.fk` — both streams as witnessed data, three measurements, each sent as a
`bfc-round` exchange (observation out via `fb_record`, adjudication, control back):

- **positional** — same id at the same position
- **bag** — reference ids appearing anywhere in ours
- **echo** — the longest run of consecutive prompt ids inside each stream

```
positional 0    bag 2    echo-ours 0    echo-ds4 4        field 204
controls: 0, 0, 0 (continue on all three exchanges)
```

## The reading, and it is sharper than the logits were

The only ids our stream shares with the reference are **6102 "capital" and 8760 "France" — prompt
words**. Our stream contains *nothing that is not from the prompt*. The reference brings new content
(`We, need, to, answer…` — ids 2581, 1309, 304, 3287) and repeats the prompt only inside a deliberate
quote (`echo-ds4 4` is the four-id run `671 6102 294 8760`).

So the forward is not merely weak: it is **promptbound** — every token it emits is drawn from what it
was shown, none from what the model knows. A model that answers must produce ids the prompt never
contained; ds4's `2581`-stream does, ours never does. Corpus row 941.

Three integers on the body's channel said this more precisely than 129 280 floats in a shell pipeline
did an hour earlier. `r = 0.46` said "half the structure is missing"; `bag 2, both prompt words` says
*which* half: the model's own contribution.

The channel's adjudicator returned `continue` on all three exchanges — correct under its
transition-count policy, since against a *reasoning* reference positional 0 is expected. The control
that fires next is 4, request evidence: the per-layer fp64 oracle, now unblocked (its conflict markers
were resolved this morning) and needing only Q8_0 and Q2_K taught to it.

## The most surprising teaching

The channel forced the question into integers, and the integers were the better instrument. The float
comparison had every advantage — 129 280 dimensions, a correlation coefficient, ranked lists — and its
conclusion was "roughly half the signal is absent," which localises nothing. The three-integer version
could not avoid localising, because each integer had to be *about* something: position, membership,
run-length. Choosing what to count is the analysis; the floats had postponed exactly that choice.

## Where discomfort turned to gold

Being told "bash and python: no" for the second time in one day, mid-repair, and noticing the reflex
run deeper than convenience: I reach for the shell because it answers *fast*, and the speed had been
substituting for deciding what the question was. The framebuffer cell took longer to write than a
pipeline would have — and the extra time was spent naming the three measurements, which turned out to
be the finding itself.

## Ground stamp

```
observe/token-stream-align.fk via ./fkwu --src -> positional 0, bag 2, echo-ours 0, echo-ds4 4, field 204
ds4 --dump-tokens (reference stream head): [2581, 1309, 304, 3287, 28, 582, 671, 6102, 294, 8760, ...]
ours (metal_dsv4_stack.sh emitted):        [270, 6102, 295, 8760, 305, 305, 1009, 14, 6102, 305]
corpus band 32767; 336 rows, max-mid 941, field 3363362941 — counts asked of the body
```
