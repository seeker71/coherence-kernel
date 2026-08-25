# Public query-offer batch: first physical surface

The committed dormant driver at `eccc7482` ran all 15 public Form families
through one local Qwen3.8-27B Q8_0 weights residence.  Each family used a fresh
stream state and an answer-free, exact source-bound query offer.

```text
plan-sha256=439e49a1ed6c5980618f998f77229a5f25ba0d74f1030e3018ebc13a887486cb
run-id=ec4ef867cd9048cab9990a73ad60ab6c451e154f9dbb8234521362f26a183e41
residence-id=e584d16c76aa516130bbab35a4fc04c505655bac46f0b4309fd40256fcb45e95
opens=1
reopens=0
release-ok=1
complete=1
ready=0
result: families=15 structural=1 transport=14 answer95=10
        heldout-credit=0 supervised-rag-credit=14 local=15 remote=0
@form fkwu 0 45803 0 45803
```

`complete=1` means the physical set is whole and structurally valid.  `ready=0`
means one supervised transport did not complete; it is not a failed run and it
is not canonical mastery.

| Family | Transport | Answer ≥95% | Raw SHA-256 |
|---|---:|---:|---|
| receipts | 1 | 1 | `2c980c59337fc70120233e4d839ed634e2e41f291a2f74ab2ba54c7daf22870f` |
| learn | 1 | 1 | `567e9ab7bd300d2b5c798c67551cb10f794e594b2a8e15527121da1b033388e8` |
| teachings | 1 | 0 | `3cd10351ab122a6766cbeab9b5a539f9f8dcf96821399605f3369f7873299924` |
| presence | 1 | 1 | `bccf445891ca7291dd24fff5366bda021fa50aba83b0e39febdee99314fe0de3` |
| grammars | 0 | 0 | `b44d2491aa8a6d24aee634543438a80091b08935de37e904eae4516798dbf217` |
| proof | 1 | 1 | `7ab7445d8dbe1795b0bb70a76bc3d1fc513e97fa74bb1e59f208107f0127a69e` |
| docs | 1 | 1 | `32231e5a0b2d23d03163d74439209627ed8ce4ccb9b6414371fd542cedc95858` |
| model | 1 | 1 | `fa414daefcada4641fd69cc24ccb51201cead6d9bac801948d92bc5c3011458d` |
| form-stdlib | 1 | 0 | `f8a36c5b4c0ad9cd83de47a1f7977f51a1a5159713c105d2770c01b1efe75114` |
| observe | 1 | 1 | `c3f4d47ef9691dbf808e909bea3e4061d8652f79aef4243146f1d88a53d8a8e0` |
| ingest | 1 | 0 | `4364bd54540e64d250312949c358ccebd91c5e66f0d80a7f6b1307b10ed39478` |
| axioms | 1 | 1 | `ee92f3bc7240bf837cb95275d20d713165de566d29e144848936c0ddfe440068` |
| control | 1 | 1 | `1db2a6672a4daf0a919f1912b58af78756930e7fdcd75a0a981d6af3727289e4` |
| bootstrap | 1 | 1 | `176153f3377521bbe5393d34a4b5828e84ad3c215d06d29e36070712a2569c45` |
| cognition | 1 | 0 | `a578a8208520bdd6f88a718242b75a655162daa889adf52c76e2cd1d004566ba` |

The grammar row exposed the hidden next path.  Its first offered query hit and
injected the exact source.  Qwen then emitted a second, more discriminative
query, `gl-register duplicate names`.  The current-source cursor held only one
lookup credit, so the second frame was recorded as `spent`; transport remained
0.  This is a bounded-recursive-query work order, not a reason to suppress the
model's refinement.

The other four misses were answer-quality observations after successful
transport: teachings 750000 ppm, form-stdlib 333333 ppm, ingest 0 ppm, and
cognition 500000 ppm.  They remain candidates for source-window/teaching
refinement after the transport mechanism can carry the grammar model's second
question.

Canonical mastery remains 0/15.  This entire run is supervised RAG.

— Codex, 2026-08-25
