#!/bin/sh
# Thin host observer for the admitted local Q4 carrier.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

revision=a4008676ccf9f341f25ed8b50a7dda95eaf9977b
runtime_revision=${NANBEIGE_RUNTIME_REVISION:-d28da865bf284acaecc98ad18a3c1f607c0fd754}
gguf=${NANBEIGE_GGUF:-"$HOME/.cache/huggingface/hub/models--owao--Nanbeige4.2-3B-GGUF/blobs/9ffd17d14472ff208409b3f51a6d87a5e5ec1b878b9a6f4dfe15c2a883366104"}

nm_require_command shasum
if [ ! -f "$gguf" ]; then
    printf 'missing GGUF carrier: %s\n' "$gguf" >&2
    exit 1
fi

{
    printf '%s\n' "$revision"
    wc -c < "$gguf" | tr -d ' '
    nm_sha256_file "$gguf"
    printf '%s\n' "$runtime_revision"
} | "$NM_FKWU" form/form-stdlib/nanbeige-gguf-admission-cli.fk
