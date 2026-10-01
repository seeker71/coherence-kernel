//go:build darwin && arm64 && cgo

// jit_inram_darwin_arm64.go — the in-RAM JIT executor: run a Form-emitted
// arm64 leaf image from lo-compile-fn in-process. Form owns emission;
// this physical carrier only admits and executes the bytes.
//
// Apple Silicon enforces W^X: a page is never writable and executable at the
// same instant. MAP_JIT pages are the sanctioned exception — allocated once,
// then toggled per-thread between writable and executable via
// pthread_jit_write_protect_np. We write the image under write-protect-off,
// flip to executable, clear the i-cache for the range, call it as
// int64 f(int64), and unmap. The toggle is a libsystem call, so this file is
// cgo + darwin/arm64 only; every other target uses the no-op stub
// (jit_inram_other.go).

package main

/*
#include <sys/mman.h>
#include <pthread.h>
#include <string.h>

// form_run_leaf — map a MAP_JIT page, write the image while jit-write-protect
// is off, flip to executable, clear the instruction cache, call f(arg), unmap.
// *ok is 0 on mmap refusal (the result is then meaningless), 1 on a real call.
static long form_run_leaf(unsigned char *code, int n, long arg, int *ok) {
    *ok = 0;
    if (n <= 0 || n > 4096) return 0;
    void *mem = mmap(NULL, 4096, PROT_READ | PROT_WRITE | PROT_EXEC,
                     MAP_PRIVATE | MAP_ANON | MAP_JIT, -1, 0);
    if (mem == MAP_FAILED) return 0;
    pthread_jit_write_protect_np(0);            // page writable on this thread
    memcpy(mem, code, n);
    pthread_jit_write_protect_np(1);            // page executable on this thread
    __builtin___clear_cache((char *)mem, (char *)mem + n);
    long (*fn)(long) = (long (*)(long))mem;
    long r = fn(arg);
    munmap(mem, 4096);
    *ok = 1;
    return r;
}
*/
import "C"
import "unsafe"

// runLeafInRAM — execute an arm64 leaf image as int64 f(int64) in-process.
// Returns (result, true) on a real call, (0, false) if the input is out of
// range or the JIT page could not be mapped.
func runLeafInRAM(code []byte, arg int64) (int64, bool) {
	if len(code) == 0 || len(code) > 4096 {
		return 0, false
	}
	var ok C.int
	r := C.form_run_leaf(
		(*C.uchar)(unsafe.Pointer(&code[0])),
		C.int(len(code)),
		C.long(arg),
		&ok,
	)
	return int64(r), ok != 0
}

// registerInRAMJIT — bind jit_leaf_inram (image, arg): run an arm64 leaf image
// in-RAM; a refused image or page answers nothing.
func (k *Kernel) registerInRAMJIT() {
	k.registerNative("jit_leaf_inram", catMethod(), func(_ *Kernel, args []Value) Value {
		if len(args) != 2 || args[0].Kind != VList || args[1].Kind != VInt {
			return Value{Kind: VNull}
		}
		code := make([]byte, len(args[0].List))
		for i, b := range args[0].List {
			if b.Kind != VInt || b.Int < 0 || b.Int > 255 {
				return Value{Kind: VNull}
			}
			code[i] = byte(b.Int)
		}
		r, ok := runLeafInRAM(code, args[1].Int)
		if !ok {
			return Value{Kind: VNull}
		}
		return Value{Kind: VInt, Int: wrap63(r)}
	})
}
