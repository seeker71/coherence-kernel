#!/bin/sh
# form_cli_table_capacity_diagnostic.sh — nonpublishing table-image witness.
#
# This never refreshes bootstrap artifacts.  It emits a disposable universal
# runner from the current Form source, builds it below a private temporary
# directory, and exercises three observations:
#
#   * a valid table whose live canary strings sit beyond the historical 1 MiB
#     prefix still loads completely and answers;
#   * a table beyond the named 16 MiB capacity is refused before parsing;
#   * a one-table image cut short of its final string rows is refused as
#     malformed rather than silently treated as a prefix.
#
# The late fixture is leading whitespace followed by the canonical source
# table byte-for-byte.  Whitespace is already legal before the first integer,
# so every semantic row — including the live canary — crosses the old one-read
# boundary without rewriting any pool index or changing the program.

set -eu
umask 077

SCRIPT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
FORM_DIR=$(CDPATH= cd -- "$SCRIPT_DIR/.." && pwd)
REPO_ROOT=$(CDPATH= cd -- "$FORM_DIR/.." && pwd)
RUST_KERNEL="$FORM_DIR/form-kernel-rust/target/release/form-kernel-rust"
SOURCE_TABLE="$FORM_DIR/form-stdlib/bootstrap/form-cli-table.txt"
CAPACITY_BYTES=16777216
LEGACY_PREFIX_BYTES=1048575
PADDING_BYTES=$((LEGACY_PREFIX_BYTES + 1))
TMP_BASE=${TMPDIR:-/private/tmp}
TMP=$(mktemp -d "$TMP_BASE/form-cli-table-capacity.XXXXXX")

cleanup() {
    rm -rf "$TMP"
}
trap cleanup 0 1 2 15

fail() {
    printf '%s\n' "form_cli_table_capacity_diagnostic: FAIL: $*" >&2
    exit 1
}

[ -x "$RUST_KERNEL" ] || fail "Rust source emitter is unavailable: $RUST_KERNEL"
[ -f "$SOURCE_TABLE" ] || fail "source table is unavailable: $SOURCE_TABLE"
command -v clang >/dev/null 2>&1 || fail 'clang is unavailable'

EMIT_REQUEST="$TMP/emit.fk"
UNIVERSAL_C="$TMP/universal.c"
RUNNER="$TMP/fkwu"
LATE_TABLE="$TMP/late-canary.tbl"
CANONICAL_TAIL="$TMP/canonical-tail.tbl"
CANARY_INPUT="$TMP/canary.in"
CANARY_OUT="$TMP/canary.out"
TRUNCATED_TABLE="$TMP/truncated.tbl"
OVERFLOW_TABLE="$TMP/overflow.tbl"
EXPECTED_OUT="$TMP/expected.out"

printf '%s\n' '(fkc-emit-universal)' > "$EMIT_REQUEST"
if ! "$RUST_KERNEL" \
        "$FORM_DIR/form-stdlib/minimal-surface.fk" \
        "$FORM_DIR/form-stdlib/hati-os-kernel.fk" \
        "$FORM_DIR/form-stdlib/host-io-fs-fkwu-emit.fk" \
        "$FORM_DIR/form-stdlib/form-table-text.fk" \
        "$FORM_DIR/form-stdlib/fkc-table-serialize.fk" \
        "$FORM_DIR/form-stdlib/hati-os-kernel-emit.fk" \
        "$EMIT_REQUEST" > "$UNIVERSAL_C" 2> "$TMP/emit.err"; then
    sed -n '1,20p' "$TMP/emit.err" >&2
    fail 'source emitter did not produce a universal runner'
fi

if ! clang -O2 -Wno-error=implicit-function-declaration \
        -Wno-implicit-function-declaration \
        -Wno-incompatible-library-redeclaration \
        -pthread -o "$RUNNER" "$UNIVERSAL_C" > "$TMP/clang.out" 2> "$TMP/clang.err"; then
    sed -n '1,20p' "$TMP/clang.err" >&2
    fail 'disposable universal runner did not compile'
fi

# The prefix is larger than the old loader's entire usable read.  `cat` appends
# the canonical table unchanged; the tail comparison makes that identity an
# observation rather than a claim.
[ "$PADDING_BYTES" -gt "$LEGACY_PREFIX_BYTES" ] || fail 'whitespace prefix did not cross legacy boundary'
[ "$PADDING_BYTES" -lt "$CAPACITY_BYTES" ] || fail 'whitespace prefix did not fit current capacity'
dd if=/dev/zero bs="$PADDING_BYTES" count=1 2>/dev/null | tr '\000' ' ' > "$LATE_TABLE"
cat "$SOURCE_TABLE" >> "$LATE_TABLE"

source_bytes=$(wc -c < "$SOURCE_TABLE" | tr -d '[:space:]')
late_bytes=$(wc -c < "$LATE_TABLE" | tr -d '[:space:]')
[ "$late_bytes" -eq $((PADDING_BYTES + source_bytes)) ] || fail 'late table size did not equal whitespace prefix plus canonical table'
[ "$late_bytes" -lt "$CAPACITY_BYTES" ] || fail 'late table did not stay below current capacity'
tail -c "$source_bytes" "$LATE_TABLE" > "$CANONICAL_TAIL"
cmp -s "$SOURCE_TABLE" "$CANONICAL_TAIL" || fail 'late table did not preserve canonical source bytes'

printf 'ping\n' > "$CANARY_INPUT"
if ! "$RUNNER" "$LATE_TABLE" 0 < "$CANARY_INPUT" > "$CANARY_OUT" 2> "$TMP/canary.err"; then
    sed -n '1,20p' "$TMP/canary.err" >&2
    sed -n '1,20p' "$CANARY_OUT" >&2
    fail 'late valid table did not execute'
fi
grep -Fqx 'pong' "$CANARY_OUT" || fail 'late valid table did not answer its canary'

# Remove a non-whitespace tail from a valid table.  The string count remains
# unchanged, so the shared parser must reach EOF while consuming string bytes.
truncated_bytes=$((late_bytes - 64))
[ "$truncated_bytes" -gt 0 ] || fail 'late table is too short for truncation observation'
head -c "$truncated_bytes" "$LATE_TABLE" > "$TRUNCATED_TABLE"
set +e
"$RUNNER" "$TRUNCATED_TABLE" 0 > "$TMP/truncated.out" 2> "$TMP/truncated.err"
truncated_status=$?
set -e
[ "$truncated_status" -eq 4 ] || fail "truncated table exit=$truncated_status, expected 4"
printf '%s\n' 'fkwu: malformed form table string length' > "$EXPECTED_OUT"
if ! cmp -s "$EXPECTED_OUT" "$TMP/truncated.out"; then
    sed -n '1,20p' "$TMP/truncated.err" >&2
    sed -n '1,20p' "$TMP/truncated.out" >&2
    fail 'truncated table did not emit exact malformed-table refusal'
fi

# This data is deliberately not a valid table: a capacity observation happens
# before parse, and must refuse the entire oversized image rather than accept a
# syntactically useful prefix.
cp "$LATE_TABLE" "$OVERFLOW_TABLE"
dd if=/dev/zero bs=1048576 count=16 2>/dev/null | tr '\000' '0' >> "$OVERFLOW_TABLE"
overflow_bytes=$(wc -c < "$OVERFLOW_TABLE" | tr -d '[:space:]')
[ "$overflow_bytes" -gt "$CAPACITY_BYTES" ] || fail 'overflow fixture did not cross current capacity'
set +e
"$RUNNER" "$OVERFLOW_TABLE" 0 > "$TMP/overflow.out" 2> "$TMP/overflow.err"
overflow_status=$?
set -e
[ "$overflow_status" -eq 3 ] || fail "overflow table exit=$overflow_status, expected 3"
printf '%s\n' 'fkwu: form table exceeds emitted buffer capacity' > "$EXPECTED_OUT"
cmp -s "$EXPECTED_OUT" "$TMP/overflow.out" || fail 'overflow table did not emit exact capacity refusal'

printf '%s\n' "form_cli_table_capacity_diagnostic: PASS source_table_bytes=$source_bytes leading_whitespace_bytes=$PADDING_BYTES late_table_bytes=$late_bytes canonical_tail_match=1 canary_verified=1 truncation_exit=$truncated_status overflow_bytes=$overflow_bytes overflow_exit=$overflow_status"
