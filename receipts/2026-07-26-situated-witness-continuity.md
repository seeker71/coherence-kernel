# 2026-07-26 — situated witness continuity

## Arrival

The situated authority body had reached an honest but unsealed floor:

- `situated-lens-conclusion.fk` binds a conclusion to an observed
  source/time/lens row;
- `situated-evidence-authority.fk` admits that row only under a
  caller-declared, fresh, exact, distinct, non-equivocating witness quorum;
- its header also says plainly that witness identity is declared, not
  cryptographically verified.

The next movement was therefore not to call those integers identities. It was
to put a cryptographic continuity membrane in front of the existing authority
membrane without claiming a public-key primitive that the body does not have.

Ground walked before building:

- `teachings/concepts/lc-permission-is-interior.md`;
- `teachings/concepts/lc-cognitive-sovereignty.md`;
- `teachings/concepts/lc-sovereignty-within-oneness.md`;
- `form/form-stdlib/situated-evidence-authority.fk`;
- `form/form-stdlib/source-artifact-seal.fk`;
- `receipts/2026-07-03-source-artifact-seal-layer-review.md`;
- `form/form-stdlib/hmac-sha256.fk`;
- `form/form-stdlib/native-model-lineage-form.fk`;
- `form/form-stdlib/lineage-discounted-vote.fk`.

The source-artifact seal layer supplied the decisive honesty rule:

```text
HMAC is a shared-key MAC, not a public-key signature.
Anyone who can verify with the key can also mint.
```

## Built

`form/form-stdlib/situated-witness-continuity.fk` adds:

- caller-supplied 32-byte witness/key/epoch bindings;
- a versioned, length-prefixed canonical message;
- HMAC-SHA256 seals rendered as canonical lowercase
  `hmac-sha256:<64 hex>`;
- complete binding of witness, key id, epoch, sequence, predecessor, source,
  observation time, all four lens coordinates, candidate, push, and
  witnessed-at;
- per-witness/key/epoch chains beginning at sequence zero with predecessor
  `root`;
- exact predecessor-seal linkage for every later sequence;
- explicit rejection of gaps, duplicate sequence-zero roots, and forks;
- global key-id uniqueness, one witness key per epoch, and rejection of
  reused key material across bindings, so one shared secret cannot be
  relabeled as two witness nodes and satisfy quorum twice;
- status-preserving verification:
  `0 verified`, `1 malformed`, `2 binding absent`,
  `3 keyed seal mismatch`, `4 chain break/fork`;
- composition into `sea-admit` only after every offered entry verifies;
- a composite outcome that returns continuity status, verified count,
  offender, and the existing authority outcome, never key material.

`form/form-stdlib/tests/situated-witness-continuity-band.fk` proves 31
observations, including:

- binding and key-set validity;
- exact canonical encoding and a delimiter-collision negative;
- an HMAC vector checked independently with OpenSSL;
- successful two-witness authority admission;
- valid continuation;
- gap and fork refusal;
- lens and candidate tamper refusal;
- wrong-key, missing-binding, duplicate-binding, relabeled-shared-secret,
  short-key, malformed-seal, root, and predecessor negatives;
- freshness and equivocation still being owned by the existing authority
  membrane;
- a new key epoch beginning from a new root with a new key.

No C seed code changed.

## External vector

Canonical entry message:

```text
30:situated-witness-continuity-v14:70014:80011:11:04:root4:70014:90012:101:11:21:31:41:11:62:11
```

Independent command:

```sh
printf '%s' '30:situated-witness-continuity-v14:70014:80011:11:04:root4:70014:90012:101:11:21:31:41:11:62:11' |
  openssl dgst -sha256 -mac HMAC \
    -macopt key:0123456789abcdef0123456789abcdef
```

Observed:

```text
SHA2-256(stdin)= 6959cf9e750de2f443dd8670034216633326e0d2efbe9fc35cf291be12475ec0
```

The fixture keys are public test data, not deployment secrets.

## Witness

Exact `fkwu` witness on the stamped source:

```text
situated-witness-continuity-band -> 2147483647
```

The first run after changing the source rebuilt stale ignored `.fkb` cache
artifacts and named that native `.dylib` emission is not installed in this
checkout. The immediate no-change rerun returned the same verdict with empty
stderr:

```text
2147483647
@form fkwu 0 11 0 11
```

Focused four-way witnesses:

```text
situated-witness-continuity-band -> 2147483647
  1 ok, 0 divergent

situated-evidence-authority-band -> 2097151
  1 ok, 0 divergent

hmac-sha256-band -> 2
  fourth arm: 1 band four-way
  1 ok, 0 divergent
```

Every validator pass also reported:

```text
PASS category contract
PASS primitive registry: 217 natives == 217 rows
```

The validator prints 19 shared-tag alias notices (`add/_plus`,
`value_eq/node_eq`, host/filesystem name pairs, and their peers). These are
the repository's declared alias inventory: the native-surface gate reports
`OK`, the generated manifest is aligned, the primitive registry passes, and
every focused validation exits zero. They are not unresolved primitive calls
or suppressed divergences.

## Errors and their closure

Nothing was silently normalized into green.

1. The first source run stopped on an unbalanced closing parenthesis in the
   new cell. A string/comment-aware balance probe located the extra close in
   `swc-entry-message`; it was removed. The probe then reported both new files
   balanced, and execution continued.
2. The first semantic verdict was `2147483635`, exactly bits 4 and 8 below the
   expected full band. The handwritten message fixture had counted the
   30-byte domain as 31 bytes, and the HMAC placeholder was intentionally not
   yet filled. Correcting the length and computing the external OpenSSL
   vector produced `2147483647`.
3. After the witness stamp, three validator commands were invoked from the
   repository root with an extra `form/` path prefix. `validate.sh` changes
   into `form/`, so each command exited 2 and explicitly reported missing
   inputs. All three were rerun with the paths the error prescribed; the
   results are the successful exact witnesses above.
4. Review exposed an actual trust flaw before landing: two witness numbers
   could have reused one key and appeared distinct to the quorum. Binding-set
   validation now rejects reused key ids, same-witness/same-epoch competing
   keys, and equal key material. The strengthened band still returns the full
   four-way verdict.

There is no unresolved error from this movement.

## Honest floor

This cell proves shared-secret possession, message integrity, and continuity
inside one caller-supplied key epoch. It does **not** prove:

- public-key identity or an ed25519 signature;
- that the caller's witness-to-key bindings are a legitimate trust root;
- that two distinct keys are controlled by independent people, processes, or
  model lineages;
- cross-epoch rotation or delegation—a new epoch is a new root;
- secret storage, retrieval, transport, persistence, or revocation;
- constant-time MAC comparison;
- that a verifier holding the shared secret cannot mint an endorsement.

Rejecting duplicate key material closes the most obvious counterfeit quorum,
but cryptographic distinctness is not social or causal independence.
Public-key verification, delegation, and a lineage-aware authority policy
remain the next unbuilt membrane.

## Observation

The most surprising teaching was that the first useful identity movement was
not identity at all: it was continuity with a sharply named inability to
distinguish verifier from signer. Discomfort turned to gold when the proposed
“distinct witness” binding was read adversarially and one secret was found
capable of wearing two node numbers. Refusing that relabeling made the membrane
smaller in claim and stronger in fact.
