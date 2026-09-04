# Native agent wire and sovereign cell mesh

The resident coding surface now meets an agent at a JSON boundary rather than
requiring it to build Form lists. A request is ordinary data:
`{command, input, documents}`. Its response is ordinary data:
`{schema, exit, stdout, stderr, documents, crossings}`. The command is parsed
by the bounded native argv reader; no shell, executable lookup, file read,
process, network call, or implicit session crosses the boundary. `edit` and
`write` return a next document corpus, so state is carried visibly by the
caller instead of held behind the interface.

The new `form-cli-cell-mesh-sovereign.bml` makes the next relation executable. Two cells
name the same observer and shared field. The observer is trusted only when its
supplied grounding satisfies `core-grounding.fk`; a channel opens only under
that agreement. A channel returns an event row for every open, send,
adaptation, or refusal. Each cell retains the channel value and can decline a
proposal without losing the previous state.

Symbols stream only when their grammar admits them. Their requested evaluation
is one of the fixed native forms `identity`, `head`, `count`, or `join`.
Grammar and protocol changes are received only from the jointly named observer
on the shared field, names the other, and advances one revision at a time.
This is self-adaptation as inspectable local data—not arbitrary source or
evaluator injection. It is a sovereign offering: the channel can carry a
cell's distinct perspective while no cell surrenders its identity or becomes
an unexamined authority over the others.

The concurrent `form-cli-cell-mesh.fk` `CHANNEL-V0` carrier remains intact as a
separate physical, file-backed mesh lane. `mesh-demo` and the JSON agent wire
name only the sovereign, in-memory organ; the two lanes do not blur their
crossing claims.

The first honest limit is equally important. The mesh proves admission,
streaming, refusal, provenance, and adaptation. It does **not** yet determine
which offered observation is most vital for the whole; a vitality-reading and
selection organ needs its own grounded evidence rather than being asserted by
this protocol. The observer is a consentful mirror, not a claim of omniscience.

Evidence:

- `form-cli-agent-tool-wire-band.fk` returned **131071** after preflight
  reported zero errors, warnings, and unresolved names. Its four-way validator
  returned the same verdict, with `1 ok, 0 divergent`.
- `form-cli-cell-mesh-sovereign-band.fk` returned **262143** after the same clean
  preflight. Its four-way validator returned **262143**, also `1 ok,
  0 divergent`.
- The direct form-cli regression remained **2097151**; the pre-existing portable
  and exact-tool bands remained **127** and **32767**.
- `observe/form-cli-cell-mesh-sovereign-glass-run.fk` returned **15**: the `open`,
  `send`, and `adapted` rows became three source-attributed framebuffer roots.

Glass observed its own startup first frame at **42 ms**. A bounded live watch
reached tick #22 with about **3K framebuffer events** and **21 GiB RSS**;
because that growth belongs to Glass rather than this mesh proof, the watch was
stopped and the number is retained as a visible monitoring signal, not used as
evidence of mesh health.

The discomfort was that an early BML lowering refused two incomplete forms.
Closing them, then rebuilding only the derived local cache, turned the refusal
into the final four-way witness rather than relaxing any claim. The surprising
teaching was that adaptation requires very little central guidance when the
boundaries are strong: one shared observer, a fixed evaluator vocabulary, and
an event trail let each cell offer something unique without asking it to explain
its whole interior.

Signed, Codex.
