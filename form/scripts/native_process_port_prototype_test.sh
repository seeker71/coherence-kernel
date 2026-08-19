#!/usr/bin/env zsh
# Compile and execute the isolated Form-emitted process-port witness.
#
# The test harness invokes the compiler and the produced binary, but the
# produced binary itself has no shell route: its self-test runs /bin/echo,
# /bin/cat, and /usr/bin/false through absolute argv plus pipes and waitpid.
# No network endpoint, client, or loopback service is contacted.

set -eu

SCRIPT_DIR="$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)"
REPO_ROOT="$(CDPATH= cd -- "$SCRIPT_DIR/../.." && pwd)"
EMITTER="$REPO_ROOT/form/form-stdlib/native-process-port-prototype.fk"
WORK_DIR="$(mktemp -d "${TMPDIR:-/tmp}/form-native-process-port.XXXXXXXX")"

cleanup() {
    /bin/rm -rf "$WORK_DIR"
}
trap cleanup EXIT HUP INT TERM

fail() {
    printf 'native_process_port_prototype_test: %s\n' "$*" >&2
    exit 1
}

"$REPO_ROOT/fkwu" "$EMITTER" > "$WORK_DIR/port.c"

for forbidden in 'system(' 'popen(' 'execvp(' 'execlp(' 'socket(' 'connect(' 'getaddrinfo(' '/bin/sh' 'sh -c'; do
    if rg -Fq "$forbidden" "$WORK_DIR/port.c"; then
        fail "emitted source contains forbidden route: $forbidden"
    fi
done

rg -Fq 'execve(argv[1], &argv[1], environ)' "$WORK_DIR/port.c" || fail 'direct absolute argv execve missing'
rg -Fq 'waitpid(child, &child_status' "$WORK_DIR/port.c" || fail 'waitpid status observation missing'
rg -Fq 'dup2(in_pipe[0], STDIN_FILENO)' "$WORK_DIR/port.c" || fail 'stdin pipe missing'
rg -Fq 'dup2(out_pipe[1], STDOUT_FILENO)' "$WORK_DIR/port.c" || fail 'stdout pipe missing'
rg -Fq 'dup2(err_pipe[1], STDERR_FILENO)' "$WORK_DIR/port.c" || fail 'stderr pipe missing'

cc -std=c99 -Wall -Wextra -Werror "$WORK_DIR/port.c" -o "$WORK_DIR/port"
cc -std=c99 -Wall -Wextra -Werror -DFNPP_SELFTEST "$WORK_DIR/port.c" -o "$WORK_DIR/port-selftest"
"$WORK_DIR/port-selftest" || fail 'native echo/cat/status self-test failed'

"$WORK_DIR/port" /bin/echo FORM_NATIVE_GENERIC_ARGV > "$WORK_DIR/echo.actual" || fail 'generic direct argv echo failed'
printf '%s\n' FORM_NATIVE_GENERIC_ARGV > "$WORK_DIR/echo.expected"
cmp -s "$WORK_DIR/echo.expected" "$WORK_DIR/echo.actual" || fail 'generic direct argv echo bytes changed'

printf '%s\n' FORM_NATIVE_GENERIC_STDIN | "$WORK_DIR/port" /bin/cat > "$WORK_DIR/stdin.actual" || fail 'generic stdin pipe cat failed'
printf '%s\n' FORM_NATIVE_GENERIC_STDIN > "$WORK_DIR/stdin.expected"
cmp -s "$WORK_DIR/stdin.expected" "$WORK_DIR/stdin.actual" || fail 'generic stdin pipe bytes changed'

set +e
"$WORK_DIR/port" /usr/bin/false
false_status=$?
set -e
[ "$false_status" -eq 1 ] || fail "generic waitpid status changed: $false_status"

set +e
"$WORK_DIR/port" echo FORM_NATIVE_PATH_LOOKUP > "$WORK_DIR/nonabsolute.actual"
nonabsolute_status=$?
set -e
[ "$nonabsolute_status" -eq 64 ] || fail "non-absolute argv was not refused: $nonabsolute_status"
[ ! -s "$WORK_DIR/nonabsolute.actual" ] || fail 'non-absolute argv produced output'

set +e
"$WORK_DIR/port" /bin/cat "$WORK_DIR/absent" > /dev/null 2> "$WORK_DIR/stderr.actual"
stderr_status=$?
set -e
[ "$stderr_status" -eq 1 ] || fail "generic stderr child status changed: $stderr_status"
[ -s "$WORK_DIR/stderr.actual" ] || fail 'generic stderr pipe carried no bytes'

printf '%s\n' 'native-process-port-prototype: pass (Form emitted; direct absolute argv; stdin/stdout/stderr pipes; waitpid status; no-network echo/cat fixture)'
