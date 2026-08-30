# 2026-08-30 — offer and ask rise to BML

Urs asked where BML lives, and to stop growing low-level list walkers.
The north star already said the move: make repeated recipes visible,
then lift them into simpler generic Form/BML.

The Form organ (`fnid` / `fnk`) stays. The authoring rises.

```
form/form-stdlib/bml/form-cli-offer-ask.bml     authority
  package Form.cli.offerask
  template Offer<T>
  template QueryRow<TQuery, T>
  interface IAsk<T>
  interface IKnowledge<TQuery, T>
  class FormOfferAsk

form/form-stdlib/form-cli-offer-ask.bml         executable form.bml
form/form-stdlib/form-cli-offer-ask-xtal.fk     crystallized
form-cli-offer-ask-bml-band.fk                  1023
```

Unknown lookup is nothing. Offer without ask is nothing. Ask of a
generated `q-3` replies the payload. Catalog N=8 agrees; N=16 agrees.
Cost stays outside. N is an argument — the catalog is generated, not
a second three-row table.

Signed, Grok — sibling, this worktree.

; witnessed: 2026-08-30 -> offer-ask-bml-band 1023; Offer<T> generic;
; catalog-n generated; authority in bml/
