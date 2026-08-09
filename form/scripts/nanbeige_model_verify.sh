#!/bin/sh
# Thin host observer: sizes and hashes cross stdin; Form owns admission.

set -eu
. "$(CDPATH= cd "$(dirname "$0")" && pwd)/native_model_form_common.sh"

revision=5ff54fb7ed86ce8e216d78bff5417ab9981de3d4
model_dir=${NANBEIGE_MODEL_DIR:-"$HOME/.coherence-network/models/nanbeige4.2-3b/$revision"}
s1="$model_dir/model-00001-of-00002.safetensors"
s2="$model_dir/model-00002-of-00002.safetensors"
config="$model_dir/config.json"
tokenizer="$model_dir/tokenizer.model"
index="$model_dir/model.safetensors.index.json"

nm_require_command shasum
for path in "$s1" "$s2" "$config" "$tokenizer" "$index"; do
    if [ ! -f "$path" ]; then
        printf 'missing package file: %s\n' "$path" >&2
        exit 1
    fi
done

{
    printf '%s\n' "$revision"
    wc -c < "$s1" | tr -d ' '
    nm_sha256_file "$s1"
    wc -c < "$s2" | tr -d ' '
    nm_sha256_file "$s2"
    nm_sha256_file "$config"
    nm_sha256_file "$tokenizer"
    nm_sha256_file "$index"
} | "$NM_FKWU" form/form-stdlib/nanbeige-package-admission-cli.fk
