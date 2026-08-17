import json
import sys
import tempfile
import unittest
from pathlib import Path


SCRIPTS = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(SCRIPTS))

import form_cli_turn_evidence as evidence


def event(timestamp, event_type, payload):
    return {"timestamp": timestamp, "type": event_type, "payload": payload}


class TurnEvidenceTest(unittest.TestCase):
    def write_rollout(self, rows):
        temp = tempfile.TemporaryDirectory()
        path = Path(temp.name) / "rollout.jsonl"
        path.write_text("".join(json.dumps(row) + "\n" for row in rows), encoding="utf-8")
        self.addCleanup(temp.cleanup)
        return path

    def test_collects_completed_turn_without_emitting_content(self):
        rows = [
            event("2026-08-17T00:00:00.000Z", "event_msg", {
                "type": "task_started", "turn_id": "turn-1"
            }),
            event("2026-08-17T00:00:00.100Z", "response_item", {
                "type": "function_call", "call_id": "native-call", "name": "exec"
            }),
            event("2026-08-17T00:00:00.400Z", "response_item", {
                "type": "function_call_output", "call_id": "native-call",
                "output": "private output\n@form fkwu 0 10 2 12"
            }),
            event("2026-08-17T00:00:00.500Z", "response_item", {
                "type": "custom_tool_call", "call_id": "local-call", "name": "patch"
            }),
            event("2026-08-17T00:00:00.700Z", "response_item", {
                "type": "custom_tool_call_output", "call_id": "local-call",
                "output": {"content": "private patch\n@form apply_patch 1 0 5 5"}
            }),
            event("2026-08-17T00:00:00.800Z", "event_msg", {
                "type": "patch_apply_end"
            }),
            event("2026-08-17T00:00:00.900Z", "event_msg", {
                "type": "token_count", "info": {"last_token_usage": {
                    "input_tokens": 100, "cached_input_tokens": 60,
                    "cache_write_input_tokens": 0, "output_tokens": 20,
                    "reasoning_output_tokens": 5, "total_tokens": 120
                }}
            }),
            event("2026-08-17T00:00:01.000Z", "event_msg", {
                "type": "task_complete", "turn_id": "turn-1", "duration_ms": 1000,
                "time_to_first_token_ms": 75, "last_agent_message": "private answer"
            }),
        ]
        path = self.write_rollout(rows)
        measured = evidence.collect(evidence.ThreadRef(
            "thread-1", path, "openai", "gpt-test"
        ))

        self.assertEqual(measured.uncached_input_tokens, 40)
        self.assertEqual(measured.tool_calls, 2)
        self.assertEqual(measured.tool_outputs, 2)
        self.assertEqual(measured.tool_wall_ms, 500)
        self.assertEqual(measured.form_commands, 2)
        self.assertEqual(measured.form_failures, 1)
        self.assertEqual(measured.form_stdout_bytes, 10)
        self.assertEqual(measured.form_stderr_bytes, 7)
        self.assertEqual(measured.form_shown_bytes, 17)
        self.assertEqual(measured.native_commands, 1)
        self.assertEqual(measured.native_failures, 0)
        self.assertEqual(measured.local_events, 1)
        self.assertEqual(measured.remote_events, 1)
        self.assertEqual(measured.total_events, 3)
        self.assertEqual(measured.patch_events, 1)
        self.assertEqual(measured.final_output_bytes, len("private answer"))
        self.assertNotIn("private", measured.row())
        self.assertEqual(len(measured.row().split("|")), 35)

    def test_selects_latest_completed_turn_and_counts_unwrapped_tool_local(self):
        rows = [
            event("2026-08-17T00:00:00Z", "event_msg", {
                "type": "task_started", "turn_id": "old"
            }),
            event("2026-08-17T00:00:01Z", "event_msg", {
                "type": "task_complete", "turn_id": "old", "last_agent_message": "old"
            }),
            event("2026-08-17T00:00:02Z", "event_msg", {
                "type": "task_started", "turn_id": "new"
            }),
            event("2026-08-17T00:00:02.100Z", "response_item", {
                "type": "function_call", "call_id": "call-1"
            }),
            event("2026-08-17T00:00:02.200Z", "response_item", {
                "type": "function_call_output", "call_id": "call-1",
                "output": "unwrapped result"
            }),
            event("2026-08-17T00:00:03Z", "event_msg", {
                "type": "task_complete", "turn_id": "new", "last_agent_message": "new"
            }),
            event("2026-08-17T00:00:04Z", "event_msg", {
                "type": "task_started", "turn_id": "still-open"
            }),
        ]
        path = self.write_rollout(rows)
        measured = evidence.collect(evidence.ThreadRef(
            "thread-1", path, "openai", "gpt-test"
        ))

        self.assertEqual(measured.turn_id, "new")
        self.assertEqual(measured.local_events, 1)
        self.assertEqual(measured.total_events, 1)
        self.assertEqual(measured.complete, 1)


if __name__ == "__main__":
    unittest.main()
