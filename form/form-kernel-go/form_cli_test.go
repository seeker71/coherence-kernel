// form_cli_test.go — canonical standalone Form CLI proof from the Go suite.
//
// Form CLI has one authoring path: the committed table/C carrier and its
// behavioral proof. The Go sibling used to carry three separate full-source
// flatten/build copies (headless, REPL, combined), each retaining tens of GB
// on a full run. That duplicated the maintainer path and proved less. This test
// now crosses the canonical build once and runs the stronger identity, exact
// bytes, production-index, embedding, grounding, dual-HMAC, and replay proof.
package main

import (
	"os"
	"os/exec"
	"path/filepath"
	"strings"
	"testing"
)

func TestFkwuFormCliCanonicalCarrier(t *testing.T) {
	_ = requireClang(t)
	bash, err := exec.LookPath("bash")
	if err != nil {
		t.Skip("bash not available — canonical carrier proof skipped")
	}
	if _, err := exec.LookPath("openssl"); err != nil {
		t.Skip("openssl not available — canonical HMAC proof skipped")
	}

	formDir, err := filepath.Abs("..")
	if err != nil {
		t.Fatal(err)
	}
	proofBinary := filepath.Join(t.TempDir(), "form-cli")
	build := exec.Command(bash, "build-form-cli.sh", proofBinary)
	build.Dir = formDir
	if output, err := build.CombinedOutput(); err != nil {
		t.Fatalf("canonical form-cli build: %v\n%s", err, output)
	}

	digestBytes, err := os.ReadFile(filepath.Join(formDir, "form-stdlib", "bootstrap", "form-cli.source.sha256"))
	if err != nil {
		t.Fatalf("read canonical source digest: %v", err)
	}
	proof := exec.Command(
		bash,
		filepath.Join(formDir, "scripts", "form_cli_bootstrap_proof.sh"),
		proofBinary,
		strings.TrimSpace(string(digestBytes)),
	)
	proof.Dir = formDir
	output, err := proof.CombinedOutput()
	if err != nil {
		t.Fatalf("canonical form-cli behavioral proof: %v\n%s", err, output)
	}
	if !strings.Contains(string(output), "form-cli behavioral proof: OK") {
		t.Fatalf("canonical proof receipt missing: %s", output)
	}
}

// TestFkwuLocaleUtf8 independently locks the fourth kernel's byte-exact UTF-8
// print path with a freshly flattened small band.
func TestFkwuLocaleUtf8(t *testing.T) {
	clang := requireClang(t)
	stdlib := filepath.Join("..", "form-stdlib")
	minimal, hatiKernel, hostIOFs, fkcSerialize, hatiEmit := emitChain(t, stdlib)
	flattenCore, formParse, bmfCore, bmfGrammar, hostEffect, formFlatten := flattenChain(t, stdlib)
	shim := filepath.Join(stdlib, "fourth-shim.fk")
	core := filepath.Join(stdlib, "core.fk")

	dir := t.TempDir()
	fkwuBin := buildFkwu(t, clang, dir, minimal, hatiKernel, hostIOFs, fkcSerialize, hatiEmit)
	want := "中文 العربية हिन्दी 😀—ok"
	bandPath := filepath.Join(dir, "locale-band.fk")
	if err := os.WriteFile(bandPath, []byte("; locale\n(str_concat \"中文 العربية हिन्दी 😀\" \"—ok\")\n"), 0o644); err != nil {
		t.Fatalf("write band: %v", err)
	}
	mods := `(list (read_file "` + shim + `") (read_file "` + core + `"))`
	band := `(read_file "` + bandPath + `")`
	flattenExpr := "(fks-table-file (flt-band-sources-fns " + mods + " " + band + ") (flt-band-sources-pool " + mods + " " + band + "))"
	_, table := runFormSource(t, readFiles(t, minimal, hatiKernel, hostIOFs, fkcSerialize, hatiEmit, flattenCore, formParse, bmfCore, bmfGrammar, hostEffect, formFlatten)+"\n"+flattenExpr+"\n")
	tablePath := filepath.Join(dir, "locale-table.txt")
	if err := os.WriteFile(tablePath, []byte(table), 0o644); err != nil {
		t.Fatalf("write table: %v", err)
	}
	got, err := FkwuEval(fkwuBin, tablePath, 0)
	if err != nil {
		t.Fatalf("FkwuEval: %v", err)
	}
	if got != want {
		t.Fatalf("fkwu locale print:\n got=%q (% x)\nwant=%q (% x)", got, []byte(got), want, []byte(want))
	}
}
