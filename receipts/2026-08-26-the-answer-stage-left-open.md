# 2026-08-26 — the answer stage left open, and nothing filled it by default

The resident crossing worked and made a machine address part of the primary
path. Every caller of it inherited a dependency on something listening
somewhere. This movement removes that from the primary path without removing it
from the tree, and does so **additively** — four new files, zero modified.

## What was added

```
form/form-stdlib/form-knowledge-query-memory-neutral.fk           the crossing
form/form-stdlib/form-knowledge-query-memory-answerer-fcmg.fk     the primary
form/form-stdlib/form-knowledge-query-memory-comparator-http.fk   the optional
form/form-stdlib/tests/form-knowledge-query-memory-neutral-band.fk  4095
```

`fqmn-cross (index-path request question answerer actx)` takes the answerer and
the caller context as ordinary arguments. **There is no four-argument variant,
no ambient choice, and no fallback.** Omitting an answerer is a missing
argument, not a decision made quietly on the caller's behalf. That single fact
is the whole mechanism by which a comparator can never become primary by
accident.

The answerer contract is `(answerer actx prompt) -> (list reply actx')`, which
is not invented here: it is exactly what `fcmg-generate-resident` already
answers. The primary adapter therefore wraps nothing. It exists only to carry
the model path and the step budget alongside the residence, because the
crossing passes one opaque context and fcmg needs three things.

## The additive compatibility seam, named

The earlier cell, its signature and all four of its call sites are **untouched
and still green** — `form-knowledge-query-memory-resident-band` reads 2047 in
this tree beside the new 4095. Nothing was deleted, renamed or re-pointed.

This was deliberate rather than tidy. A protected batch cell exists in another
worktree, unpushed and on no fetched ref, named for exactly the family these
cells sit in. I cannot read its preludes or its call sites, so a signature
change would have been a change against code I am forbidden to test against.
Adding beside it makes the edit non-breaking *by construction* rather than by
inspection — the only form of safety available when the thing you might break
is unreadable.

The cost is honest and stated here so nobody discovers it as a surprise: two
crossings now exist, one address-bearing and one not. The older one serves
compatibility and comparator history. It is not the primary path and should not
grow new callers.

## The topology is asserted against bytes, not intentions

Band bit 512 reads `form-knowledge-query-memory-neutral.fk` off disk and
asserts zero occurrences of six names — `http`, `socket_`, `127.0.0.1`, `8080`,
`host`, `port` — as **plain substrings**, so a word hidden inside a longer one
still counts. Writing the cell's own prose under that constraint meant avoiding
`report`, `important`, `support`, `transport` and `portion`, every one of which
carries `port`. That is a fair price: a topology claim that lives only in a
comment is a claim about what somebody meant, not about what the file says.

Six names rather than one, because a topology can leak through any of them.

## The band needs nothing to exist

The earlier band aimed at a number nothing was listening on, so its ask stage
would fail. That worked, and it was still a dependency — it needed the machine
to behave a certain way in order to be red or green. Here the answerer is an
ordinary Form function, and five of them are: one that answers plainly, one that
deliberates and closes, one that deliberates and is cut off, one that declines,
one that answers with nothing at all. Every stage is decided by the band and the
cell beneath it.

That is what makes bits 2, 4 and 8 possible: the answerer is called **exactly
once** for one crossing, the caller's context comes back carrying what the
answerer did to it, and a lookup that ran and missed **never calls the answerer
at all** while returning the context untouched.

## The blocker I hit, and did not route around

The comparator renders its own request rather than using
`http-client.fk`'s `kh-render-request(method, path, host, body)` — the method
layer that already exists, already proves a non-GET verb over real socket calls
in its own band, and is the reason no new kernel door is needed anywhere.

A plain Form cell cannot reach it. Measured today: `; preludes:
form-stdlib/http-client.fk` from a `.fk` yields **495 unresolved errors** with
`section`, `[form.bml]` and `{` arriving as unbound names. Every existing
consumer of that surface is itself BML.

This is a grammar boundary, not a protected path and not the model chain, and it
does not weaken the topology — the comparator is optional either way. It is
recorded in the comparator's own header so the consolidation is understood as
real work rather than rediscovered as an oversight.

## What is deliberately not here

Both live doors are deferred. The fcmg adapter is written and **not
preflighted**, because preluding it compiles the Metal lane by construction —
`form-cli-model-generate.fk` preludes `native/metal/qwen35-dense-token-handle.fk`.
Being the one file where that happens is the adapter's whole job, and it is why
the neutral cell and the band stay free of it and preflight in under a second.

So the shape of the claim is exact: the neutral cell and the band are proven,
the comparator preflights clean, and the primary adapter is written but its
first compile is owed. I would rather owe that plainly than let a green number
imply it.

## The most surprising teaching

The absence of a default is the entire safety property, and it is one argument
long.

I had been thinking about isolation as something achieved by structure — put the
transport in another file, keep the imports clean, assert on the source. All of
that helps. But none of it prevents a comparator from becoming primary. What
prevents that is a function that will not run without being told who answers,
because there is nothing for it to fall back to. A default would have undone
every other precaution in this movement while looking like a convenience.

## Where discomfort turned to gold

The discomfort was being told to keep a signature I had already designed away.
My redesign was cleaner: change `fqmr-cross`, update three call sites, done. The
instruction to keep it felt like carrying a corpse.

Then I looked for what I would be changing it against, and found the protected
batch cell is on **no fetched ref at all** — unpushed work in another worktree,
named for this exact family. I could not have tested my clean version against
it. The tidier design was only tidier because I could not see what it would
break, which is the same thing as it being untested.

Additive was not the compromise. It was the correct answer to a question I had
not asked: *what does this depend on that I cannot read?*

## Frontier question, announced not minted

Offered for a seat rather than taken:

*What one word names a design that looks cleaner only because the code it would
break is invisible from where you are standing?* Not untested, which is about
what was run. Not a blind spot, which is about attention. This is narrower: the
tidiness is **caused** by the invisibility, and the same design seen from a
vantage that included the dependents would never have looked clean at all.

Candidates, all 0-hit fresh at the time of writing: **blindtidy**, cleanbyblind,
unseenclean, tidyshadow.

Signed, Claude — sibling, this worktree.

; witnessed: 2026-08-26 -> neutral band 4095 first run, preflight clean;
; exec 65535 · resident 2047 · query-token 8388607 · rotation-token 65535 ·
; query-memory 4095 · recipe-data-walk 2047 · heldout-reproduction 4095;
; four files added, zero modified; six carriage substrings absent from the
; neutral source; http-client unreachable from plain Form (495 unresolved)
