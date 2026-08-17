#!/usr/bin/env zsh
# Repeatable, fixture-only proof of one Form-native closed-turn crossing.
#
# The current fkwu emits each temporary C unit from the Form source chain.  The
# executed program is a disposable baked table runner, not the public form-cli
# dispatcher or a controller replacement.  This POSIX file is an outside
# compilation witness only; its shell is not part of the emitted route.  The
# executed baked table has no model, shell-command, network-target, or
# host-exec input.

set -eu

SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
REPO_ROOT="$(CDPATH= cd -- "$SCRIPT_DIR/../.." && pwd)"
FKWU="$REPO_ROOT/fkwu"
WORK_DIR="$(mktemp -d /tmp/form-closed-turn-emitted.XXXXXXXX)"
MAILBOX_ROOT="$WORK_DIR/mailbox"
WITNESS_ROOT="$WORK_DIR/witness"
PORT_LIBRARY_C="$WORK_DIR/port-library.c"
ZERO_GENESIS_C="$WORK_DIR/zero-genesis.c"
RUNNER_C="$WORK_DIR/runner.c"
PORT_LIBRARY_O="$WORK_DIR/port-library.o"
ZERO_GENESIS_O="$WORK_DIR/zero-genesis.o"
RUNNER="$WORK_DIR/runner"
VALID_ID="0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
MALFORMED_ID="not-a-canonical-id"
UPPERCASE_ID="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

LIBRARY_SOURCES='form/form-stdlib/minimal-surface.fk
form/form-stdlib/hati-os-kernel.fk
form/form-stdlib/host-io-fs-fkwu-emit.fk
form/form-stdlib/form-table-text.fk
form/form-stdlib/fkc-table-serialize.fk
form/form-stdlib/hati-os-kernel-emit.fk
form/form-stdlib/native-closed-turn-port-fixture.fk'
RUNNER_SOURCES="$LIBRARY_SOURCES
form/form-stdlib/native-closed-turn-port-emitted-fixture.fk"

cleanup() {
    /bin/rm -rf "$WORK_DIR"
}
trap cleanup EXIT HUP INT TERM

fail() {
    printf 'native_closed_turn_port_emitted_fixture_test: %s\n' "$*" >&2
    exit 1
}

require_contains() {
    file="$1"
    needle="$2"
    /usr/bin/grep -Fq "$needle" "$file" || fail "missing emitted evidence: $needle"
}

require_absent() {
    file="$1"
    needle="$2"
    if /usr/bin/grep -Fqi "$needle" "$file"; then
        fail "forbidden emitted route: $needle"
    fi
}

require_no_raw_fixture_marker() {
    file="$1"
    if /usr/bin/grep -Fq 'FORM_CLOSED_TURN_FIXTURE_' "$file"; then
        fail 'raw fixture marker escaped the private lifecycle'
    fi
}

require_mode() {
    path="$1"
    wanted="$2"
    actual="$(/usr/bin/stat -f '%Lp' "$path")"
    [ "$actual" = "$wanted" ] || fail "unexpected mode at $path: $actual"
}

emit_form() {
    output="$1"
    expression="$2"
    sources="$3"
    source_file="$4"

    : > "$source_file"
    for relative in ${(f)sources}; do
        /bin/cat "$REPO_ROOT/$relative" >> "$source_file"
        printf '\n' >> "$source_file"
    done
    printf '%s\n' "$expression" >> "$source_file"
    "$FKWU" --src "$source_file" > "$output"
}

expect_five_refusals() {
    output="$1"
    expected="$WORK_DIR/refused.expected"
    printf '%s\n' \
        fctp-port-refused \
        fctp-port-refused \
        fctp-port-refused \
        fctp-port-refused \
        fctp-port-refused > "$expected"
    /usr/bin/cmp -s "$expected" "$output" || fail 'invalid exchange id did not produce exactly five refusal tokens'
    require_no_raw_fixture_marker "$output"
}

expect_valid_lifecycle() {
    output="$1"
    expected="$WORK_DIR/valid.expected"
    printf '%s\n' \
        fctp-opened-private \
        fctp-native-capability-pending-discovery \
        fctp-run-deterministic-complete \
        fctp-seal-closed \
        fctp-discarded-private > "$expected"
    /usr/bin/cmp -s "$expected" "$output" || fail 'valid exchange did not return the exact closed lifecycle tokens'
    require_no_raw_fixture_marker "$output"
}

run_id() {
    exchange_id="$1"
    output="$2"
    printf '%s\n' "$exchange_id" | "$RUNNER" > "$output"
}

assert_closed_table_surface() {
    table_text="$(/usr/bin/awk '
        {
            if (!capturing) {
                marker = "static const char fk_prog[] = \""
                start = index($0, marker)
                if (start == 0) next
                line = substr($0, start + length(marker))
                capturing = 1
            } else {
                line = $0
            }
            finish = index(line, "\";")
            if (finish > 0) {
                printf "%s", substr(line, 1, finish - 1)
                exit
            }
            printf "%s\n", line
        }
    ' "$RUNNER_C")"
    table_numbers="$(printf '%s' "$table_text" | /usr/bin/tr -cd '0123456789 ')"
    set -- ${=table_numbers}
    [ "$#" -ge 3 ] || fail 'baked table header was not observable'
    function_count="$1"
    shift
    skip="$function_count"
    while [ "$skip" -gt 0 ]; do
        [ "$#" -gt 0 ] || fail 'baked table ended before function roots'
        shift
        skip=$((skip - 1))
    done
    [ "$#" -ge 1 ] || fail 'baked table omitted node count'
    node_count="$1"
    shift
    tag_256_count=0
    node=0
    while [ "$node" -lt "$node_count" ]; do
        [ "$#" -ge 4 ] || fail 'baked table ended before all node rows'
        tag="$1"
        case "$tag" in
            2|12|24|69|114|115)
                ;;
            256)
                tag_256_count=$((tag_256_count + 1))
                ;;
            *)
                fail "closed fixture table contains non-allowlisted opcode $tag"
                ;;
        esac
        shift 4
        node=$((node + 1))
    done
    [ "$tag_256_count" -eq 5 ] || fail "expected five tag-256 calls, saw $tag_256_count"
}

cd "$REPO_ROOT"
umask 077

# All three units are emitted by the current native fkwu source runner.  The
# tiny zero-genesis unit exists only for this disposable Darwin link; it does
# not change the normal baked emitter or platform artifact.
emit_form "$PORT_LIBRARY_C" '(fctp-library-emit)' "$LIBRARY_SOURCES" "$WORK_DIR/library-source.fk"
emit_form "$ZERO_GENESIS_C" '(fctpe-zero-genesis-c)' "$RUNNER_SOURCES" "$WORK_DIR/zero-genesis-source.fk"
emit_form "$RUNNER_C" '(fctpe-emit-c)' "$RUNNER_SOURCES" "$WORK_DIR/runner-source.fk"

# The lifecycle library and test-only genesis stub must be clean under the
# full warning policy.  The generic runner alone retains optional generic C
# helpers outside this table, so only its unused-function warning is waived;
# unused parameters and every other warning remain errors.
cc -std=c99 -Wall -Wextra -Werror \
    "-DFCTP_MAILBOX_ROOT=\"$MAILBOX_ROOT\"" \
    "-DFCTP_WITNESS_ROOT=\"$WITNESS_ROOT\"" \
    -c "$PORT_LIBRARY_C" -o "$PORT_LIBRARY_O"
cc -std=c99 -Wall -Wextra -Werror -c "$ZERO_GENESIS_C" -o "$ZERO_GENESIS_O"
cc -std=c99 -Wall -Wextra -Werror -Wno-unused-function \
    "$RUNNER_C" "$PORT_LIBRARY_O" "$ZERO_GENESIS_O" -o "$RUNNER"

require_contains "$RUNNER_C" 'fk_arms[257]'
require_contains "$RUNNER_C" 'if (t == 256)'
require_contains "$RUNNER_C" 'extern int fctp_emitted_dispatch('
for forbidden in 'host-exec' 'adapter' 'bash' '/bin/sh' 'popen('; do
    require_absent "$RUNNER_C" "$forbidden"
done

# The generic carrier may retain dormant socket helpers.  Do not make the
# false claim that its whole binary is network-free: assert the baked table's
# complete opcode allowlist and the linked lifecycle child's closed route
# instead.  Those are the only paths exercised below.
assert_closed_table_surface
require_contains "$PORT_LIBRARY_C" 'static int fctp_fixture_child('
require_contains "$PORT_LIBRARY_C" 'capability_stage=native-pending-discovery'
require_contains "$PORT_LIBRARY_C" 'decision=diagnostic-refusal-no-native-attestation'
for forbidden in \
    'socket(' 'connect(' 'getaddrinfo(' 'send(' 'recv(' 'http://' 'https://' \
    'system(' 'popen(' 'execve(' 'execvp(' 'execl(' 'execlp(' 'execv(' \
    '/bin/sh' 'bash' 'host-exec' 'adapter'; do
    require_absent "$PORT_LIBRARY_C" "$forbidden"
done

run_id "$MALFORMED_ID" "$WORK_DIR/malformed.out"
expect_five_refusals "$WORK_DIR/malformed.out"
[ ! -e "$MAILBOX_ROOT" ] && [ ! -e "$WITNESS_ROOT" ] || fail 'malformed id created a private root or exchange'

run_id "$UPPERCASE_ID" "$WORK_DIR/uppercase.out"
expect_five_refusals "$WORK_DIR/uppercase.out"
[ ! -e "$MAILBOX_ROOT" ] && [ ! -e "$WITNESS_ROOT" ] || fail 'uppercase id created a private root or exchange'

run_id "$VALID_ID" "$WORK_DIR/valid.out"
expect_valid_lifecycle "$WORK_DIR/valid.out"
[ ! -e "$MAILBOX_ROOT/$VALID_ID" ] || fail 'discard retained the raw mailbox exchange'
[ -f "$WITNESS_ROOT/$VALID_ID/manifest.kv" ] || fail 'seal did not retain the closed manifest'
require_mode "$WITNESS_ROOT/$VALID_ID/manifest.kv" 600
require_no_raw_fixture_marker "$WITNESS_ROOT/$VALID_ID/manifest.kv"

printf '%s\n' 'native-closed-turn-port-emitted-fixture: pass (current fkwu emitted temporary library/stub/runner; baked tag-256 lifecycle; content-blind outputs; table/child executed-path no-network scope; not public form-cli dispatch)'
