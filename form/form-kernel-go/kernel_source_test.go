// kernel_source_test.go — the suite's way to run Form source in-process, the way
// the kernel's own entry runs it: files loaded with their prelude closures, the
// whole walked as one unit.
package main

import (
	"os/exec"
	"runtime"
	"runtime/debug"
	"strings"
	"testing"
)

// runFormSource walks Form source in-process as one unit and returns the result
// value's raw string (the .Str for a string value, else .String()).
func runFormSource(t *testing.T, src string) (Value, string) {
	t.Helper()
	// The helper's stack may conservatively keep the completed kernel live until
	// its owning test returns. A cleanup runs after that stack is gone and before
	// the next test starts, which is the reliable process-like phase boundary.
	t.Cleanup(func() {
		runtime.GC()
		debug.FreeOSMemory()
	})
	k := NewKernel()
	root := readRootFromSource(k, src)
	// a source runs as the kernel's own entry runs it: one unit, its defns global
	result := k.walkUnit(root, NewFrame(nil))
	text := result.String()
	if result.Kind == VStr {
		text = result.Str
	}
	// Each call models an independent source-to-artifact invocation; release the
	// completed kernel's graph before the next proof begins.
	k = nil
	runtime.GC()
	debug.FreeOSMemory()
	return result, text
}

// readFiles joins the files as the kernel's own entry loads them: each file's
// prelude closure before it, every unit once, a .bml lowered.
func readFiles(t *testing.T, paths ...string) string {
	t.Helper()
	loaded, err := loadFormSourceClosure(paths)
	if err != nil {
		t.Fatalf("load %v: %v", paths, err)
	}
	var b strings.Builder
	for _, part := range loaded {
		b.WriteString(part.source)
		b.WriteByte('\n')
	}
	return b.String()
}

func requireClang(t *testing.T) string {
	t.Helper()
	clang, err := exec.LookPath("clang")
	if err != nil {
		t.Skip("clang not available — native proof skipped")
	}
	return clang
}
