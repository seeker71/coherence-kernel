#!/usr/bin/env sh
# Physical shell transport to the Form-owned JIT witness.
set -eu
cd "$(dirname "$0")/../.."
exec ./fkwu observe/native-jit-witness-run.fk
