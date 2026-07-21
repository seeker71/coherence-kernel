// Verify the committed public program bytes against pinned Exercism clones and
// regenerate source-manifest.tsv. Node only; no language fixture is executed.
import { createHash } from "node:crypto";
import { execFileSync } from "node:child_process";
import { readFileSync, writeFileSync } from "node:fs";
import { dirname, join } from "node:path";
import { fileURLToPath } from "node:url";

const fixtureRoot = dirname(fileURLToPath(import.meta.url));
const upstreamRoot = process.argv[2];
if (!upstreamRoot) throw new Error("usage: node build.mjs <exercism-clones-root>");

const families = [
  "binary-search",
  "matching-brackets",
  "prime-factors",
  "run-length-encoding",
  "robot-simulator",
];
const languages = [
  ["python", "py"], ["javascript", "js"], ["typescript", "ts"],
  ["java", "java"], ["c", "c"], ["cpp", "cpp"], ["csharp", "cs"],
  ["go", "go"], ["rust", "rs"], ["ruby", "rb"], ["php", "php"],
  ["swift", "swift"], ["kotlin", "kt"],
];
const javaNames = {
  "binary-search": "BinarySearch.java",
  "matching-brackets": "BracketChecker.java",
  "prime-factors": "PrimeFactorsCalculator.java",
  "run-length-encoding": "RunLengthEncoding.java",
  "robot-simulator": "Robot.java",
};
const kotlinNames = {
  "binary-search": "BinarySearch.kt",
  "matching-brackets": "MatchingBrackets.kt",
  "prime-factors": "PrimeFactors.kt",
  "run-length-encoding": "RunLengthEncoding.kt",
  "robot-simulator": "Robot.kt",
};
const swiftNames = {
  "binary-search": "Sources/BinarySearch/BinarySearchExample.swift",
  "matching-brackets": "Sources/MatchingBrackets/MatchingBracketsExample.swift",
  "prime-factors": "Sources/PrimeFactors/PrimeFactorsExample.swift",
  "run-length-encoding": "Sources/RunLengthEncoding/RunLengthEncodingExample.swift",
  "robot-simulator": "Sources/RobotSimulator/RobotSimulatorExample.swift",
};

function upstreamPath(language, family) {
  const base = `exercises/practice/${family}/.meta/`;
  if (language === "python") return `${base}example.py`;
  if (language === "javascript") return `${base}proof.ci.js`;
  if (language === "typescript") return `${base}proof.ci.ts`;
  if (language === "java") return `${base}src/reference/java/${javaNames[family]}`;
  if (language === "c") return `${base}example.c`;
  if (language === "cpp") return `${base}example.cpp`;
  if (language === "csharp") return `${base}Example.cs`;
  if (language === "go") return `${base}example.go`;
  if (language === "rust") return `${base}example.rs`;
  if (language === "ruby") return `${base}example.rb`;
  if (language === "php") return `${base}example.php`;
  if (language === "swift") return `${base}${swiftNames[family]}`;
  return `${base}src/reference/kotlin/${kotlinNames[family]}`;
}

const rows = ["family\tlanguage\tupstream\tcommit\tupstream_path\tlocal_file\tlicense\tsha256\tbytes"];
for (const family of families) {
  for (const [language, extension] of languages) {
    const repository = join(upstreamRoot, language);
    const commit = execFileSync("git", ["-C", repository, "rev-parse", "HEAD"], { encoding: "utf8" }).trim();
    const relativeUpstream = upstreamPath(language, family);
    const upstreamBytes = readFileSync(join(repository, relativeUpstream));
    const localRelative = `${family}/${language}.${extension}`;
    const localBytes = readFileSync(join(fixtureRoot, localRelative));
    if (!upstreamBytes.equals(localBytes)) {
      throw new Error(`fixture differs from upstream: ${family}/${language}`);
    }
    const digest = createHash("sha256").update(localBytes).digest("hex");
    rows.push([
      family, language, `https://github.com/exercism/${language}`, commit,
      relativeUpstream, localRelative, "MIT Exercism 2021", digest, localBytes.length,
    ].join("\t"));
  }
}
writeFileSync(join(fixtureRoot, "source-manifest.tsv"), `${rows.join("\n")}\n`);
console.log(`verified ${families.length * languages.length} exact public programs`);
