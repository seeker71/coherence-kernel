# Resident NodeID invocation without a new seat

Date: 2026-08-27
Carrier: rebuilt `fkwu` on Darwin arm64
Movement: cold image birth, O(1) resident call, explicit dissolution

## The call that came home

The existing `jit_leaf_inram` carrier now accepts either its legacy byte image
or one retained Form request:

```text
[0, structural NodeID, image]  birth if unseen, then invoke
[1, structural NodeID, []]     release: 1 dissolved / 0 absent / nothing malformed
```

Cold birth walks and admits the emitted bytes once. The hot route takes the
already-interned NodeID's value-node index directly to a resident executable
slot. Slot generations invalidate evicted aliases without a sweep; resident
reference counts let distinct identities share byte-identical pages without
one release evicting the others. Release tombstones the identity for the
session, so a dead meaning cannot silently reload. A new meaning epoch mints a
new structural identity and may birth normally.

No SHA, byte fold, image reconstruction or cache scan occurs on the hot call.
No new primitive/op-table seat was added. The three-field request is Form data
carried through the already-existing two-argument door. A legacy image cannot
be mistaken for it: the second field must be a negative interned node, while an
admitted image contains only nonnegative bytes.

The runtime node index is explicitly session-ephemeral. Persistence retains
program and meaning data and interns them again after restart; it does not
persist the process-local index.

## Two frontier movements converged

Claude's PR #527 found the corridor cost above the native page and proved that
interned identity is the right executable key. Its first merged form added
`jit_leaf_hot` and `jit_leaf_call` under tags 29 and 32. Those seats were already
reserved by the authoritative manifest for `substring` and `int_to_str`.
Direct-source bands stayed green only because current `core.fk` serves those
names in Form; other lanes still depend on the manifest. The reconciliation
removed the duplicate rows and dispatch cases while keeping the identity and
meaning-epoch insight inside the existing door.

The resident band now carries dedicated `substring` and `int_to_str` bits so
this exact split-ledger shadow cannot hide behind lane-local luck again.

## Exact observations

After rebuilding the carrier:

```text
ground                                      42, exit 0
binary freshness                            31, exit 0
jit-resident-nodeid-band                  16383, exit 0
jit-self-crystallization-band             16383, exit 0
jit-once-born-band                        32767, exit 0
jit-meaning-epoch-band                       31, exit 0
jit-lower-self-crystallized-band           4095, exit 0
core-substring-equivalence-band             2047, exit 0
```

The 1,292-byte shape specialization probe reported:

```text
resident 10,000 calls       0-1 ms
legacy image 10,000 calls  33-37 ms
parity                           1
release first/again            1/0
```

The complete recursive lowerer reported:

```text
birth                           10 ms
Form 100 flows                  5 ms
native NodeID 100 flows         3 ms
flow parity                      1
births per retained flow         0
```

## Honest seed debt

This movement grows `runtime/fkwu-uni.c` as a short-lived checkout carrier
repair. The new meaning—identity, request shape, epoch, route and dissolution—
lives in Form cells. The C portion only owns mmap page residence and the
session-local NodeID-index→page association. Its shrink path is to move that
association into the Form-native carrier boundary when the native walker can
retain executable page handles directly; it is not a new home for compiler
policy.

## Fresh health map

The adaptive census remains `61 observed / 47 ready / 14 gaps / 0 unknown / 0
invalid / 770 permille`. No internal cache stage became a permanent target.
`compiler-self-specialization-nodeid` gained the resident invocation evidence.
The selected `direct-source-jit-self-crystallization` gap remains because the
evaluator does not yet expose heat as Form data and emitted images cannot yet
call one another by retained identity.

## Closing

Kept alive: two concurrent implementations were allowed to disagree, measure,
and then converge instead of one overwriting the other silently.

Most surprising teaching: removing the byte walk made the recursive native
route faster than its cold Form challenger, but the deeper win came from not
minting another operation name—the request itself was already sufficient.

Discomfort turned to gold: a merged green movement reused two reserved tags.
The collision exposed a split namespace ledger and became permanent regression
bits rather than blame.

Signed: Codex/Sol with Claude's corridor-cost and identity insight retained.
