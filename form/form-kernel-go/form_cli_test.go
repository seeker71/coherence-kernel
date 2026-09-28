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
