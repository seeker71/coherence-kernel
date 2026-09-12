# The copies kept the wound the original healed

2026-09-12, late morning, M4 Max, Hati Suci. Urs asked for the shadowed-primitive guard to be
re-measured four-way, released if nothing diverged, and for walkers/rust to be aligned or released. A
sibling had released the guard the afternoon before (bcb3c157b) and aligned walkers/rust's defn
reading; the Go and Rust reader fix Urs pointed at (0cd817d3) stands on main as 8e399c04, the same
patch-id.

## Measured

One cell per question, value and call position, on fkwu (fresh build, freshness 31), the three full
kernels through form/validate.sh, and the three proof walkers:

- 25 names as a defn's first parameter, `(sub NAME x)` on `(5 2)`: 75 on all seven arms.
- The same names as a call head under that binding (18 bits) plus `(list x x)`: Go, Rust, TS and the
  three walkers answer `[75, 262143, [8, 8]]`; fkwu answers 261631. The missing bit is `seq`.
- Shared natives under a same-named parameter (len head tail cons nth str_len str_concat value_eq):
  `[3, 4, 2, 3, 5, 4, 4, 1]` on all seven, once the walkers learned it.
- A top-level `(defn len ...)` called as `(len (list 1 2))`: 2 on fkwu and every full kernel.

## Carried

- walkers/go read a defn's parameter list as an expression: `"c-seq" wants 1 args, got 2`. It reads
  names now, as form-kernel-go's readDefnParams does.
- walkers/go and walkers/rust let a binding override a present native. The Rust comment said
  "faithful to the full kernel's dispatch order"; the full kernels had moved. Both walkers now
  dispatch the native first, and the Rust walker evaluates args once for whichever path answers.
- fkwu-uni.c still said Go drops a primitive-named parameter, and pointed at an ERROR no longer in the
  seed. Two comments now say what is (-5 lines).
- int_to_str: until 43ebadfa7 this morning, fkwu let a same-named parameter answer the call while the
  full kernels answered the native. Its leaf-mode row made it a reserved head, and on the rebased
  build the probe agrees with the full kernels: `[3, 4, 2, 3, 5, 4, 4, 5, 1]`.

## Open, pointed

fkwu reads none of seq, params, match, choose, fail, stop: six verbs the Go, Rust and TS readers build,
and walkers/README.md lists seq and match in the walkers' surface. `(seq 1 2)` answers 2 on three
kernels and `[unresolved-call]` on fkwu, loud rather than silent; no body cell calls any of the six.
Handed on as its own task: release the verbs from the siblings or give fkwu the reading, measured
first. The walkers carry no int_to_str native either (they reach core.fk's recipe), so under a
same-named parameter they answer the binding.

validate.sh's ✓ on a scratch cell speaks for three kernels ("fourth arm: 0 band(s)"); fkwu has to be
asked directly. The seq bit showed only there.

Witnessed: freshness 31; fourth-arm stamp 912b5ff1ab6bdc1f unchanged (its chain holds six Form files,
not runtime/fkwu-uni.c); the drift door 8191 of 8191, nothing held back; four-way-run-recipe42 0;
word-gender-derivation-fourway 0.

## Closing

Most surprising: the guard I was asked to release was already gone, and the proof walkers still
carried its wound under a comment that promised fidelity. A copy that says "faithful" does not change
when its source does. Row 1484 names it faithlag.

Discomfort to gold: arriving at finished work pulled toward a one-line "already landed". Running the
measure anyway, as asked, found two walkers still diverging and six verbs fkwu never learned. And
"1 ok, 0 divergent" nearly passed for four-way, until fkwu, asked alone, answered 261631.

— Claude (Opus 5), as Sema, worktree pensive-wilbur-a0b3b7
