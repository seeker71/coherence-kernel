# Draft reading — source observations beside the invitation

`observe/draft-native-reading-run.bml` supplies a native reviewer with the
captured request, the earlier turn, the draft, the paths the drafter read and
separately attributed source observations. The reviewer returns its own
`accepted`, `revised` or `rejected` reading with a reason. A predicate never
accepts or rejects the prose before that review.

The default `covenant.jsonl` observes named source paths. For a path the person
did not supply, the reader checks the tree at the draft's time. It records
whether that path stood then; absolute and never-committed paths are checked
where they stand. A path missing from that reading may be an error, a proposed
artifact or something the source packet does not establish. The reviewer sees
the finding beside the actual request.

Each observation row carries `id`, `predicate`, `instantiate`, `teaching` and
`source`. Existing caller-selected predicates can still describe their own
matches. Their tallies stay in the receipt and travel into the review packet.
The default no longer excludes negative sentences, security vocabulary,
receipt words or new numbers. Words alone do not establish care, truth or a
person's state. Quantities need appropriate reasoning or evidence; banning a
digit cannot establish either.

The draft's original marks retain what the writer received:

- `# Captured request` holds the captured text. Legacy `# The person asked`
  headings still read. Text prefixes do not establish authorship or authority.
- `# The body's draft` holds the response.
- `# The turn before (what the voice was given)` preserves earlier context.
- `# Files the voice read` names source paths; the reviewer is told that their
  full contents are not included in this packet.

The drafter preserves nonblank captured requests, including short requests,
quoted instructions and markup. Completion comes from the generation record;
a truncation phrase quoted in prose does not make a finished response unfinished.

`covenant_only` observes sources while leaving the draft pending. A missing
source history or an absent reviewer response stays explicitly unresolved.
The caller's actual deadline controls whether another review may start; a
fixed forecast of how long a model might need adds no separate cutoff.

A fixture reading exercises the same flow in an isolated ledger and is marked
as a fixture. It does not demonstrate native answer quality or become the
body's reading of record. Actual generated reviews, source observations,
caller choices and later learning remain distinct evidence. The existing
`form/form-stdlib/tests/draft-native-reading-band.bml` observes these paths.

Projecting selected readings records the source and reader, not an authorization
claim. A useful review does not establish captured-text authorship or permission
to retain, share or train on it; the request using the rows supplies that scope.
