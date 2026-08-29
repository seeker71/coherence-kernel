# Temporary source-loader dependency growth repair

## Floor

`runtime/fkwu-uni.c` used a fixed `FK_SRC_DEP_CAP` of 128 for temporary
source-loader metadata. An expanded but otherwise valid Form closure therefore
failed before the source/JIT evaluator could parse it. This is bootstrap
bookkeeping, not Form meaning, and it was an ungrounded ceiling.

## Repair

The collector now starts on the heap and grows by doubling only as an observed
source graph requires it. Allocation-size checks run before converting the
growth count to allocator width. Growth allocates all six metadata arrays,
copies the held collector, then swaps only after every allocation succeeds; a
failed allocation leaves the previous collector intact. Speculative import
compiles snapshot and restore exactly the observed dependency count, including
content digests. `fk_src_dep_release` frees the collector on both threaded and
fallback process exits, whether source compilation succeeded or returned an
error.

## Focused witness

`runtime/tests/fkwu-source-dependency-growth.sh` builds a disposable linear
Form graph of 160 dependencies and runs the current `fkwu` against its root.
That crosses the old 128 floor and the new 32 -> 64 -> 128 -> 256 allocations:

```
sh runtime/tests/fkwu-source-dependency-growth.sh ./fkwu
-> fkwu source dependency growth: 160 dependencies -> 4242
```

The existing expanded direct-source contribution closure also now reaches the
fourth arm:

```
cd form && ./validate.sh form-stdlib/tests/form-cli-peer-contribution-turnwheel-band.fk
-> 1048575, four-way
```

## Shrink obligation

This is a short-lived checkout-witness repair in the C seed. It exists only
until the native Form source loader owns source-graph collection and source
identity itself. At that crossing, delete this heap collector and preserve this
160-dependency witness against the native implementation. No new language
meaning, primitive, operation table, or policy authority was added here.

The surprising teaching is that a 128-row C array can silently become a claim
about the size of the Form body. The discomfort was seeing a valid direct-source
closure fail at that border; it turned into a named, exercised shrink target
rather than a larger hidden ceiling.
