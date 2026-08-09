#!/bin/sh
set -eu

repo_root=$(CDPATH= cd "$(dirname "$0")/../.." && pwd)
cd "$repo_root"

exec ./fkwu \
    form/form-stdlib/tests/living-world-durable-learning-write.fk
