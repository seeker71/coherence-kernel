package main

import (
	"os"
	"runtime"
	"syscall"
	"testing"
)

// TestHostExecLaunchFailureAnswersNothing — the Go arm's twin of
// form/form-stdlib/tests/host-exec-launch-honesty-band.fk: a host-exec whose
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

// TestHostReadAbsentAnswersNothing — a file that never was answers nothing,
// never "": "" means the file EXISTS and holds zero bytes. host-read was the
// lone masked read organ on this arm (read_file already answered VNull);
// healed 2026-08-27.
func TestHostReadAbsentAnswersNothing(t *testing.T) {
	k := NewKernel()
	read := func(p string) Value {
		return k.natives[k.internName("host-read")].Fn(k, []Value{{Kind: VStr, Str: p}})
	}
	if v := read("/nonexistent-host-read-absence-probe"); v.Kind != VNull {
		t.Fatalf("absent file must answer nothing (VNull): kind=%v str=%q", v.Kind, v.Str)
	}
	if v := read("/dev/null"); v.Kind != VStr || v.Str != "" {
		t.Fatalf("present empty file must answer \"\": kind=%v str=%q", v.Kind, v.Str)
	}
}

// TestPartialAsWholeOrgansAnswerNothing — the partial-as-whole cousins
// (2026-08-27): a dead socket handle or slice-of-never-was answers nothing;
// a past-end str_line_at answers nothing (an absent line is not an empty
// line); an EOF-short slice of a real file stays an honest short string.
func TestPartialAsWholeOrgansAnswerNothing(t *testing.T) {
	k := NewKernel()
	call := func(name string, args ...Value) Value {
		return k.natives[k.internName(name)].Fn(k, args)
	}
	if v := call("socket_recv", Value{Kind: VInt, Int: -1}, Value{Kind: VInt, Int: 16}); v.Kind != VNull {
		t.Fatalf("dead-handle recv must answer nothing: kind=%v", v.Kind)
	}
	if v := call("read_file_slice", Value{Kind: VStr, Str: "/nonexistent-pw-probe"}, Value{Kind: VInt, Int: 0}, Value{Kind: VInt, Int: 4}); v.Kind != VNull {
		t.Fatalf("slice of never-was must answer nothing: kind=%v", v.Kind)
	}
	f, err := os.CreateTemp(t.TempDir(), "pw-*.txt")
	if err != nil {
		t.Fatal(err)
	}
	if _, err := f.WriteString("ab"); err != nil {
		t.Fatal(err)
	}
	f.Close()
	if v := call("read_file_slice", Value{Kind: VStr, Str: f.Name()}, Value{Kind: VInt, Int: 0}, Value{Kind: VInt, Int: 8}); v.Kind != VStr || v.Str != "ab" {
		t.Fatalf("EOF-short slice stays honest bytes: kind=%v str=%q", v.Kind, v.Str)
	}
	if v := call("str_line_at", Value{Kind: VStr, Str: "ab\ncd"}, Value{Kind: VInt, Int: 99}); v.Kind != VNull {
		t.Fatalf("past-end line must answer nothing: kind=%v", v.Kind)
	}
	if v := call("str_line_at", Value{Kind: VStr, Str: "ab\ncd"}, Value{Kind: VInt, Int: 3}); v.Kind != VStr || v.Str != "cd" {
		t.Fatalf("in-range line unchanged: kind=%v str=%q", v.Kind, v.Str)
	}
}

// TestStrLenOfNothingDiesLoud — measuring an absence must not answer a
// counterfeit 0: str_len over the null head-of-empty carries (the value a
// starved host-exec now answers) dies loud, on this arm as on fkwu's op 25.
// Silent error hides illness (2026-08-27).
func TestStrLenOfNothingDiesLoud(t *testing.T) {
	k := NewKernel()
	defer func() {
		if recover() == nil {
			t.Fatalf("str_len of nothing answered instead of dying")
		}
	}()
	k.natives[k.internName("str_len")].Fn(k, []Value{{Kind: VNull}})
}
