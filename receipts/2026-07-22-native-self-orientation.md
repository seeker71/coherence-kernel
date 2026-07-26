# 2026-07-22 — floor, north star, walk one, re-orient

## Initial orientation

The live two-reader cognition cycle produced this floor:

```text
[22 rows, 18 native routes, 4 evidence routes, 17 correct native,
 1 wrong native, 126 inquiry replays, 126 expected, 44 frames]
```

The north-star invariants are computed as `[no observed false-native, inquiry
integrity, directional-frame integrity, unresolved work visible]`, initially
`[0,1,1,1]`.

`nso-next-two` selected movements from observed pressure, without a target row
count or accuracy threshold:

```text
[1001099101 diversify recognition, 1001099102 learn evidence routes]
```

## Walk one

The first movement executed the already witnessed representation-diverse
recognition walker. Its first nested run exposed a real diagnostic defect: the
child called `framebuffer-clear`, erasing the parent orientation frames. The
result contained 47 frames and `success 0`.

The child witness now accepts an inherited window and measures only the frames
it adds. Parent orientation, 44 child direction frames, and post-walk orientation
coexist as 49 events.

## Updated orientation

```text
./fkwu --src cognition/tests/native-self-orientation-band.fk
[nothing, 0, 1, 1001099011,
 [22,18,4,17,1,126,126,44], [0,1,1,1],
 [1001099101,1001099102], 1001099101, 1,
 [22,16,6,16,0,112,112,44], [1,1,1,1],
 [1001099102,1001099103], 49, 1]
```

The walked floor has 16/16 correct native responses, zero observed wrong native
responses, six visible evidence routes, 112/112 inquiry replays, and 44 child
frames. The next two movements changed to:

1. `1001099102` — learn from evidence-routed rows;
2. `1001099103` — re-assess recognition after learning.

That change was selected by the body: once wrong-native pressure became zero,
visible evidence debt became the leading pressure.

## Honest floor

The movement vocabulary and selection policy are authored Form, not a learned
meta-policy. Only `diversify recognition` has an executable walker today; the
other movement nodes honestly return `nothing` until built. This is native
self-orientation over live evidence, not yet open-ended self-programming.
