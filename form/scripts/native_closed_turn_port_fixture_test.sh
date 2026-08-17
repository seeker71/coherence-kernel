#!/usr/bin/env zsh
# Standalone, temp-only witness for the Form-emitted closed typed turn-port
# fixture. The shell is only a test harness: the carrier itself receives only
# a closed verb/id argv and never launches a shell, Codex, a model, or a
# network client.

set -eu

SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
REPO_ROOT="$(CDPATH= cd -- "$SCRIPT_DIR/../.." && pwd)"
EMITTER="$REPO_ROOT/form/form-stdlib/native-closed-turn-port-fixture.fk"
WORK_DIR="$(mktemp -d /tmp/form-closed-turn-port.XXXXXXXX)"
MAILBOX_ROOT="$WORK_DIR/mailbox"
WITNESS_ROOT="$WORK_DIR/witness"
PORT="$WORK_DIR/closed-turn-port"
PORT_CLOSED_FAIL="$WORK_DIR/closed-turn-port-closed-fail"
UID_NOW="$(/usr/bin/id -u)"
ID_ONE="0123456789abcdef0123456789abcdef0123456789abcdef0123456789abcdef"
ID_TWO="1111111111111111111111111111111111111111111111111111111111111111"
ID_PENDING_TARGET="2222222222222222222222222222222222222222222222222222222222222222"
ID_CLOSED_TARGET="3333333333333333333333333333333333333333333333333333333333333333"
ID_UNEXPECTED="4444444444444444444444444444444444444444444444444444444444444444"
ID_MAILBOX_SYMLINK="5555555555555555555555555555555555555555555555555555555555555555"
ID_MODE="6666666666666666666666666666666666666666666666666666666666666666"
ID_ROOT_MODE="7777777777777777777777777777777777777777777777777777777777777777"
ID_ROOT_SYMLINK="8888888888888888888888888888888888888888888888888888888888888888"
ID_UPPER="AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"

cleanup() {
    /bin/rm -rf "$WORK_DIR"
    /bin/rm -f "$REPO_ROOT/form/form-stdlib/native-closed-turn-port-fixture.fkb" \
        "$REPO_ROOT/form/form-stdlib/native-closed-turn-port-fixture.sym"
}
trap cleanup EXIT HUP INT TERM

fail() {
    printf 'native_closed_turn_port_fixture_test: %s\n' "$*" >&2
    exit 1
}

require_line() {
    expected="$1"
    value="$2"
    printf '%s\n' "$value" | /usr/bin/grep -Fqx "$expected" || fail "missing token $expected"
}

require_no_raw() {
    value="$1"
    case "$value" in
        *FORM_CLOSED_TURN_FIXTURE_*) fail 'carrier stdout retained raw fixture bytes' ;;
    esac
}

require_mode_owner() {
    path="$1"
    wanted_mode="$2"
    actual="$(/usr/bin/stat -f '%Lp:%u' "$path")"
    [ "$actual" = "$wanted_mode:$UID_NOW" ] || fail "unsafe mode/owner at $path: $actual"
}

compile_port() {
    output="$1"
    shift
    cc -std=c99 -Wall -Wextra -Werror \
        "-DFCTP_MAILBOX_ROOT=\"$MAILBOX_ROOT\"" \
        "-DFCTP_WITNESS_ROOT=\"$WITNESS_ROOT\"" \
        "$@" "$WORK_DIR/port.c" -o "$output"
}

"$REPO_ROOT/fkwu" "$EMITTER" > "$WORK_DIR/port.c"

for forbidden in 'system(' 'popen(' 'execve(' 'execvp(' 'execlp(' 'getenv(' 'getcwd(' 'socket(' 'connect(' 'getaddrinfo(' '/bin/sh' 'sh -c' 'codex'; do
    if /usr/bin/grep -Fq "$forbidden" "$WORK_DIR/port.c"; then
        fail "emitted source contains forbidden runtime route: $forbidden"
    fi
done

/usr/bin/grep -Fq 'if (argc != 3)' "$WORK_DIR/port.c" || fail 'exact closed argv gate missing'
/usr/bin/grep -Fq 'open(path, O_RDONLY | O_DIRECTORY | O_NOFOLLOW)' "$WORK_DIR/port.c" || fail 'root descriptor custody missing'
/usr/bin/grep -Fq 'openat(parent_fd, name, O_RDONLY | O_DIRECTORY | O_NOFOLLOW)' "$WORK_DIR/port.c" || fail 'exchange descriptor custody missing'
/usr/bin/grep -Fq 'fstat(fd, &st)' "$WORK_DIR/port.c" || fail 'descriptor owner/mode validation missing'
/usr/bin/grep -Fq 'fstatat(directory_fd, name, &st, AT_SYMLINK_NOFOLLOW)' "$WORK_DIR/port.c" || fail 'leaf nofollow validation missing'
/usr/bin/grep -Fq 'mkdirat(' "$WORK_DIR/port.c" || fail 'descriptor relative directory creation missing'
/usr/bin/grep -Fq 'openat(' "$WORK_DIR/port.c" || fail 'descriptor relative file access missing'
/usr/bin/grep -Fq 'unlinkat(' "$WORK_DIR/port.c" || fail 'descriptor relative closure missing'
/usr/bin/grep -Fq 'linkat(' "$WORK_DIR/port.c" || fail 'atomic descriptor witness publication missing'
/usr/bin/grep -Fq 'FCTP_SEAL_PENDING_TEMP' "$WORK_DIR/port.c" || fail 'closed temporary leaf enum missing'
/usr/bin/grep -Fq 'FCTP_WITNESS_PENDING' "$WORK_DIR/port.c" || fail 'pending witness enum missing'
/usr/bin/grep -Fq 'FCTP_STAGE_NATIVE_CAPABILITY' "$WORK_DIR/port.c" || fail 'fixed native-capability stage enum missing'
/usr/bin/grep -Fq 'capability_stage=native-pending-discovery' "$WORK_DIR/port.c" || fail 'pending-discovery stage evidence missing'
/usr/bin/grep -Fq 'decision=diagnostic-refusal-no-native-attestation' "$WORK_DIR/port.c" || fail 'no-attestation decision missing'
/usr/bin/grep -Fq 'raw_state=closure-pending' "$WORK_DIR/port.c" || fail 'conservative pending state missing'
if /usr/bin/grep -Fq 'lstat(' "$WORK_DIR/port.c"; then
    fail 'pathname lstat remains in emitted carrier'
fi

compile_port "$PORT"
compile_port "$PORT_CLOSED_FAIL" '-DFCTP_TEST_FAIL_CLOSED_LINK=1'

set +e
bad_argv="$("$PORT" open "$ID_ONE" extra)"
bad_argv_status=$?
bad_id="$("$PORT" open "not-a-canonical-id")"
bad_id_status=$?
bad_operation="$("$PORT" unbounded "$ID_ONE")"
bad_operation_status=$?
bad_native_capability="$("$PORT" native-capability "$ID_ONE")"
bad_native_capability_status=$?
bad_upper="$("$PORT" open "$ID_UPPER")"
bad_upper_status=$?
set -e
[ "$bad_argv_status" -eq 64 ] || fail "extra argv was accepted: $bad_argv_status"
[ "$bad_id_status" -eq 64 ] || fail "invalid id was accepted: $bad_id_status"
[ "$bad_operation_status" -eq 64 ] || fail "invalid operation was accepted: $bad_operation_status"
[ "$bad_native_capability_status" -eq 64 ] || fail "internal stage was caller-selectable: $bad_native_capability_status"
[ "$bad_upper_status" -eq 64 ] || fail "uppercase id was accepted: $bad_upper_status"
require_line 'decision=refused-closed-argv' "$bad_argv"
require_line 'decision=refused-invalid-exchange-id' "$bad_id"
require_line 'decision=refused-invalid-operation' "$bad_operation"
require_line 'decision=refused-invalid-operation' "$bad_native_capability"
require_line 'decision=refused-invalid-exchange-id' "$bad_upper"
require_no_raw "$bad_argv"
require_no_raw "$bad_id"
require_no_raw "$bad_operation"
require_no_raw "$bad_native_capability"
require_no_raw "$bad_upper"
[ ! -e "$MAILBOX_ROOT" ] && [ ! -e "$WITNESS_ROOT" ] || fail 'invalid invocation created a root'

/bin/mkdir "$WORK_DIR/root-target"
/bin/ln -s "$WORK_DIR/root-target" "$MAILBOX_ROOT"
set +e
root_symlink="$("$PORT" open "$ID_ROOT_SYMLINK")"
root_symlink_status=$?
set -e
[ "$root_symlink_status" -eq 77 ] || fail "root symlink was accepted: $root_symlink_status"
require_line 'decision=refused-unsafe-mailbox-or-witness' "$root_symlink"
require_no_raw "$root_symlink"
[ ! -e "$WORK_DIR/root-target/$ID_ROOT_SYMLINK" ] || fail 'root symlink received an exchange'
/bin/rm "$MAILBOX_ROOT"

opened="$("$PORT" open "$ID_ONE")"
require_line 'schema=closed-typed-turn-port-fixture-v1' "$opened"
require_line 'operation=open' "$opened"
require_line "exchange_id=$ID_ONE" "$opened"
require_line 'decision=opened-private-mailbox' "$opened"
require_line 'raw_state=private' "$opened"
require_no_raw "$opened"
require_mode_owner "$MAILBOX_ROOT" 700
require_mode_owner "$WITNESS_ROOT" 700
require_mode_owner "$MAILBOX_ROOT/$ID_ONE" 700
require_mode_owner "$WITNESS_ROOT/$ID_ONE" 700
require_mode_owner "$MAILBOX_ROOT/$ID_ONE/prompt.raw" 600
require_mode_owner "$MAILBOX_ROOT/$ID_ONE/native-capability-stage.kv" 600
/usr/bin/grep -Fqx 'capability_stage=native-pending-discovery' "$MAILBOX_ROOT/$ID_ONE/native-capability-stage.kv" || fail 'internal stage missed pending-discovery evidence'
/usr/bin/grep -Fqx 'decision=diagnostic-refusal-no-native-attestation' "$MAILBOX_ROOT/$ID_ONE/native-capability-stage.kv" || fail 'internal stage claimed unattested native use'
/usr/bin/grep -Fqx 'raw_state=private' "$MAILBOX_ROOT/$ID_ONE/native-capability-stage.kv" || fail 'internal stage missed private raw state'
if /usr/bin/grep -Fq 'FORM_CLOSED_TURN' "$MAILBOX_ROOT/$ID_ONE/native-capability-stage.kv"; then
    fail 'internal stage retained raw fixture bytes'
fi
[ ! -e "$MAILBOX_ROOT/$ID_ONE/response.raw" ] || fail 'internal stage created a raw response'

ran="$("$PORT" run "$ID_ONE")"
require_line 'operation=run' "$ran"
require_line 'decision=fixture-run-complete' "$ran"
require_line 'raw_state=private' "$ran"
require_no_raw "$ran"
require_mode_owner "$MAILBOX_ROOT/$ID_ONE/response.raw" 600
require_mode_owner "$MAILBOX_ROOT/$ID_ONE/run-stage.kv" 600

sealed="$("$PORT" seal "$ID_ONE")"
require_line 'operation=seal' "$sealed"
require_line 'decision=sealed-raw-closed' "$sealed"
require_line 'raw_state=closed' "$sealed"
require_no_raw "$sealed"
[ ! -e "$MAILBOX_ROOT/$ID_ONE/prompt.raw" ] || fail 'seal retained raw prompt'
[ ! -e "$MAILBOX_ROOT/$ID_ONE/response.raw" ] || fail 'seal retained raw response'
[ -f "$WITNESS_ROOT/$ID_ONE/manifest.kv" ] || fail 'seal did not publish closed witness'
[ ! -e "$WITNESS_ROOT/$ID_ONE/closure-pending.manifest.kv" ] || fail 'normal seal retained pending witness'
[ ! -e "$MAILBOX_ROOT/$ID_ONE/.seal-pending.tmp" ] || fail 'normal seal retained pending temporary'
[ ! -e "$MAILBOX_ROOT/$ID_ONE/.seal-closed.tmp" ] || fail 'normal seal retained closed temporary'
require_mode_owner "$WITNESS_ROOT/$ID_ONE/manifest.kv" 600
/usr/bin/grep -Fq 'raw_state=closed' "$WITNESS_ROOT/$ID_ONE/manifest.kv" || fail 'closed witness missed raw closure'
/usr/bin/grep -Fq 'response_fnv1a64=' "$WITNESS_ROOT/$ID_ONE/manifest.kv" || fail 'closed witness missed response identity'
if /usr/bin/grep -Fq 'FORM_CLOSED_TURN' "$WITNESS_ROOT/$ID_ONE/manifest.kv"; then
    fail 'closed witness retained raw fixture bytes'
fi

discarded="$("$PORT" discard "$ID_ONE")"
require_line 'operation=discard' "$discarded"
require_line 'decision=discarded-private-mailbox' "$discarded"
require_no_raw "$discarded"
[ ! -e "$MAILBOX_ROOT/$ID_ONE" ] || fail 'discard retained sealed mailbox'
[ -f "$WITNESS_ROOT/$ID_ONE/manifest.kv" ] || fail 'discard erased sealed witness'

"$PORT" open "$ID_TWO" >/dev/null
"$PORT" run "$ID_TWO" >/dev/null
"$PORT" discard "$ID_TWO" >/dev/null
[ ! -e "$MAILBOX_ROOT/$ID_TWO" ] || fail 'discard did not close unsealed mailbox'
[ ! -e "$WITNESS_ROOT/$ID_TWO/manifest.kv" ] || fail 'discard published an unsealed witness'
[ ! -e "$WITNESS_ROOT/$ID_TWO/closure-pending.manifest.kv" ] || fail 'discard retained an unsealed pending witness'

# A malformed pending target refuses before raw closure. Removing it permits a
# fresh seal retry on the same private raw mailbox.
"$PORT" open "$ID_PENDING_TARGET" >/dev/null
"$PORT" run "$ID_PENDING_TARGET" >/dev/null
/bin/ln -s "$WORK_DIR/root-target" "$WITNESS_ROOT/$ID_PENDING_TARGET/closure-pending.manifest.kv"
set +e
pending_target="$("$PORT" seal "$ID_PENDING_TARGET")"
pending_target_status=$?
set -e
[ "$pending_target_status" -eq 77 ] || fail "malformed pending target was accepted: $pending_target_status"
require_line 'decision=refused-unsafe-or-unpublishable-witness' "$pending_target"
require_no_raw "$pending_target"
[ -f "$MAILBOX_ROOT/$ID_PENDING_TARGET/prompt.raw" ] || fail 'malformed pending target closed raw prompt'
[ -f "$MAILBOX_ROOT/$ID_PENDING_TARGET/response.raw" ] || fail 'malformed pending target closed raw response'
/bin/rm "$WITNESS_ROOT/$ID_PENDING_TARGET/closure-pending.manifest.kv"
pending_retry="$("$PORT" seal "$ID_PENDING_TARGET")"
require_line 'decision=sealed-raw-closed' "$pending_retry"
require_no_raw "$pending_retry"

# An unknown mailbox leaf is refused before seal makes a pending receipt or
# closes raw. Once removed, the same raw exchange can seal normally.
"$PORT" open "$ID_UNEXPECTED" >/dev/null
"$PORT" run "$ID_UNEXPECTED" >/dev/null
printf '%s\n' 'synthetic-unexpected-leaf' > "$MAILBOX_ROOT/$ID_UNEXPECTED/not-a-closed-leaf"
set +e
unexpected_leaf="$("$PORT" seal "$ID_UNEXPECTED")"
unexpected_leaf_status=$?
set -e
[ "$unexpected_leaf_status" -eq 77 ] || fail "unexpected leaf was accepted: $unexpected_leaf_status"
require_line 'decision=refused-unsafe-or-unpublishable-witness' "$unexpected_leaf"
require_no_raw "$unexpected_leaf"
[ -f "$MAILBOX_ROOT/$ID_UNEXPECTED/prompt.raw" ] || fail 'unexpected leaf closed raw prompt'
[ -f "$MAILBOX_ROOT/$ID_UNEXPECTED/response.raw" ] || fail 'unexpected leaf closed raw response'
[ ! -e "$WITNESS_ROOT/$ID_UNEXPECTED/closure-pending.manifest.kv" ] || fail 'unexpected leaf published a pending witness'
/bin/rm "$MAILBOX_ROOT/$ID_UNEXPECTED/not-a-closed-leaf"
unexpected_retry="$("$PORT" seal "$ID_UNEXPECTED")"
require_line 'decision=sealed-raw-closed' "$unexpected_retry"
require_no_raw "$unexpected_retry"

# The fault-only temporary build forces final publication to fail *after* the
# pending witness is atomically published and raw leaves close. The normal
# temporary build must then promote that pending receipt on retry.
"$PORT" open "$ID_CLOSED_TARGET" >/dev/null
"$PORT" run "$ID_CLOSED_TARGET" >/dev/null
set +e
closed_publish_failure="$("$PORT_CLOSED_FAIL" seal "$ID_CLOSED_TARGET")"
closed_publish_failure_status=$?
set -e
[ "$closed_publish_failure_status" -eq 75 ] || fail "forced final publication failure had status $closed_publish_failure_status"
require_line 'decision=closure-pending-retry-required' "$closed_publish_failure"
require_line 'raw_state=closure-pending' "$closed_publish_failure"
require_no_raw "$closed_publish_failure"
[ ! -e "$MAILBOX_ROOT/$ID_CLOSED_TARGET/prompt.raw" ] || fail 'post-pending failure retained raw prompt'
[ ! -e "$MAILBOX_ROOT/$ID_CLOSED_TARGET/response.raw" ] || fail 'post-pending failure retained raw response'
[ -f "$WITNESS_ROOT/$ID_CLOSED_TARGET/closure-pending.manifest.kv" ] || fail 'post-pending failure lost recoverable pending witness'
[ ! -e "$WITNESS_ROOT/$ID_CLOSED_TARGET/manifest.kv" ] || fail 'post-pending failure claimed a closed witness'
/usr/bin/grep -Fq 'raw_state=closure-pending' "$WITNESS_ROOT/$ID_CLOSED_TARGET/closure-pending.manifest.kv" || fail 'pending receipt missed conservative state'
if /usr/bin/grep -Fq 'FORM_CLOSED_TURN' "$WITNESS_ROOT/$ID_CLOSED_TARGET/closure-pending.manifest.kv"; then
    fail 'pending receipt retained raw fixture bytes'
fi
closed_retry="$("$PORT" seal "$ID_CLOSED_TARGET")"
require_line 'decision=sealed-raw-closed' "$closed_retry"
require_no_raw "$closed_retry"
[ -f "$WITNESS_ROOT/$ID_CLOSED_TARGET/manifest.kv" ] || fail 'retry did not publish closed witness'
[ ! -e "$WITNESS_ROOT/$ID_CLOSED_TARGET/closure-pending.manifest.kv" ] || fail 'retry retained pending witness'

/bin/ln -s "$WORK_DIR/root-target" "$MAILBOX_ROOT/$ID_MAILBOX_SYMLINK"
set +e
mailbox_symlink="$("$PORT" open "$ID_MAILBOX_SYMLINK")"
mailbox_symlink_status=$?
set -e
[ "$mailbox_symlink_status" -eq 77 ] || fail "mailbox symlink was accepted: $mailbox_symlink_status"
require_line 'decision=refused-unsafe-mailbox-or-witness' "$mailbox_symlink"
require_no_raw "$mailbox_symlink"
[ ! -e "$WORK_DIR/root-target/prompt.raw" ] || fail 'mailbox symlink received a raw prompt'

/bin/mkdir "$MAILBOX_ROOT/$ID_MODE"
/bin/chmod 755 "$MAILBOX_ROOT/$ID_MODE"
set +e
unsafe_mode="$("$PORT" open "$ID_MODE")"
unsafe_mode_status=$?
set -e
[ "$unsafe_mode_status" -eq 77 ] || fail "unsafe exchange mode was accepted: $unsafe_mode_status"
require_line 'decision=refused-unsafe-mailbox-or-witness' "$unsafe_mode"
require_no_raw "$unsafe_mode"

/bin/chmod 755 "$MAILBOX_ROOT"
set +e
unsafe_root="$("$PORT" open "$ID_ROOT_MODE")"
unsafe_root_status=$?
set -e
[ "$unsafe_root_status" -eq 77 ] || fail "unsafe root mode was accepted: $unsafe_root_status"
require_line 'decision=refused-unsafe-mailbox-or-witness' "$unsafe_root"
require_no_raw "$unsafe_root"

printf '%s\n' 'native-closed-turn-port-fixture: pass (Form emitted; closed argv; fixed internal native-capability pending-discovery stage; descriptor-held private roots/exchanges; deterministic no-network child; pending-before-close witness; atomic closed promotion; raw closure/retry)'
