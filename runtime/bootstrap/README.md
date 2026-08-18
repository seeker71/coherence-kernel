# Direct-source Mac kernel

`fkwu-darwin-arm64` is the Apple-silicon checkout witness used by
`Sema Sessions.app`. Unlike the small fourth-arm proof walker under
`form/form-stdlib/bootstrap/`, this is the full direct-source kernel: it reads
and runs `.fk` source files, including the Sessions room.

It was built from this checkout with the repository's ordinary Darwin carrier:

```
cc -O2 -mmacosx-version-min=13.3 \
  -o fkwu-darwin-arm64 \
  runtime/fkwu-uni.c form/native/metal/fk-metal-carrier.m \
  -framework Metal -framework Foundation -fobjc-arc
```

Observed source identities on 2026-08-17:

```
c62fdab08475f5fbf936ace910af1d047e82b593c18f825fe6556a96ed876c1b  runtime/fkwu-uni.c
29ef41b8fdb92640c6a94cb23859083c1e80fd718fe397c0d4d3315a6458cf5b  form/native/metal/fk-metal-carrier.m
9480a7cc95ad5415264c850003207869a93aacbee05110c1508fddfee9f1ced6  runtime/bootstrap/fkwu-darwin-arm64
```

The Mach-O declares arm64 and macOS 13.3. It links only macOS system libraries
and the Metal, Foundation, and CoreFoundation frameworks. The committed binary
directly answered grounding `42`, recursion `55`, freshness `31`, and numeric
lists `[1, 2.5, [3, 4]]` before admission.
