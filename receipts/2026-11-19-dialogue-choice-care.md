# Language, choice and draft evidence

This repair followed actual counterexamples in the dialogue and draft paths.
It removes automatic judgments from word matches and cardinality rules. It
adds no gate, lexical quality replacement, model training or public message.
The checks below are local, native execution with synthetic inputs. They do
not establish native generated-answer quality; the companion's actual replies
are separate observations.

## What failed and what changed

`form-run ./fkwu /tmp/form-counsel-reading-probe.bml` observed the former
whole-reply classifier:

```text
[reported-interior, 0]
[requested-two-questions, 0]
[unsupported-fact, 1]
```

It rejected “You said he feels sad. What would help?” beside “He told me he
feels sad,” and two questions the request explicitly invited; it accepted
“The Moon is made of cheese.” `dcc-read` and its lexical rules were retired,
along with `cp-lands-line`. Authored dialogue observations remain authored
observations. A grounded response can contain several questions or teachings,
finish with an answer, or honor a pause. The person's request supplies depth.

`form-run ./fkwu /tmp/form-draft-choice-probe.bml` retained a synthetic draft
that accurately said a service “does not run by day.” Before repair:

```text
[native-reading] P rejected voice=not-asked covenant=0/1 ... note=covenant 0/1; affirmative: does not
```

After repair the same unmet observation reached the fixture reviewer:

```text
[native-reading] P accepted voice=fixture covenant=0/1 ... note=accurately preserves the schedule
```

That is routing evidence, not a claim that a model reviewed the answer.
The default draft observations no longer exclude negative sentences, security
or receipt words, or new quantities. Remaining source observations go beside
the whole draft. A missing path may be a proposed artifact or unavailable
evidence. Actual `voice_status` now distinguishes an answered review from an
absent one; historical rows retain their original interpretation. The caller's
deadline is observed directly, without the old five-minute admission guess.

`form-run ./fkwu /tmp/form-source-enquiry-probe.bml` compared these requests:

| Request | Before: source lookup | After |
| --- | --- | --- |
| Which kernel source should I read? | 0 | 1 |
| Which source defines our kernel? | 0 | 1 |
| Which kernel source should we read? | 1 | 1 |

The personal-pronoun and possessive vetoes were removed. Query cues still
select source retrieval; matches are candidates, and a miss leaves other
sources and meanings open. Neither outcome establishes the person's meaning.

`form-run ./fkwu /tmp/form-captured-language-probe.bml` used the actual drafter
with four scratch capture rows and fixture replies. Before, it drafted one
request and preserved none of the four exact inputs: a short request, leading
markup and a quoted wake phrase were discarded; literal system-reminder text
was stripped from the fourth. After, it drafted all four and preserved all
four request bodies. The retained evidence is:

- Before: `/tmp/form-captured-language-1795065257553/result.txt`
- After: `/tmp/form-captured-language-1795065410957/result.txt`

The capture carries `cwd`, `prompt`, `session` and `t`; its wording cannot
establish authorship or authorization. New drafts use `# Captured request`;
both draft readers retain support for the legacy `# The person asked` heading.
Nonblank captured text survives without a minimum-length or prefix verdict.

`form-run ./fkwu /tmp/form-draft-completion-probe.bml` found the projection
omitting a completed response that quoted `(cut at the reply budget of `,
while including a generation-marked incomplete response without that phrase.
Before, the one projected row was `unfinished`; after, it was `literal`.
Completion now comes from the generating operation's `voice` and `cut` data.

- Before: `/tmp/form-draft-completion-1795065466620/result.txt`
- After: `/tmp/form-draft-completion-1795065508793/result.txt`

The projection receipt also stopped asserting `authorization: our own readings
of our own drafts`. Its only consumer was an expectation in the existing test;
runtime consumers select the row files, not that assertion. The receipt now
records the actual selected reader, sources and destinations. Existing walk
and explicit training requests retain their own scope; review quality is no
evidence of input authorship or permission to retain, share or train.

## Source and observed checks

The owning changes are in `cognition/dialogue-covenant.fk`,
`cognition/difficult-conversation-counsel.fk`, `observe/companion-ground.bml`,
`form/form-stdlib/role-prompts.bml`, the public plugin's welcome/instructions,
and the existing draft writer, reader, ledger and projection. Their existing
bands and guidance were updated with their callers. Role instructions no
longer impose positivity, one-step or sentence-count limits, blanket negative
word avoidance, or a model-origin quality verdict. The public unnamed welcome
no longer promises that nothing is retained.

Native commands and observed pins:

```text
form-run ./fkwu cognition/tests/dialogue-covenant-band.fk                  1111111111111111
form-run ./fkwu cognition/tests/difficult-conversation-counsel-band.fk    1111111111
form-run ./fkwu form/form-stdlib/tests/voice-door-ground-band.bml         1023
form-run ./fkwu form/form-stdlib/tests/lora-lanes-band.fk                 255
form-run ./fkwu plugin/tests/chatgpt-plugin-band.fk                       111111111
form-run ./fkwu plugin/tests/public-dialogue-contract-band.fk            255
form-run ./fkwu form/form-stdlib/tests/draft-native-reading-band.bml      65535
form-run ./fkwu form/form-stdlib/tests/draft-readings-rows-band.bml       63
form-run git diff --check                                               exit 0
```

The lora-lanes check is static role consistency and toy arithmetic, not a
model-training run. Draft and projection bands only write scratch datasets;
no training process was invoked.

The draft-native-reading band first returned `57343`: it exposed that
`dl-voice-read?` inferred no review from failed predicates. Recording the actual
voice status repaired it, and the band returned `65535`. After completion
moved to generation metadata, a later run returned `65519`: synthetic P-22
claimed truncation in prose while its fixture metadata said `answered`.
The fixture now records `incomplete`; its failing directory remains at
`/var/folders/xt/5zt6_wmn77x22yf_wgv97cb40000gn/T/draft-native-reading-band-1795065568840`.
The subsequent whole draft band returned `65535`, and the projection band
returned `63` with the actual-completion and receipt-scope cases.

Temporary authoring attempts also retained their failures: the first
`/tmp/form-dialogue-choice-repair.bml` expected a passage predating the newly
added disagreement case and wrote nothing; the corrected edit preserved it.
`/tmp/form-draft-review-repair.bml` failed on unresolved `split-on` after its
first source write; the remaining data change was applied directly.
`/tmp/form-dialogue-label-repair.bml` initially failed on the same unavailable
binding without writing; it was repaired to use existing string primitives.
These failures were resolved locally rather than repeated unchanged.

## Next pass: quoted controls stay examples

After `07abbbdaca`, the remaining live control grammar still treated inline
code as executable instructions. The original source is
`form/form-stdlib/bml/form-token-grammar.bml` at that commit. The exact native
probe is `form-run ./fkwu /tmp/form-inline-control-20261119.bml`; it feeds only
scripted text and a harmless arithmetic expression, without a model or retained
crossing ledger. Before the repair it returned:

```text
example=Use `<|form:eval|>6 * 7<|/form:eval|>` as an example.
prose=Use `` as an example.
complete-opening-labelled-unfinished=1
quoted-example-observation=value=42;
quoted-example-executions=1
```

The complete sentence used for the unfinished check was
`The opening is \`<|form:eval|>\`.`. The unchanged probe after repair returned
the exact original example as prose, unfinished `0`, an empty observation and
executions `0`.

The shared scanner now keeps backtick code quotations as text, including
inline spans and wider fences containing shorter runs. The stream window
keeps a quotation begun after an active frame and waits for the next chunk to
establish a closing run's width. Actual controls still execute, their payloads
remain verbatim, and the existing reserve and multiple-control behavior remains.
This is a code quotation boundary, not a judgment about human meaning or quality.

The affected callers are `ftg-feed` and `ftg-coding-chunk` for execution,
`fcnv-read-owned` for presented prose and unfinished status,
`fgcr-ordinary-prose` for ordinary generation, and grounded synthesis's partial
native response projection. They share the repaired grammar; no caller-specific
filter or new admission layer was added.

The existing `form-token-grammar-band.bml` now exercises single and double
backticks, fenced examples, wider quotation, longer closing fences, quotations
split across chunks, and a quotation following an actual control. It observes
only the intended active control crossing in both the interceptor and coding
parser. `form-run ./fkwu form/form-stdlib/tests/form-token-grammar-band.bml`
returned `1023` with all ten claims, and `form-run git diff --check` exited `0`.

The first edit's native compilation reported unbound `end-at` and `end-quote`;
spacing subtraction repaired those names, and the same probe recompiled and
produced the changed observations above. An additional compatibility case then
returned `1021`: an equal-width-only implementation failed to close a three-tick
fence with four ticks. The owning matcher now consumes equal or longer closing
fences, and the same band returned `1023`. These observations establish parsing,
execution and presentation behavior; no generated-answer quality claim follows.

## Next pass: transmutation keeps the source's meaning

`form-run ./fkwu /tmp/form-transmute-claim-20261119.bml` supplied this complete
fixture answer to the old `tm-served`:

```text
answer=This event does not serve [lc-ceremony]; [outside-the-list] is the source's own name.
recorded-as-served=[lc-ceremony]
```

`observe/transmute-run.bml` used that lookup directly for its `ideas served`
display and `ideas_served` ledger field. Source search found no runtime reader
of that field; historical receipts keep their original rows. The lookup now
has the narrower name `tm-mentioned-seeds`, its caller displays `seed mentions`
and writes `seed_mentions`. The identical negative sentence still mentions
the supplied ID; it establishes no agreement or service. Unknown names remain
in the answer. They are outside this supplied index, not counterfeit language.

The same probe retained the actual event packet before repair. For a source
that established no gift or relationship, the packet required two or three
ideas, a gift, four uplifting sentences and a final thread question. It also
required hedging every unobserved interior, including attributed reports of
feelings, and the passage path replaced absences with offered paths. The
repaired shared fidelity guidance preserves facts, negation, refusals,
uncertainty and reports; the event's connections and possibilities remain
interpretations, including evidence against an idea or no established link.
The supplied field vocabulary remains available for the requested telling.
These are observed packet changes, not observed improvements in generation.

The existing `transmute-band.bml` first returned `183`: its two packet checks
still required the retired `UPLIFTING DIALOGUE` title. The checks now observe
the current shared card, fidelity guidance, source packet and mention scope;
the same command returned `255`:

```text
form-run ./fkwu form/form-stdlib/tests/transmute-band.bml
```

The actual `observe/transmute-run.bml` then ran from
`/tmp/form-transmute-20261119` with source `fixture`, model `0`, blank seed
paths and the event `No connection to the supplied ideas is established.`.
The command used absolute paths to the repository's `fkwu` and source cell,
and completed with exit `0`, `seed mentions 0`, `voice=not-asked`. Readback of
the scratch ledger confirmed `seed_mentions:[]` and no `ideas_served` field.
No model ran, and no repository ledger was written.

The isolated invocation spent time lowering; its retained compiler warning
identifies `word-frequency-purify.fk` changing during that operation and the
subsequent re-lowering. A sampled stack was in the native BML lowerer. A
later stop request found the process already complete. A ledger-path-only
fixture of the same source was also run afterward and returned `0`; both
rows are confined to `/tmp/form-transmute-20261119/receipts/transmute-ledger.jsonl`.
No generated-answer claim depends on these model-disabled runs.
