# 2026-08-17 — native-port-check command grammar

The closed command shape is now named as direct Form rather than existing only
inside the disposable emitted-table source string:

```
native-port-check <64 lowercase hexadecimal bytes>
```

`form/form-stdlib/native-port-check-command-grammar.fk` makes the prefix,
lengths, lowercase-hex identity rule, extraction, admission, and refusal
surface executable. Its band exercises accepted input and wrong command,
missing/double separator, uppercase, short, non-hex, extra-token, and newline
refusals. The direct structural mirror follows the emitted fixture's ordering:
length, prefix, then identity.

Observed on the source executor:

```
./fkwu form/form-stdlib/tests/native-port-check-command-grammar-band.fk
127
```

Both the grammar cell and its band preflighted clean: balanced, 0 errors,
0 warnings, 0 unresolved calls.

This is not an emitted-runner crossing, an fsh continuity claim, a public
form-cli command, or evidence of a port/resource/child/network action. Those
boundaries remain separate until a profile is explicitly wired and witnessed.

Signed: form_parser_structural

; witnessed: 2026-08-17 -> direct Form closed command grammar 127
