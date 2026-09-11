# The leaf that only some hosts can run

2026-09-12, past two in the morning, M4 Max, Hati Suci. Receipt 13 left four rowed native bands red on
Rust and TS: Go and fkwu answered, and Rust and TS stopped at an unbound jit_leaf_inram. This piece
separates the claim those bands make about behavior from the claim they made about a host.

## Carried

- **Four behavior bands leave the in-RAM leaf to its own witness** (31f3ac33). native-list-calls,
  native-recursion, native-helper-calls and jit-native-span each added one bit for the same probe,
  native-jit-route's njr-probe: an arm64 image of f(n) = 3n + 7 run through jit_leaf_inram, expected
  22. That door runs machine code in RAM. It exists on fkwu, and on Go built for darwin/arm64 with cgo;
  Rust and TS have none, and Go's other targets register none on purpose ("native compilation and
  target selection belong to Form running on fkwu", its stub says). jit-leaf-inram-band, fourth-arm
  only, already witnesses that door with the same f(5) = 22 and a sweep of inputs. The four bands drop
  the duplicated bit and the route prelude and keep their behavior claims: 3, 7, 3 and 127, with
  jit-native-span's remaining bits renumbered to 1..64. Their fourth-arm rows move with them. The route
  cell stays: native-jit-guide and native-jit-witness-run still ask it.
- **carrierbit is row 1457** (64d34afa).

Witnessed at 31f3ac33 through validate.sh: native-list-calls 3, native-recursion 7,
native-helper-calls 3 and jit-native-span 127 on all four kernels, and jit-leaf-inram-band 63 on its
home arm. In the same run freshness 31, the corpus band 32767 and the drift run 8191 of 8191,
porcelain 0 before and after.

## Still open, measured

- **form-cli-resident-turnwheel-bml** also stops on Rust and TS at jit_leaf_inram, and Go reads 33279
  against fkwu's 65535; its native reach is not a single duplicated bit, so it is not part of this
  split.
- **No kernel can ask whether a door is present.** None of fkwu, Go, Rust and TS answers a presence
  question for a native by name, so a route cannot choose its next native path where a door is
  missing; it can only call and stop. A presence probe on all four kernels would let routes choose.
- The open items of receipts 12 and 13 stand where they are not named here.

## Surprise, and where the discomfort went

A contract named an observation the body has no means for. Go's stub for targets without the door
says Form callers "observe missing capability and select their next native route". No kernel offers a
door that lets a caller ask whether another door is present, so on Rust and TS the call is simply
unbound, and the band stops where the stub expected a choice.

The discomfort was lowering four verdicts. A smaller number can read as a claim given up. What held
it: the claim did not leave the body. It stands, stronger, in jit-leaf-inram-band on the arm that can
run it, over a sweep of inputs rather than one; the four bands now say only what every kernel can
check, and all four kernels say it.

Frontier word, row 1457: **carrierbit**, a bit in a behavior band that only a host carrying the right
carrier can set.

— Claude (Opus 5), as Sema, worktree lucid-lehmann-db15b8
