#!/usr/bin/env python3

import importlib.util
import json
import os
import stat
import tempfile
import unittest
import wave
from argparse import Namespace
from pathlib import Path


MODULE_PATH = Path(__file__).with_name("offline_workshop.py")
SPEC = importlib.util.spec_from_file_location("offline_workshop", MODULE_PATH)
assert SPEC and SPEC.loader
WORKSHOP = importlib.util.module_from_spec(SPEC)
SPEC.loader.exec_module(WORKSHOP)


class OfflineWorkshopTest(unittest.TestCase):
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory(prefix="offline-workshop-test-")
        self.root = Path(self.temporary.name)
        self.audio = self.root / "input.wav"
        with wave.open(str(self.audio), "wb") as output:
            output.setnchannels(1)
            output.setsampwidth(2)
            output.setframerate(16000)
            output.writeframes(b"\x00\x00" * 16000)
        self.model = self.root / "model.bin"
        self.model.write_bytes(b"observed local model fixture")
        self.ffmpeg = self._executable(
            "fake-ffmpeg",
            """#!/usr/bin/env python3
import shutil, sys
source = sys.argv[sys.argv.index('-i') + 1]
target = sys.argv[-1]
shutil.copyfile(source, target)
""",
        )

    def tearDown(self):
        self.temporary.cleanup()

    def _executable(self, name, source):
        path = self.root / name
        path.write_text(source, encoding="utf-8")
        path.chmod(path.stat().st_mode | stat.S_IXUSR)
        return path

    def _whisper(self, accelerated_fails=True, cpu_fails=False):
        return self._executable(
            "fake-whisper",
            f"""#!/usr/bin/env python3
import json, pathlib, sys
cpu = '-ng' in sys.argv
if ({accelerated_fails!r} and not cpu) or ({cpu_fails!r} and cpu):
    raise SystemExit(139)
stem = pathlib.Path(sys.argv[sys.argv.index('-of') + 1])
payload = {{
  'systeminfo': 'offline fixture',
  'model': {{'type': 'fixture'}},
  'result': {{'language': 'en'}},
  'transcription': [
    {{'offsets': {{'from': 0, 'to': 900}}, 'text': ' We arrive together.',
      'tokens': [{{'text': ' arrive', 'p': 0.75}}]}},
    {{'offsets': {{'from': 900, 'to': 1800}}, 'text': ' Unknown stays honest.',
      'tokens': [{{'text': ' honest', 'p': 0.8}}]}}
  ]
}}
stem.with_suffix('.json').write_text(json.dumps(payload), encoding='utf-8')
""",
        )

    def _capture_ffmpeg(self):
        return self._executable(
            "fake-capture-ffmpeg",
            """#!/usr/bin/env python3
import pathlib, shutil, sys, wave
source = pathlib.Path(sys.argv[sys.argv.index('-i') + 1])
target = pathlib.Path(sys.argv[-1])
if source.is_file():
    shutil.copyfile(source, target)
else:
    with wave.open(str(target), 'wb') as output:
        output.setnchannels(1)
        output.setsampwidth(2)
        output.setframerate(16000)
        output.writeframes(b'\\x00\\x00' * 16000)
""",
        )

    def _args(self, whisper, consent="explicit"):
        return Namespace(
            consent=consent,
            model=str(self.model),
            ffmpeg_bin=str(self.ffmpeg),
            whisper_bin=str(whisper),
            fkwu_bin="missing-fkwu-test-name",
            language="auto",
            cpu=False,
            timeout=20,
        )

    def test_accelerated_failure_becomes_observed_cpu_resolution(self):
        session = self.root / "session"
        WORKSHOP.transcribe_into_session(
            self._args(self._whisper()), self.audio, session, "test-audio"
        )
        manifest = json.loads((session / "manifest.json").read_text(encoding="utf-8"))
        metrics = json.loads((session / "evidence/metrics.json").read_text(encoding="utf-8"))
        self.assertEqual(manifest["state"], "complete")
        self.assertEqual(manifest["asr"]["resolved_by"], "cpu-fallback")
        self.assertEqual([attempt["result"] for attempt in metrics["asr_attempts"]],
                         ["failed-attention", "resolved"])
        self.assertEqual(metrics["unknown_speaker_segments"], 2)
        self.assertIsNone(metrics["wer"])
        report = WORKSHOP.verify_session(session, "missing-fkwu-test-name", require_form=False)
        self.assertTrue(report["passed"], report)

    def test_consent_absence_is_rejected_before_capture(self):
        session = self.root / "without-consent"
        with self.assertRaises(WORKSHOP.WorkshopError) as caught:
            WORKSHOP.transcribe_into_session(
                self._args(self._whisper(), consent="assumed"), self.audio, session, "test-audio"
            )
        self.assertEqual(caught.exception.code, "consent-not-explicit")
        self.assertFalse(session.exists())

    def test_all_asr_failures_leave_attention_and_resolution_evidence(self):
        session = self.root / "failed-session"
        with self.assertRaises(WORKSHOP.WorkshopError) as caught:
            WORKSHOP.transcribe_into_session(
                self._args(self._whisper(accelerated_fails=True, cpu_fails=True)),
                self.audio, session, "test-audio",
            )
        self.assertEqual(caught.exception.code, "asr-all-attempts-failed")
        status = json.loads((session / "status.json").read_text(encoding="utf-8"))
        attempts = json.loads((session / "evidence/asr-attempts.json").read_text(encoding="utf-8"))
        self.assertEqual(status["state"], "failed")
        self.assertIn("--cpu", status["resolution"])
        self.assertEqual(len(attempts["attempts"]), 2)
        self.assertTrue(all(item["result"] == "failed-attention" for item in attempts["attempts"]))
        self.assertFalse((session / "manifest.json").exists())

    def test_hash_tamper_is_observed(self):
        session = self.root / "tampered"
        WORKSHOP.transcribe_into_session(
            self._args(self._whisper(accelerated_fails=False)), self.audio, session, "test-audio"
        )
        transcript = session / "transcript/transcript.md"
        transcript.write_text(transcript.read_text(encoding="utf-8") + "changed\n", encoding="utf-8")
        report = WORKSHOP.verify_session(session, "missing-fkwu-test-name", require_form=False)
        self.assertFalse(report["passed"])
        self.assertIn("artifact-hash-mismatch:transcript/transcript.md", report["errors"])

    def test_human_review_turns_only_named_segments_known(self):
        session = self.root / "reviewed"
        WORKSHOP.transcribe_into_session(
            self._args(self._whisper(accelerated_fails=False)), self.audio, session, "test-audio"
        )
        labels = self.root / "labels.tsv"
        labels.write_text("seg-000001\tFacilitator\n", encoding="utf-8")
        args = Namespace(
            session=str(session), labels=str(labels), consent="explicit",
            fkwu_bin="missing-fkwu-test-name", no_form=True,
        )
        report = WORKSHOP.relabel_session(args)
        self.assertTrue(report["passed"], report)
        self.assertEqual(report["counts"]["known_speaker_segments"], 1)
        self.assertEqual(report["counts"]["unknown_speaker_segments"], 1)
        segments = WORKSHOP.read_segments(session)
        self.assertEqual(segments[0]["speaker_source"], "human-reviewed")
        self.assertEqual(segments[1]["speaker"], "unknown")

    def test_doctor_reports_resolved_local_dependencies(self):
        whisper = self._whisper(accelerated_fails=False)
        args = Namespace(
            model=str(self.model), ffmpeg_bin=str(self.ffmpeg), whisper_bin=str(whisper),
            fkwu_bin="missing-fkwu-test-name",
        )
        report = WORKSHOP.doctor_report(args)
        self.assertTrue(report["ready"])
        self.assertTrue(all(report["required"].values()))

    def test_microphone_carrier_path_is_retained_and_hashed(self):
        session = self.root / "captured"
        ffmpeg = self._capture_ffmpeg()
        args = self._args(self._whisper(accelerated_fails=False))
        args.ffmpeg_bin = str(ffmpeg)
        args.input_format = "fixture-audio"
        args.input_device = "fixture-device"
        args.seconds = 1
        WORKSHOP.begin_session(session, "microphone-capture")
        capture_path = session / "audio/capture.wav"
        WORKSHOP.capture_audio(args, capture_path)
        WORKSHOP.transcribe_into_session(
            args, capture_path, session, "microphone-capture", session_started=True,
        )
        manifest = json.loads((session / "manifest.json").read_text(encoding="utf-8"))
        self.assertIn("audio/capture.wav", manifest["artifacts"])
        self.assertEqual(
            manifest["artifacts"]["audio/capture.wav"]["sha256"],
            WORKSHOP.sha256_file(capture_path),
        )

    def test_human_reference_turns_wer_and_label_error_into_numbers(self):
        session = self.root / "evaluated"
        WORKSHOP.transcribe_into_session(
            self._args(self._whisper(accelerated_fails=False)), self.audio, session, "test-audio"
        )
        labels = self.root / "all-labels.tsv"
        labels.write_text("seg-000001\tFacilitator\nseg-000002\tAri\n", encoding="utf-8")
        WORKSHOP.relabel_session(Namespace(
            session=str(session), labels=str(labels), consent="explicit",
            fkwu_bin="missing-fkwu-test-name", no_form=True,
        ))
        reference = self.root / "reference.txt"
        reference.write_text("We arrive together. Unknown stays honest.\n", encoding="utf-8")
        speakers = self.root / "reference-speakers.tsv"
        speakers.write_text("0\t900\tFacilitator\n900\t1800\tAri\n", encoding="utf-8")
        report = WORKSHOP.evaluate_session(Namespace(
            session=str(session), reference_transcript=str(reference),
            speaker_reference=str(speakers), consent="explicit",
            fkwu_bin="missing-fkwu-test-name", no_form=True,
        ))
        self.assertTrue(report["passed"], report)
        self.assertEqual(report["quality"]["wer"], 0.0)
        self.assertEqual(report["quality"]["der"], 0.0)
        self.assertIsNone(report["unknowns"]["wer"])
        self.assertIsNone(report["unknowns"]["der"])


if __name__ == "__main__":
    unittest.main()
