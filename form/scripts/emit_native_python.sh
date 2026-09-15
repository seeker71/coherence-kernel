#!/usr/bin/env bash
# Emit the cached native-Python objects module on fkwu, and let the body's own
# Python grammar read it.
#
#   form-stdlib/form-ontology.json             the python.bmf categories
#     └─ form-stdlib/bml/ontology-emit.bml     read as (name, inst) pairs
#        └─ emits/python-native.fk             pn-emit-objects-module
#           └─ form/.cache/emit_native_python/python_bmf/objects.py
#              └─ form/scripts/python-page-read.bml   the body reads it as Python
#
# No Form literal of the category table lives in the emitter: the ontology is
# the source of truth, read where it stands. Every step runs on fkwu.
#
# Usage:
#   form/scripts/emit_native_python.sh

set -euo pipefail

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
cd "$REPO_ROOT"
[[ -x ./fkwu ]] || { echo "the body's kernel is missing: ./fkwu (cc -O2 -o fkwu runtime/fkwu-uni.c)" >&2; exit 1; }

echo "Step 1: the Form-native emitter writes the module..." >&2
./fkwu form/scripts/emit-native-python-objects.bml

echo "Step 2: the body's own Python grammar reads the page..." >&2
printf '%s\n' form/.cache/emit_native_python/python_bmf/objects.py > form/.cache/emit_native_python/objects-path
./fkwu form/scripts/python-page-read.bml < form/.cache/emit_native_python/objects-path
