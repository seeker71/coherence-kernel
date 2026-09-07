#!/usr/bin/env python3
"""Real CLI benchmark and evaluation-memory exclusion regression."""
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[2]


def main():
    spool = ROOT / ".form-heal"
    spool.mkdir(exist_ok=True)
    driver = spool / "eval-cli-test.fk"
    driver.write_text('; preludes: form-stdlib/form-cli.fk\n(print_str (fc-respond (str_concat "heal " (read_line))))\n')
    result = subprocess.run(["./fkwu", str(driver)], cwd=ROOT, input="eval\n",
                            text=True, capture_output=True, timeout=600)
    assert result.returncode == 0, result.stderr
    assert "heal evaluation measured" in result.stdout, result.stdout + result.stderr
    report = Path(next(line.split("=", 1)[1] for line in result.stdout.splitlines() if line.startswith("evaluation-report=")))
    summary = json.loads((report / "summary.json").read_text())
    assert summary["status"] == "measured", summary
    assert summary["counts"] == {"repaired": 3, "unresolved": 2, "preserved": 1}, summary
    assert summary["repair_memory_unchanged"] and summary["remote_calls"] == 0
    assert len([case for case in summary["cases"] if case["split"] == "heldout"]) == 4
    for case in summary["cases"]:
        evidence = report / case["id"] / "evidence"
        timeline = json.loads((evidence.parent / "case.process.json").read_text())
        stages = [e["phase"] for e in timeline["events"]]
        assert all(stage in stages for stage in ("healing-case-ready", "healing-snapshot-begin", "healing-snapshot-ready", "healing-process-baseline-begin", "healing-snapshot-release-ready", "healing-report-ready")), stages
        timing = json.loads((evidence.parent / "timing-report.json").read_text())["processes"][0]["timing"]
        assert timing["accounted_ms"] == timeline["elapsed_ms"]
        assert (evidence / "evaluation-only").is_file()
        assert not (evidence / "work").exists()
        if case["id"] in ("recursive-base", "positive-boundary"):
            assert (evidence / "baseline.status").read_text().strip() == "0"
            assert not (evidence / "baseline.err").read_text().strip()
            assert "chain         clean" in (evidence / "baseline-preflight.out").read_text()
        if case["outcome"] == "repaired":
            assert "memory-excluded" in (evidence / "attempts.tsv").read_text()
            assert "Excluded from repair memory and training" in (evidence / "training-candidate.txt").read_text()
    # Even an accidentally inserted pointer must not replay a held-out answer.
    job = report / "outer-close" / "evidence"
    pointer = "\t".join([str(job), "eval-subject.fk", "eval-check.fk", "19"])
    probe = spool / "eval-memory-test.fk"
    probe.write_text('; preludes: form-stdlib/bml/form-cli-heal.bml\n(do (let row (read_line)) (let source (read_file (read_line))) (len (fh-memory (list row) source "eval-check.fk" "19" 32)))\n')
    checked = subprocess.run(["./fkwu", str(probe)], cwd=ROOT,
        input=pointer + "\n" + str(job / "original") + "\n", text=True, capture_output=True, timeout=30)
    assert checked.returncode == 0 and checked.stdout.strip() == "0", checked.stdout + checked.stderr
    print(json.dumps({"passed": 4, "report": str(report), "checks": [
        "real form-cli eval dispatch measures all six cases",
        "three structural repairs, two semantic gaps, one unchanged control",
        "evaluation records retained without repair-memory or training admission",
        "inserted evaluation pointer cannot replay its accepted source"
    ]}, indent=2))


if __name__ == "__main__":
    main()
