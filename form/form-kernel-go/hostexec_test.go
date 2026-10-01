//go:build unix

package main

import (
	"runtime"
	"syscall"
	"testing"
)

// TestHostExecLaunchFailureAnswersNothing — a host-exec whose
// sh never launched answers the null head-of-empty carries (nothing), never
// ""; "" is reserved for a command that ran and spoke zero bytes, and a
// process that ran and exited nonzero still answers its output. The starved
// leg lowers RLIMIT_NPROC to 1 so the launch fork fails for real, then
// restores it (witnessed 2026-08-27, PR #542 diagnosis).
func TestHostExecLaunchFailureAnswersNothing(t *testing.T) {
	k := NewKernel()
	call := func(cmd string) Value {
		return k.natives[k.internName("host-exec")].Fn(k, []Value{{Kind: VStr, Str: cmd}})
	}

	if v := call("printf ''"); v.Kind != VStr || v.Str != "" {
		t.Fatalf("empty speech must stay a string: kind=%v str=%q", v.Kind, v.Str)
	}
	if v := call("exit 3"); v.Kind != VStr {
		t.Fatalf("a process that ran and exited nonzero still answers its output: kind=%v", v.Kind)
	}

	rlimitNproc := 7 // darwin
	if runtime.GOOS == "linux" {
		rlimitNproc = 6
	}
	var old syscall.Rlimit
	if err := syscall.Getrlimit(rlimitNproc, &old); err != nil {
		t.Skipf("cannot read RLIMIT_NPROC on this host: %v", err)
	}
	starved := syscall.Rlimit{Cur: 1, Max: old.Max}
	if err := syscall.Setrlimit(rlimitNproc, &starved); err != nil {
		t.Skipf("cannot starve fork on this host: %v", err)
	}
	v := call("date +%s%N")
	// darwin silently clamps the HARD limit to kern.maxprocperuid during the
	// unprivileged lower (witnessed 16000 -> 10666), so restoring the exact
	// Getrlimit round-trip answers EPERM. Re-read and restore the soft limit
	// under whatever hard limit actually stands.
	var clamped syscall.Rlimit
	if err := syscall.Getrlimit(rlimitNproc, &clamped); err != nil {
		t.Fatalf("re-read RLIMIT_NPROC: %v", err)
	}
	clamped.Cur = old.Cur
	if clamped.Cur > clamped.Max {
		clamped.Cur = clamped.Max
	}
	if err := syscall.Setrlimit(rlimitNproc, &clamped); err != nil {
		t.Fatalf("restore RLIMIT_NPROC: %v", err)
	}
	if v.Kind != VNull {
		t.Fatalf("starved launch must answer nothing (VNull): kind=%v str=%q", v.Kind, v.Str)
	}
	if v := call("printf hx-tok"); v.Kind != VStr || v.Str != "hx-tok" {
		t.Fatalf("after restore host-exec must speak again: kind=%v str=%q", v.Kind, v.Str)
	}
}

// TestHostExecAnswersStdout — host-exec answers what the child wrote to its
// stdout, as fkwu's fk_host_exec does; its stderr passes through to the
// kernel's own and never joins the answer, and a non-empty input is its stdin.
func TestHostExecAnswersStdout(t *testing.T) {
	k := NewKernel()
	call := func(cmd, input string) Value {
		return k.natives[k.internName("host-exec")].Fn(k, []Value{{Kind: VStr, Str: cmd}, {Kind: VStr, Str: input}})
	}
	if v := call("printf out; echo host-exec-stderr-passes-through >&2", ""); v.Kind != VStr || v.Str != "out" {
		t.Fatalf("the answer is stdout alone: kind=%v str=%q", v.Kind, v.Str)
	}
	if v := call("cat", "hello"); v.Kind != VStr || v.Str != "hello" {
		t.Fatalf("a non-empty input is the child's stdin: kind=%v str=%q", v.Kind, v.Str)
	}
}

// TestReadFileAbsentAnswersNothing — a file that never was answers nothing,
// never "": "" means the file exists and holds zero bytes.
func TestReadFileAbsentAnswersNothing(t *testing.T) {
	k := NewKernel()
	read := func(p string) Value {
		return k.natives[k.internName("read_file")].Fn(k, []Value{{Kind: VStr, Str: p}})
	}
	if v := read("/nonexistent-read-file-absence-probe"); v.Kind != VNull {
		t.Fatalf("absent file must answer nothing (VNull): kind=%v str=%q", v.Kind, v.Str)
	}
	if v := read("/dev/null"); v.Kind != VStr || v.Str != "" {
		t.Fatalf("present empty file must answer \"\": kind=%v str=%q", v.Kind, v.Str)
	}
}
