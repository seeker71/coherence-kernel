# Heedmark joins the fileless BML floor

## Crossing

The rebased kernel correctly removed derived `*-xtal.fk` files, while the
heedmark consumers still named one. The first fresh resident compile therefore
stopped at the missing artifact instead of reaching the BML floor.

`form-cli-heedmark.bml` now carries its own lowering dependencies:

```bml
// preludes: form-stdlib/core.fk form-stdlib/language-template.fk
```

The fileless BML runner carries that declaration into the in-memory lowered
source. Every heedmark consumer and its bands now prelude
`form-cli-heedmark.bml` directly; the old compile recipe that wrote
`form-cli-heedmark-xtal.fk` is gone. The only cache artifact is the BML image
owned by the runner, never a source-shaped mirror.

## Evidence

After rebuilding the committed bootstrap so the rebased fileless `.bml` lane
was actually in the process image:

- direct `./fkwu form/form-stdlib/form-cli-heedmark.bml` returned `0`;
- `form-cli-heedmark-band.fk` returned `1023`;
- `form-cli-heed-cursor-band.fk` returned `524287`;
- `form-cli-heed-twophase-band.fk` returned `65487`;
- `form-cli-heed-cursor-adversarial-band.fk` returned `2047`;
- the resident BML floor band again returned `65535`.

`rg` finds no Form source prelude referring to `form-cli-heedmark-xtal`.

## What the error gave back

The source freshness band did not reveal that the already-running local
bootstrap predated the rebased C seed; it still answered its own historical
check. Directly asking it to run a `.bml` named the missing capability. Rebuild
made the actual process-image boundary visible, and then the next error named
the absent BML dependency declaration precisely. Both errors became a direct
path into the BML source rather than a reason to restore an artifact.

— Codex, 2026-08-30

; witnessed: 2026-08-30 -> direct heedmark BML 0; heedmark 1023; cursor 524287; twophase 65487; adversarial 2047; resident BML 65535
