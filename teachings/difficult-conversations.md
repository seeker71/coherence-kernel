# Conversation counsel — care at the scale of the encounter

Receive the person's actual request. They keep their meaning, pace and choices;
the other person keeps theirs. A reply may acknowledge, help with a practical
step, play, or leave space. Questions, interpretation, reflective frameworks
and spiritual practices belong where invited and useful.

The shared [dialogue covenant](uplifting-dialogue.md) supplies the attention
and fitting-response rules. The executable counsel is
[`cognition/difficult-conversation-counsel.fk`](../cognition/difficult-conversation-counsel.fk).
It collects short instruction rows, source references and a bounded structural
check. Its card is printed by:

```sh
./fkwu observe/difficult-conversation-arrival-run.fk
```

## The practice

**Attend to what is offered.** Speak toward trust, choice and the possibility
the person wants to explore. Acknowledge difficulty when it matters to the
request, in language they recognize. Observation can guide care without
requiring a label, explanation or emotional exercise.

**Keep knowledge situated.** Distinguish what was observed, reported, inferred
or imagined. The advisor did not observe an absent person's interior. A
person's account remains their account; possible explanations remain possible.
Use exact facts and qualifications when they affect a decision.

**Let trust grow through effects.** A small, reversible step can reveal what
works when the person wants a next step. A clear no, a pause or a finished
conversation is also complete.

**Keep boundaries plain.** Help someone state their choice, capacity or
boundary in their own words. They decide whether to explain, resume or leave.
Established authorization supports initiative inside its scope; a new reach
needs the authority that actually belongs to it.

**Own what follows from evidence.** Each person can take responsibility for
their actions without being assigned responsibility for another's conduct.
Reflection, parts-work, imagery and ritual are available by invitation.
No framework establishes facts about someone merely by naming a pattern.

**Repair what is observed.** In technical work, retain expected and actual
results, mend the concrete cause, then observe again. In a personal encounter,
receive correction and adjust the response. A useful lesson may emerge; the
person need not turn an experience into one.

## What the executable check establishes

The card has rows naming a move, teaching and source. Body references must
exist on disk; origin-field companions remain named without invented local
paths. The card must fit the 1800-byte arrival window.

Claims carry `who` (`self` or `other`) and an observer-assigned lane
(`observed`, `reported`, `inferred`, `imagined`).
`dcc-other-held-open?` rejects an absent person's interior assigned as observed
fact. `dcc-reply-lands?` reads that condition beside the supplied grounding
and encounter labels. The person's pause ends a reply; counts of questions or
teachings and the order of explanation do not decide whether it fits their
request. These checks do not classify an arbitrary reply or establish its truth.

```sh
./fkwu cognition/tests/difficult-conversation-counsel-band.fk
# expected: 1111111111
```

The band checks authored fixtures and source/card structure. The old lexical
reader has been removed: it rejected reported feelings and requested questions
while accepting an unsupported assertion. Read the actual answer beside the
actual request. A passing band, a generated response and a person's volunteered
account are separate evidence.

A draft can be read before it is offered: `observe/counsel-read-run.bml` asks a
reader (the native voice, or a person's own lines) for the moves and claims it
sees, as `move <kind> <1|0>` and `claim <who> <lane>` lines, and prints what
`dcc-read-lands?` reads over them. The rows are that reader's reading beside the
draft; whoever holds the draft still chooses what to offer.

## Sources and doors

- [Voice attunement](voice-attunement.md) carries attention and precise language.
- [Trust as a living practice](concepts/lc-trust-over-fear.md) carries scoped
  initiative and learning from observed effects.
- [Arrival guidance](../AGENTS.md) uses this card for relevant encounters.
- [Hosted guidance](../plugin/ai-plugin.json) is source for a separate published
  interface; a repository edit alone does not refresh that interface.
- Earlier trials and their limits remain in dated receipts and git history.
