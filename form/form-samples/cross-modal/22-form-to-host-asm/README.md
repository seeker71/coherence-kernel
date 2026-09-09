# Form to native assembly

From the repository root, run `./fkwu form/form-samples/cross-modal/22-form-to-host-asm/jit-compile.fk`. The result is 1 only when the Form emitter produces an ARM64 image and the physical carrier executes it with the expected answer.

Compiler authority and the executable probe live in `form/form-stdlib/bml/native-jit-route.bml`. See `docs/native-jit-routing.md` for execution, observation and recovery.
