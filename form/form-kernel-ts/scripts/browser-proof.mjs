import { build } from "esbuild";

function assert(condition, message) {
  if (!condition) throw new Error(`browser proof: ${message}`);
}

const buildResult = await build({
  entryPoints: ["src/browser.ts"],
  bundle: true,
  format: "esm",
  platform: "browser",
  target: ["es2022"],
  minify: true,
  legalComments: "none",
  metafile: true,
  treeShaking: true,
  write: false,
});

assert(buildResult.outputFiles.length === 1, "expected one bundled browser module");
const output = buildResult.outputFiles[0].text;
const forbidden = [
  /node:(?:fs|path|worker_threads)/,
  /require\(["']node:/,
  /\bBuffer\b/,
  /\bprocess\.(?:env|stdout|stderr|argv)/,
];
for (const pattern of forbidden) {
  assert(!pattern.test(output), `bundle contains forbidden dependency ${pattern}`);
}
const inputs = Object.keys(buildResult.metafile.inputs);
assert(!inputs.some((path) => path.endsWith("node-host.ts")), "Node adapter entered browser graph");
assert(!inputs.some((path) => path.endsWith("main.ts")), "Node CLI entered browser graph");

const moduleURL = `data:text/javascript;base64,${Buffer.from(output).toString("base64")}`;
const browser = await import(moduleURL);

assert(browser.RBasic.CHOICE === 20, "CHOICE category drifted");
assert(browser.RBasic.CHOICE_MATCH === 35, "CHOICE_MATCH category drifted");
assert(browser.RBasic.RESOLVE === 5, "RESOLVE category drifted");
assert(browser.RBasic.FIELD_RESOLVE === 97, "FIELD_RESOLVE category drifted");

const arithmetic = browser.runLocalFormBinary("(add 1 (mul 2 3))");
assert(arithmetic.result === "7", `arithmetic result ${arithmetic.result}`);
assert(arithmetic.root.startsWith("@0."), `canonical composite identity ${arithmetic.root}`);
assert(arithmetic.rootCategory === "@1.2.12.1", `root category ${arithmetic.rootCategory}`);
assert(arithmetic.trace.total_walks > 0, "trace is empty");

const recursive = browser.runLocalFormBinary(`(do
  (defn fact (n) (if (le n 1) 1 (mul n (fact (sub n 1)))))
  (fact 8))`);
assert(recursive.result === "40320", `recursive result ${recursive.result}`);

const asciiBytes = browser.runLocalFormBinary(`(string_bytes "Form")`);
assert(asciiBytes.result === "[70, 111, 114, 109]", `ASCII bytes ${asciiBytes.result}`);
const unicodeNulBytes = browser.runLocalFormBinary(`(string_bytes "A\u0000λ🙂")`);
assert(
  unicodeNulBytes.result === "[65, 0, 206, 187, 240, 159, 153, 130]",
  `Unicode/NUL bytes ${unicodeNulBytes.result}`,
);
const asciiByteFold = browser.runLocalFormBinary(`(do
  (defn sum-byte (acc byte) (add acc byte))
  (string_byte_fold "Form" 0 sum-byte))`);
assert(asciiByteFold.result === "404", `ASCII byte fold ${asciiByteFold.result}`);
const unicodeNulByteFold = browser.runLocalFormBinary(`(do
  (defn sum-byte (acc byte) (add acc byte))
  (string_byte_fold "A\u0000λ🙂" 0 sum-byte))`);
assert(
  unicodeNulByteFold.result === "1140",
  `Unicode/NUL byte fold ${unicodeNulByteFold.result}`,
);
const longByteFold = browser.runLocalFormBinary(`(do
  (defn count-byte (acc byte) (add acc 1))
  (string_byte_fold "${"x".repeat(100_000)}" 0 count-byte))`);
assert(longByteFold.result === "100000", `long byte fold ${longByteFold.result}`);

const printed = browser.runLocalFormBinary(`(do (print "browser-canonical") 9)`);
assert(printed.result === "9", `print expression result ${printed.result}`);
assert(printed.stdout === "browser-canonical\n", `stdout capture ${JSON.stringify(printed.stdout)}`);
assert(printed.stderr === "", `stderr capture ${JSON.stringify(printed.stderr)}`);

const source = `(do (let message "browser-λ") (str_concat message "-roundtrip"))`;
const sourceKernel = new browser.Kernel();
const sourceRoot = browser.readAll(sourceKernel, source);
const artifact = browser.serializeRecipeArtifact(sourceKernel, sourceRoot);
const targetKernel = new browser.Kernel();
const targetRoot = browser.deserializeRecipeArtifact(targetKernel, artifact);
const targetValue = browser.walk(targetKernel, targetRoot, new browser.Frame(null));
assert(targetKernel.render(targetValue) === "browser-λ-roundtrip", "binary reader round-trip failed");

const fieldProof = browser.runFieldRuntimeProof();
assert(fieldProof.marker === "field-model-form-browser-runtime-proof:4", "field marker drifted");
assert(fieldProof.score === 4, `field proof score ${fieldProof.score}`);
assert(fieldProof.checks.length === 4, "field proof checks incomplete");

console.log(JSON.stringify({
  verdict: "PASS",
  bundle_bytes: buildResult.outputFiles[0].contents.length,
  browser_graph_inputs: inputs.length,
  arithmetic: arithmetic.result,
  recursive: recursive.result,
  string_bytes_ascii: asciiBytes.result,
  string_bytes_unicode_nul: unicodeNulBytes.result,
  string_byte_fold_ascii: asciiByteFold.result,
  string_byte_fold_unicode_nul: unicodeNulByteFold.result,
  string_byte_fold_100k: longByteFold.result,
  canonical_root: arithmetic.root,
  binary_roundtrip: targetKernel.render(targetValue),
  field: fieldProof,
}));
