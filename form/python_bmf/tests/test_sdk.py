from __future__ import annotations

import ast
import tempfile
import unittest
from pathlib import Path

from form.python_bmf import sdk


FORM_DIR = Path(__file__).resolve().parents[2]


class PythonWireBoundaryTests(unittest.TestCase):
    def test_nodeid_is_strict_kernel_data(self) -> None:
        node = sdk.NodeID(1, 2, 12, 5)
        self.assertEqual(str(node), "@1.2.12.5")
        self.assertEqual(sdk.NodeID.parse(str(node)), node)
        for invalid in (-1, 2**32):
            with self.assertRaises(ValueError):
                sdk.NodeID(1, 2, 12, invalid)
        with self.assertRaises(TypeError):
            sdk.NodeID(1, 2, 12, True)

    def test_canonical_formbin2_and_lens_round_trip(self) -> None:
        root = sdk.FormBinaryComposite(
            sdk.FormBinaryLeaf(sdk.NodeID(1, 2, 34, 1)),
            (
                sdk.FormBinaryLeaf(sdk.NodeID(1, 1, 2, 0), "grounded ∴ observed"),
                sdk.FormBinaryLeaf(sdk.NodeID(1, 1, 1, 42)),
                sdk.FormBinaryInt64(-(2**40)),
                sdk.FormBinaryFloat64(3.5),
            ),
        )
        encoded = sdk.encode_form_binary(root)
        self.assertEqual(encoded[:8], b"FORMBIN2")
        self.assertNotIn(b"FKB1", encoded)
        self.assertEqual(sdk.decode_form_binary(encoded), root)
        self.assertEqual(sdk.encode_form_binary(sdk.decode_form_binary(encoded)), encoded)

        with tempfile.TemporaryDirectory(prefix="form-python-wire-") as temp:
            artifact = Path(temp) / "answer.fkb"
            sdk.dump_form_binary(artifact, root)
            self.assertEqual(sdk.load_form_binary(artifact), root)

            issued = sdk.NodeID(1, 2, 12, 5)
            lens = sdk.Lens(
                {
                    str(issued): {
                        "symbol": "answer",
                        "span": {
                            "path": "answer.fk",
                            "start_offset": 0,
                            "end_offset": 2,
                            "start_line": 1,
                            "start_col": 0,
                            "end_line": 1,
                            "end_col": 2,
                        },
                    }
                }
            )
            lens_path = sdk.lens_path_for(artifact)
            lens.write(lens_path)
            loaded = sdk.Lens.load(lens_path)
            self.assertEqual(loaded.symbol_for(issued), "answer")
            self.assertEqual(loaded.span_for(issued).path, "answer.fk")

    def test_codec_rejects_private_or_malformed_binary(self) -> None:
        with self.assertRaisesRegex(ValueError, "bad magic"):
            sdk.decode_form_binary(b"FKB1" + bytes(32))
        with self.assertRaisesRegex(ValueError, "truncated"):
            sdk.decode_form_binary(b"FORMBIN2")
        with self.assertRaises(ValueError):
            sdk.FormBinaryInt64(2**63)

    def test_boundary_contains_no_language_implementation(self) -> None:
        forbidden_defs = {
            "intern",
            "parse_python",
            "evaluate",
            "compile_statement",
            "decompile_module",
            "action_to_python",
        }
        for path in sorted((FORM_DIR / "python_bmf").glob("*.py")):
            source = path.read_text()
            tree = ast.parse(source, filename=str(path))
            names = {
                node.name
                for node in ast.walk(tree)
                if isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef))
            }
            self.assertEqual(names & forbidden_defs, set(), path.name)
            dynamic_calls = {
                node.func.id
                for node in ast.walk(tree)
                if isinstance(node, ast.Call) and isinstance(node.func, ast.Name)
            } & {"eval", "exec"}
            self.assertEqual(dynamic_calls, set(), path.name)
            self.assertLess(len(source.splitlines()), 400, path.name)

        templates = FORM_DIR / "form-stdlib" / "emits" / "python-native-templates"
        self.assertEqual(list(templates.glob("*.py")), [])


class FormSubmoduleBoundaryTests(unittest.TestCase):
    def test_no_consumer_kernel_dependency_remains(self) -> None:
        legacy_fragments = (
            "kernels" + "/" + "python_bmf",
            "kernels" + "." + "python_bmf",
        )
        ignored_parts = {".cache", "node_modules", "target", "__pycache__"}
        hits: list[str] = []
        for path in FORM_DIR.rglob("*"):
            if not path.is_file() or any(part in ignored_parts for part in path.parts):
                continue
            if path.suffix not in {".fk", ".md", ".py", ".sh", ".json", ".ts", ".txt"}:
                continue
            text = path.read_text(errors="replace")
            if any(fragment in text for fragment in legacy_fragments):
                hits.append(str(path.relative_to(FORM_DIR)))
        self.assertEqual(hits, [])


if __name__ == "__main__":
    unittest.main()
