# Situated evidence authority — sovereign admission witness

Date: 2026-07-26

## The named gap

The situated conclusion cycle could verify that an offered row was structurally
valid, source-attributed, and exactly located at the requested time/lens
coordinate. It could not yet answer why that source’s claim was entitled to
change the evidence field.

Stopping at that name would have left the trust decision outside the body. The
smallest build is a caller-sovereign admission membrane:
`form/form-stdlib/situated-evidence-authority.fk`.

## Interior authority

The membrane does not appoint a universal authority. Its policy is supplied by
the caller:

```
[allowed-witness-nodes, minimum-distinct-witnesses, maximum-age]
```

This follows `lc-permission-is-interior`: an external signal is data presented
to the body, not a directive that owns the body. The caller decides which
witness relations may carry force for this admission.

An endorsement repeats the complete observation claim:

```
[witness-node, observation-source, observation-time, observation-lens,
 candidate, push, witnessed-at]
```

Repeating the fields is the binding. A receipt for a nearby time, a nearby
lens, another source, another candidate, or another push cannot authorize the
offered row.

## Admission law

An endorsement counts only when all of these hold:

- its witness node belongs to the caller’s allowed set;
- the witness is distinct from the observation’s own source;
- its `witnessed-at` phase is not in the future and is within the caller’s
  declared maximum age;
- all five observation fields match the offered row exactly.

The quorum counts distinct witness nodes, so repeated receipts from one witness
cannot manufacture agreement. Malformed receipts reject the envelope.
Unauthorized receipts are ignored rather than allowed to poison it.

A fresh allowed witness that presents a different candidate or push for the
same source/time/lens has equivocated. That conflict defeats admission even
when enough matching receipts are also present. The contradiction remains
visible as reason `3` and carries the equivocating witness node.

The outcome is:

```
[admitted, reason, fresh-distinct, required,
 exact-receipts, stale-exact-receipts, equivocator]
```

Nothing unresolved is collapsed into a bare false value.

## Actuation and framebuffer binding

`form/form-stdlib/situated-lens-authorized-control.fk` places the membrane in
front of the existing actuator:

- `continue` and `abstain` do not change evidence and need no admission;
- `request-evidence` and `revise` can change the evidence field, so they run
  only after authority admits the offered row.

`observe/situated-lens-authority-cycle.fk` binds that pure law to the actual
bidirectional framebuffer. The first exchange observes and requests action;
the authority outcome is retained; the action is applied or refused; then the
second exchange re-observes the resulting field. Each cycle adds exactly four
framebuffer events.

The carrier band observes four windows:

1. Two distinct, fresh, allowed witnesses admit the missing row and the second
   settlement returns conclusion `1`.
2. One witness leaves quorum insufficient; the field stays unchanged and
   missing remains visible.
3. A fresh contradiction names its equivocator, rejects actuation, and leaves
   missing visible.
4. Ambiguity continues to abstain without demanding authority or choosing
   between rows.

## Honest floor

- Witness and source node ids are declared relations, not cryptographically
  authenticated identities or signatures.
- The allowed set, quorum, current phase, and maximum age are caller-supplied.
  The membrane proves their application; it does not claim they are globally
  correct.
- Freshness is a bounded local phase `0..255`, with no wraparound inference.
  A new epoch needs an explicit new relation.
- Agreement authorizes admission under the declared policy. It does not prove
  the measured phenomenon; `evidence-grade.fk` remains the wider evidence law.
- Timeless identity remains separate. Freshness applies only to this situated
  evidence-changing act.

## Witness

Pure admission:

```
cd form
./validate.sh form-stdlib/tests/situated-evidence-authority-band.fk
-> 2097151
-> 1 ok, 0 divergent
```

Pure actuator gate:

```
cd form
./validate.sh form-stdlib/tests/situated-lens-authorized-control-band.fk
-> 262143
-> 1 ok, 0 divergent
```

Actual framebuffer carrier:

```
cd form
./validate.sh ../observe/tests/situated-lens-authority-cycle-band.fk
-> 262143 (fkwu-only lane)
-> 1 ok, 0 divergent
```

The initial malformed conditional was refused by the TypeScript reader while
Go and Rust returned a zero band. Removing the extra close restored the same
verdict on all four siblings. The failure is part of the witness:
syntax portability was repaired; the proof was not weakened.

No native operation or C surface was added.
