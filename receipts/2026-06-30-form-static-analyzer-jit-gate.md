# Receipt - Form static analyzer becomes the pre-execution and pre-JIT gate (2026-06-30)

## What landed

The Form-native JIT track now has a compile-time lint/static analyzer gate. It
does not replace runtime checks; it rejects what can be proven unsafe before
execution or JIT lowering.

New files:

- `observe/form-static-analyzer.fk`
- `observe/tests/form-static-analyzer-band.fk`

The analyzer currently works over a small lowered Form AST row shape:

```text
("int" value source)
("nothing" 0 source)
("var" name source)
("list" items source)
("call" name args source)
```

It emits issue rows:

```text
("issue" code severity source detail)
```

The first codes are:

```text
201 missing source attribution
202 unbound symbol
203 arity mismatch
204 literal div/mod by zero
205 literal bounds failure
206 static null/empty dereference
```

## Witness

Run:

```sh
( cat observe/jit-runtime-fault.fk observe/form-static-analyzer.fk observe/tests/form-static-analyzer-band.fk ) > /tmp/fsa.fk
./fkwu --src /tmp/fsa.fk
```

Observed:

```text
1023
```

Meaning:

- `1`: valid attributed expression is allowed for execute/JIT.
- `2`: unbound symbol is caught.
- `4`: arity mismatch is caught.
- `8`: literal div by zero is caught.
- `16`: literal mod by zero is caught.
- `32`: literal list `nth` out-of-bounds is caught.
- `64`: `head`/`tail` over a statically empty list is caught.
- `128`: missing source attribution is caught.
- `256`: unsafe expression is blocked before execute.
- `512`: unsafe expression is blocked before JIT lowering.

## Honest scope

This is the first analyzer core, not yet the parser-integrated compiler pass.
The next step is to have the source parser/compiler feed this AST shape before
`fkwu --src` execution and before any Form-native JIT lowering. Runtime checks
remain required for facts that static analysis cannot prove.
