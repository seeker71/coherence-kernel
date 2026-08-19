#!/usr/bin/env bash
# form_cli_table_validator_test.sh — the bootstrap table must bind a callable
# graph, not merely have a parseable numeric container.
set -euo pipefail

cd "$(dirname "$0")/.."
# shellcheck source=scripts/form_cli_bootstrap_proof.sh
source scripts/form_cli_bootstrap_proof.sh

work_dir="$(mktemp -d "${TMPDIR:-/tmp}/form-cli-table-validator.XXXXXX")"
cleanup() {
    rm -rf "$work_dir"
}
trap cleanup EXIT INT TERM

valid="$work_dir/valid.txt"
bad_root="$work_dir/bad-root.txt"
bad_call="$work_dir/bad-call.txt"

# one function rooted at node 0; one inert node; one empty string entry.
printf '%s\n' '1 0 1 0 0 0 0 1 0' > "$valid"
# The numeric framing is valid, but the only root is outside its node range.
printf '%s\n' '1 1 1 0 0 0 0 1 0' > "$bad_root"
# Tag 12 is a direct function call; function index 1 is outside [0, 1).
printf '%s\n' '1 0 1 12 1 0 0 1 0' > "$bad_call"

form_cli_validate_table "$valid" >/dev/null

for case_file in "$bad_root" "$bad_call"; do
    if form_cli_validate_table "$case_file" >/dev/null 2>&1; then
        printf 'form_cli_table_validator_test: malformed callable graph accepted: %s\n' "$case_file" >&2
        exit 1
    fi
done

printf '%s\n' 'form_cli_table_validator_test: PASS'
