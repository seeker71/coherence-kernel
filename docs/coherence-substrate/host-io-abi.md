# Host I/O ABI And Membrane

Host I/O has three layers. The semantic class authority is
`form/form-stdlib/bml/host-io.bml`; the effect-vocabulary grammar authority is
`grammars/host-effect-vocabulary.bmf`, executable through
`form/form-stdlib/host-effect-grammar.fk`. The remaining `.fk` files are
lowered backing.

1. Runtime ABI registrations are portable carrier calls. Use names shaped like `host_file_read_text`, `host_file_write_text`, `host_file_read_slice`, `host_file_append_bytes`, `host_file_size`, `host_file_mtime`, `host_path_exists`, `host_path_remove`, `host_path_rename`, `host_path_is_dir`, `host_dir_mkdir`, `host_dir_rmdir`, `host_dir_list`, and `host_temp_dir`.
2. The Form membrane lives in `form/form-stdlib/form-fs.fk`. Its raw call sites are `host-abi-*`; its reusable door is `host-fs-*`.
3. Normal stdlib code uses `fs-read-text`, `fs-write-text`, `fs-read-slice`, `fs-list`, `fs-temp-dir`, and the rest of the `fs-*` vocabulary.

`read_file`, `write_file`, `write_file_text`, and the older `fs_*` native names are compatibility aliases while existing source is migrated. `write_file_text` is not a separate concept; it is the text variant of `write_file`, implemented over `host_file_write_text`.

When adding or renaming a host ABI operation, update all proof surfaces together:

- `runtime/fkwu-optable.h`
- `grammars/host-effect-vocabulary.bmf`
- `form/form-stdlib/host-effect-grammar.fk`
- `flatten/form-flatten.fk`
- `form/form-stdlib/native-op-manifest.fk`
- `form/form-stdlib/form-flatten.fk`
- Go, Rust, and TypeScript sibling kernel native registrations
- `form/form-stdlib/form-fs.fk` membrane wrappers
- `form/form-stdlib/tests/host-effect-grammar-band.fk`

Only membrane modules, raw ABI tests, or carrier implementations should call `host_*` directly. Application, stdlib, grammar, model, learning, and presence code should use `fs-*` or a domain-specific wrapper built on `fs-*`.
