#!/usr/bin/env python3
"""Execute the real Form healer in disposable repository fixtures.

Provider-order cases use explicitly simulated transports; they prove orchestration,
not model quality. All candidate checks and guarded file replacements are real.
"""
import json
import hashlib
import os
from pathlib import Path
import shutil
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[2]


def run(root, argv, text=None, limit=90):
    result = subprocess.run(argv, cwd=root, input=text, text=True, capture_output=True, timeout=limit, env={**os.environ, "PATH": str(root / "bin") + os.pathsep + os.environ["PATH"]})
    if result.returncode:
        raise AssertionError(f"{argv}: exit={result.returncode}\n{result.stdout}\n{result.stderr}")
    return result.stdout


def write(root, path, text):
    file = root / path
    file.parent.mkdir(parents=True, exist_ok=True)
    file.write_text(text)


def form(root, expression):
    write(root, "driver.fk", "; preludes: form-stdlib/bml/form-cli-heal.bml\n" + expression + "\n")
    return run(root, ["./fkwu", "driver.fk"])


def prepare(root):
    return run(root, ["python3", "form/scripts/heal-host.py", "prepare", "subject.fk", "check.fk"]).strip()


def repair_result(output):
    if "heal healed" not in output:
        for line in output.splitlines():
            if line.startswith("evidence="):
                job = Path(line.split("=", 1)[1])
                ledger = job / "attempts.tsv"
                if ledger.is_file():
                    output += "\nStage evidence:\n" + ledger.read_text()
                for leaf in ("inventory.status", "inventory.out", "inventory.err"):
                    file = job / leaf
                    if file.is_file():
                        output += "\n" + leaf + ":\n" + file.read_text()[:4000]
    return output


def try_source(root, job, candidate, route):
    # The test driver must already be snapshotted; use a stable stdin driver.
    return run(root, ["./fkwu", "try-driver.fk"], "\n".join([job, route, candidate.replace("\n", " ")]) + "\n").strip()


def mock_native(root, rows, replies=None, adapter_only=False):
    """Explicit simulation: no model admission, weights, or quality claim."""
    reply = '"NOTHING"'
    for model, text in reversed(list((replies or {}).items())):
        reply = f'if str_eq(model, {json.dumps(model)}) then {json.dumps(text)} else {reply}'
    selected = 'if str_eq(mode, "adapter") then mock-reply(model) else "NOTHING"' if adapter_only else 'mock-reply(model)'
    inventory = json.dumps("\n".join(rows) + "\n")
    write(root, "observe/form-cli-heal-model-run.bml", f'''section [form.bml] {{
 class MockNative {{
 def mock-reply(model) = {reply};
 def mock-generate(mode) {{
 let model = read_line();
 let prompt = read_line();
 let output = read_line();
 write_file(output, {selected});
 }}
 def mock-main() {{
 let mode = read_line();
 if str_eq(mode, "inventory") then print_str({inventory}) else mock-generate(mode);
 }}
 }}
 mock-main();
}}
''')


def main():
    results = []
    with tempfile.TemporaryDirectory(prefix="form-heal-proof-") as directory:
        root = Path(directory)
        names = subprocess.check_output(["git", "ls-files", "-z", "--cached", "--others", "--exclude-standard"], cwd=ROOT).split(b"\0")
        for raw in names:
            name = os.fsdecode(raw)
            if not name or name.startswith((".form-heal/", ".coherence-network/")):
                continue
            source = ROOT / name
            if source.suffix not in (".fk", ".bml", ".py", ".md", ".c") or not source.is_file() or source.is_symlink():
                continue
            destination = root / name
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(source, destination)
        shutil.copy2(ROOT / "fkwu", root / "fkwu")
        # Reproduce the real source/cache boundary, including BML lowering
        # memos. An incomplete fixture ignore list makes caches stale inputs.
        write(root, ".gitignore", (ROOT / ".gitignore").read_text())
        run(root, ["git", "init", "-q"])
        write(root, "subject.fk", "(do\n (defn answer () 7)\n")
        write(root, "check.fk", "; preludes: subject.fk\n(answer)\n")
        write(root, "try-driver.fk", '; preludes: form-stdlib/bml/form-cli-heal.bml\n(do (let job (read_line)) (let route (read_line)) (let source (read_line)) (fh-try job "subject.fk" "check.fk" "7" 2 source route 700))\n')
        # Avoid paid calls in ALL fixtures, including regressions in route order.
        write(root, "bin/codex", '#!/usr/bin/env python3\nimport sys,pathlib\nif "--help" in sys.argv: print("--ephemeral --ignore-user-config")\nelse:\n sys.stdin.read()\n pathlib.Path(sys.argv[sys.argv.index("-o")+1]).write_text("<heal-old>\\n() 6\\n</heal-old>\\n<heal-new>\\n() 7\\n</heal-new>")\n')
        (root / "bin/codex").chmod(0o755)
        # Full command dispatch, not just the standalone helper.
        write(root, "command.fk", '; preludes: form-stdlib/form-cli.fk\n(print_str (fc-respond (str_concat "heal " (read_line))))\n')
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10|local\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        assert run(root, ["./fkwu", "check.fk"]).strip() == "7"
        job = next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence="))
        assert (Path(job) / "accepted-source").is_file()
        assert not (Path(job) / "work").exists()
        assert not (Path(job) / "remote.process.json").exists()
        results.append("real CLI: structural repair, exact band, durable lesson, no remote")
        write(root, "subject.fk", "(do\n (defn answer () 7)\n")
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10|local\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        results.append("repair memory replay reverified")

        original = "(defn answer () 6)\n"
        write(root, "subject.fk", original)
        job = prepare(root)
        assert try_source(root, job, "(defn answer () 8)", "semantic") == "0"
        assert (root / "subject.fk").read_text() == original
        assert try_source(root, job, "(do (defn answer () 7) (never-defined))", "false-green") == "0"
        assert (root / "subject.fk").read_text() == original
        outcome = try_source(root, job, '(defn answer () (do (host-exec "sleep 5" "") 7))', "timeout")
        assert outcome.splitlines()[-1] == "0" and "timeout check-timeout:" in outcome, outcome
        assert (Path(job) / "check-timeout.status").read_text() == "124"
        assert (root / "subject.fk").read_text() == original
        outcome = try_source(root, job, '(defn answer () (do (write_file "check.fk" "7") 7))', "test-tamper")
        assert outcome == "0", outcome
        assert (root / "check.fk").read_text() == "; preludes: subject.fk\n(answer)\n"
        results.append("semantic mismatch, false green, timeout and test tamper refused; original bytes retained")
        write(root, "subject.fk", "(defn answer () 9)\n")
        assert try_source(root, job, "(defn answer () 7)", "stale") == "-1"
        assert (root / "subject.fk").read_text() == "(defn answer () 9)\n"
        results.append("stale live input preserved")

        # Admission binds actual base bytes, adapter bytes, and evaluated receipt.
        write(root, "base.bin", "fixture base")
        write(root, "adapter.bin", "fixture adapter, not a model-quality witness")
        base_sha = hashlib.sha256((root / "base.bin").read_bytes()).hexdigest()
        adapter_sha = hashlib.sha256((root / "adapter.bin").read_bytes()).hexdigest()
        receipt = f"repair-heldout-v1|fixture|{base_sha}|{adapter_sha}|2|2|0"
        write(root, ".form-heal/evaluation.txt", receipt)
        receipt_sha = hashlib.sha256(receipt.encode()).hexdigest()
        write(root, ".form-heal/adapter.tsv", "\t".join(["fixture", base_sha, str(root / "adapter.bin"), adapter_sha, str(root / ".form-heal/evaluation.txt"), receipt_sha]))
        query = f'(fh-adapter "fixture" (list "fixture\\t{root}/base.bin\\tanswer\\tartifact=1 seal=1"))'
        assert str(root / "adapter.bin") in form(root, query)
        write(root, "base.bin", "changed base")
        assert str(root / "adapter.bin") not in form(root, query)
        (root / ".form-heal/adapter.tsv").unlink()
        results.append("LoRA registration rejects a changed base; device-shape admission remains separate")

        # Explicit provider simulations for a physical, ordered fallback witness.
        ready = "artifact=1 native=1 seal=1 tokfast=1"
        native_rows = [f"mock-q8\t/local/q8\tanswer review\t{ready}", f"mock-q4\t/local/q4\tfast-answer review\t{ready}"]
        mock_native(root, native_rows)
        write(root, "form/scripts/heal-ollama.py", 'import sys\nif sys.argv[1]=="list": print("reasoner\\ncoder\\nqwen-review")\nelse:\n text=sys.stdin.read()\n if "You are the diagnosis" in text: print("Inspect the answer body")\n elif "You are the review" in text: print("ACCEPT")\n else: print("<heal-old>\\n() 6\\n</heal-old>\\n<heal-new>\\n() 8\\n</heal-new>")\n')
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        ledger = (job / "attempts.tsv").read_text()
        stages = ["adapter-admission", "native-model-0\t", "native-model-0-complete", "native-model-1\t", "native-model-1-complete", "local-diagnosis", "local-patch\t", "local-review", "remote-admission", "remote\t"]
        offsets = [ledger.index(stage) for stage in stages]
        assert offsets == sorted(offsets), ledger
        assert json.loads((job / "local-patch.process.json").read_text())["argv"][-1] == "coder"
        assert json.loads((job / "local-review.process.json").read_text())["argv"][-1] == "qwen-review"
        assert (job / "frontier-return.tsv").is_file()
        assert (root / "subject.fk").read_text() == "(defn answer () 7)\n"
        assert "mock-q4\t/local/q4\teligible\tnative-model-1" in (job / "native-plan.tsv").read_text()
        results.append("simulated providers: both native models complete before local roles and one remote proposal; real Form verification")
        # A passing local proposal ends the movement before frontier admission.
        (root / ".form-heal/learned.tsv").unlink()
        local_script = root / "form/scripts/heal-ollama.py"
        local_script.write_text(local_script.read_text().replace('() 8', '() 7'))
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        assert not (job / "remote.process.json").exists()
        assert "local-patch\tapplied" in (job / "attempts.tsv").read_text()
        results.append("passing simulated local patch and review stop before remote")

        (root / ".form-heal/learned.tsv").unlink()
        local_script.write_text(local_script.read_text().replace('() 7', '() 8'))
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10|local\n", limit=120)
        assert "unresolved-local-evidence-retained" in output, output
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        assert not (job / "remote.process.json").exists()
        assert (root / "subject.fk").read_text() == original
        results.append("local-only exhaustion remains unresolved without a remote call")

        # A failed first native candidate advances to the second eligible model.
        # Unwired, unindexed and duplicate paths are explained, never admitted.
        proposal = "<heal-old>\n() 6\n</heal-old>\n<heal-new>\n() 7\n</heal-new>"
        expanded = native_rows + [
            f"alias\t/local/q4\tanswer\t{ready}",
            "unwired\t/local/moe\tcode\tartifact=1 native=0 seal=1 tokfast=1",
            "unindexed\t/local/plain\tanswer\tartifact=1 native=1 seal=1 tokfast=0",
        ]
        mock_native(root, expanded, {"mock-q8": proposal.replace("() 7", "() 8"), "mock-q4": proposal})
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        ledger = (job / "attempts.tsv").read_text()
        assert "native-model-0-complete\tmodel=mock-q8;result=0" in ledger
        assert "native-model-1-complete\tmodel=mock-q4;result=1" in ledger
        assert "native-model-1\tapplied" in ledger
        assert not (job / "local-list.process.json").exists()
        assert not (job / "remote.process.json").exists()
        plan = (job / "native-plan.tsv").read_text()
        assert all(reason in plan for reason in ("duplicate-artifact", "lane-unavailable", "tokenizer-index-absent")), plan
        assert not (job / "native-model-2.process.json").exists()
        results.append("second simulated native model repairs after first candidate fails; unavailable and duplicate resources are explained; no local/remote generation")

        # A malformed inventory leaves an evidence gap; it cannot claim local
        # exhaustion even if the ordinary local roles all report completion.
        (root / ".form-heal/learned.tsv").unlink()
        mock_native(root, native_rows + ["unparsed resource"])
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10\n", limit=120)
        assert "unresolved-local-evidence-retained" in output, output
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        assert (job / "native-inventory-status").read_text() == "refused-inventory-evidence"
        assert "remote-admission\t0" in (job / "attempts.tsv").read_text()
        assert not (job / "remote.process.json").exists()
        assert (root / "subject.fk").read_text() == original
        results.append("malformed native inventory blocks remote fallback despite completed local-role receipts")

        # Registration can belong to the second native model. Hash admission
        # is real; this provider deliberately simulates adapter generation.
        write(root, "base-second.bin", "second simulated base")
        base_sha = hashlib.sha256((root / "base-second.bin").read_bytes()).hexdigest()
        receipt = f"repair-heldout-v1|mock-q4|{base_sha}|{adapter_sha}|2|2|0"
        write(root, ".form-heal/evaluation.txt", receipt)
        receipt_sha = hashlib.sha256(receipt.encode()).hexdigest()
        write(root, ".form-heal/adapter.tsv", "\t".join(["mock-q4", base_sha, str(root / "adapter.bin"), adapter_sha, str(root / ".form-heal/evaluation.txt"), receipt_sha]))
        adapter_rows = [native_rows[0], f"mock-q4\t{root}/base-second.bin\tfast-answer review\t{ready}"]
        mock_native(root, adapter_rows, {"mock-q4": proposal}, adapter_only=True)
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        assert (job / "native-adapter.request").read_text().splitlines()[:2] == ["adapter", "mock-q4"]
        assert "native-adapter\tapplied" in (job / "attempts.tsv").read_text()
        assert not (job / "native-model-0.process.json").exists()
        assert not (job / "remote.process.json").exists()
        results.append("evaluated registration for second native model is offered first; simulated adapter proposal passes real source checks")

        # Actual timeout in a simulated provider, then the real next-resource
        # branch and guarded repair. This measures routing, not model skill.
        (root / ".form-heal/learned.tsv").unlink()
        (root / ".form-heal/adapter.tsv").unlink()
        mock_native(root, native_rows, {"mock-q4": proposal})
        provider = root / "observe/form-cli-heal-model-run.bml"
        provider.write_text(provider.read_text().replace("let model = read_line();", 'let model = read_line();\n let wait = if str_eq(model, "mock-q8") then host-exec("sleep 3", "") else "";'))
        policy = root / "form/form-stdlib/bml/form-cli-heal-policy.bml"
        policy_text = policy.read_text()
        assert "field HealNativeSeconds = 0;" in policy_text
        policy.write_text(policy_text.replace("field HealNativeSeconds = 0;", "field HealNativeSeconds = 1;"))
        write(root, "subject.fk", original)
        output = run(root, ["./fkwu", "command.fk"], "subject.fk|check.fk|7|10|local\n", limit=120)
        assert "heal healed" in output, repair_result(output)
        job = Path(next(line.split("=", 1)[1] for line in output.splitlines() if line.startswith("evidence=")))
        assert json.loads((job / "native-model-0.process.json").read_text())["reason"] == "timeout"
        report = json.loads((job / "timing-report.json").read_text())
        choices = report["choices"]
        branch = [c for c in choices if c["selected"] == "native-model:mock-q4"]
        assert [c["state"] for c in branch] == ["selected", "applied"], branch
        assert branch[0]["id"] == branch[1]["id"] and branch[1]["result"] == "1"
        admission = next(c for c in choices if c["selected"] == "mock-q8" and c["state"] == "selected")
        assert admission["offered"] == ["mock-q8", "mock-q4"]
        assert all(r["timing"]["accounted_ms"] == r["process"]["elapsed_ms"] for r in report["processes"])
        assert "timeout native-model-0:" in output, output
        assert not (job / "remote.process.json").exists()
        results.append("real timeout in simulated first provider; offered models, selected and applied second-model branch; conserved timing; verified repair without remote")
    print(json.dumps({"passed": len(results), "checks": results}, indent=2))


if __name__ == "__main__":
    main()
