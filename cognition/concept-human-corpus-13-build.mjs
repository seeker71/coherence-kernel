#!/usr/bin/env node
// Rebuild the bounded 13-locale human sentence snapshot from pinned Tatoeba
// detailed exports.  This script uses Node + the host bzip2 executable; Python
// is neither required nor invoked.
import { createHash } from "node:crypto";
import { readFileSync, writeFileSync } from "node:fs";
import { resolve } from "node:path";
import { spawn } from "node:child_process";
import { createInterface } from "node:readline";

const root = resolve(import.meta.dirname, "..");
const archiveDir = resolve(process.argv[2] || ".");
const verifyOnly = process.argv.includes("--verify");
const outputPath = resolve(root, "cognition/fixtures/human-corpus-13/tatoeba-human-sentences.tsv");
const manifestPath = resolve(root, "cognition/fixtures/human-corpus-13/ARCHIVES.tsv");
const offsetsPath = resolve(root, "cognition/concept-human-corpus-13-offsets.fk");
const metadataPath = resolve(root, "cognition/concept-human-corpus-13-metadata.fk");
const labelPath = resolve(root, "cognition/concept-nl-semantic-13-omw.tsv");

const locales = [
  ["en", "eng", "86dfa17528230f4bacd5d51108d0126548ad56f984dab4ad11262d8327ba7e6f"],
  ["id", "ind", "bb69c9bb7b93f5495b9340e29d4ebb3ba792e92a3da2f899afb67b7ff7c8cc71"],
  ["es", "spa", "1c966d212089a20f826c0d4b87cd302627bb939e4946eca2f2e10c4ae6c72058"],
  ["fr", "fra", "ddf85e6d1e2e0cc682779b4fad3260ea207f8c7976577eb595975df10e1300cd"],
  ["pt-br", "por", "93215bf85dfb972d0a537734de84a1345406b6a73fa85b3ccfb0e469b74b1871"],
  ["sw", "swh", "9625be94a36d430366a9f82705e75bed93d93d7cf141254eab3b951bad9331c2"],
  ["de", "deu", "1fa6c77c9d695039710c2ff1efe01e46c1ae80cef4b135b6a119b48326e116f4"],
  ["ru", "rus", "ed9ab4d96b80ba2955a449731de19f333e1f1558f2c8d37767adcf204a0a5b7b"],
  ["zh", "cmn", "4b49d40facbb83e331575f9407ce38882fce491660c6ebf8949c525b114a6ae8"],
  ["ja", "jpn", "30c7a77475b0af1c43f57950303cf200e06045931cb7dae915954c5581c19a50"],
  ["ar", "ara", "d58e778f4cc0a30bbf8db4f92b98824e1bcf7f1bfb9bf5d9d48ef727e23c6a25"],
  ["hi", "hin", "24088064bca54508cd886ec9694f645b40885cb35a0e2361f7bb2bce71d1af09"],
  ["tr", "tur", "d062d36375c7da89b8a7d0656d072bb3ff1a2302bc7c40aadfa178f2ebb6437b"],
];
const targets = [
  [150, "work"], [185, "money"], [270, "family"], [327, "school"],
  [370, "doctor"], [377, "water"], [532, "food"], [628, "hospital"],
  [1102, "bus"], [1357, "rain"],
];

const digest = (value) => createHash("sha256").update(value).digest("hex");
const asciiFold = (text) => text.replace(/[A-Z]/g, (c) => c.toLowerCase());
const wordChar = (char) => {
  if (!char) return false;
  const n = char.charCodeAt(0);
  return (n >= 48 && n <= 57) || (n >= 65 && n <= 90) ||
    (n >= 97 && n <= 122) || n >= 128;
};

function addPattern(nodes, pattern, value) {
  let at = 0;
  for (const char of pattern) {
    if (!nodes[at].next.has(char)) {
      nodes[at].next.set(char, nodes.length);
      nodes.push({ next: new Map(), fail: 0, out: [] });
    }
    at = nodes[at].next.get(char);
  }
  nodes[at].out.push(value);
}

function buildMatcher(labels, unsegmented) {
  const nodes = [{ next: new Map(), fail: 0, out: [] }];
  for (const [surface, ids] of labels) addPattern(nodes, asciiFold(surface), [surface, ids]);
  const queue = [];
  for (const child of nodes[0].next.values()) queue.push(child);
  for (let q = 0; q < queue.length; q++) {
    const at = queue[q];
    for (const [char, child] of nodes[at].next) {
      queue.push(child);
      let fail = nodes[at].fail;
      while (fail && !nodes[fail].next.has(char)) fail = nodes[fail].fail;
      if (nodes[fail].next.has(char)) fail = nodes[fail].next.get(char);
      nodes[child].fail = fail;
      nodes[child].out.push(...nodes[fail].out);
    }
  }
  return (sentence) => {
    const folded = asciiFold(sentence);
    let at = 0;
    const ids = new Set();
    const surfaces = new Map();
    let offset = 0;
    for (const char of folded) {
      while (at && !nodes[at].next.has(char)) at = nodes[at].fail;
      if (nodes[at].next.has(char)) at = nodes[at].next.get(char);
      offset += char.length;
      for (const [surface, surfaceIds] of nodes[at].out) {
        const width = asciiFold(surface).length;
        const start = offset - width;
        if (!unsegmented && (wordChar(folded[start - 1]) || wordChar(folded[offset]))) continue;
        for (const id of surfaceIds) ids.add(id);
        if (!surfaces.has(surface)) surfaces.set(surface, surfaceIds);
      }
    }
    return { ids: [...ids].sort((a, b) => a - b), surfaces };
  };
}

function readLabels() {
  const rows = readFileSync(labelPath, "utf8").trimEnd().split("\n");
  const header = rows.shift().split("\t");
  const byLocale = new Map(header.slice(1).map((code) => [code, new Map()]));
  rows.forEach((line, id) => {
    const cells = line.split("\t");
    header.slice(1).forEach((code, column) => {
      const surface = cells[column + 1];
      if (!surface) return;
      const map = byLocale.get(code);
      if (!map.has(surface)) map.set(surface, []);
      map.get(surface).push(id);
    });
  });
  return byLocale;
}

async function shaFile(path) {
  const hash = createHash("sha256");
  const stream = (await import("node:fs")).createReadStream(path);
  for await (const chunk of stream) hash.update(chunk);
  return hash.digest("hex");
}

async function selectLocale(locale, lang, labels) {
  const archive = resolve(archiveDir, `${lang}.tsv.bz2`);
  const matcher = buildMatcher(labels, locale === "zh" || locale === "ja");
  const chosenTargets = new Map();
  let ambiguity = null;
  let negative = null;
  let scanned = 0;
  const used = new Set();
  const child = spawn("bzip2", ["-dc", archive], { stdio: ["ignore", "pipe", "inherit"] });
  const lines = createInterface({ input: child.stdout, crlfDelay: Infinity });
  for await (const line of lines) {
    scanned++;
    const cells = line.split("\t");
    if (cells.length !== 6) continue;
    const [sentenceId, rowLang, sentence, author, added, modified] = cells;
    if (rowLang !== lang || !author || author === "\\N" || sentence.includes("\t")) continue;
    const length = [...sentence].length;
    if (length < 12 || length > 220) continue;
    const found = matcher(sentence);
    const base = { locale, lang, sentenceId, author, added, modified, sentence,
      ids: found.ids, rowHash: digest(line) };
    if (!negative && found.ids.length === 0) {
      negative = { ...base, role: "negative", domain: "none", expected: -1, surface: "" };
      used.add(sentenceId);
    }
    if (!ambiguity) {
      for (const [surface, ids] of found.surfaces) {
        if (ids.length > 1) {
          ambiguity = { ...base, role: "ambiguity", domain: "collision", expected: ids[0], surface };
          used.add(sentenceId);
          break;
        }
      }
    }
    for (const [id, domain] of targets) {
      if (chosenTargets.has(id) || used.has(sentenceId) || !found.ids.includes(id)) continue;
      const surface = [...labels].find(([candidate, ids]) => ids.includes(id) && found.surfaces.has(candidate))?.[0] || "";
      chosenTargets.set(id, { ...base, role: "domain", domain, expected: id, surface });
      used.add(sentenceId);
    }
  }
  const code = await new Promise((accept) => child.on("close", accept));
  if (code !== 0) throw new Error(`bzip2 failed for ${archive}: ${code}`);
  const rows = [...chosenTargets.values()];
  if (ambiguity) rows.push(ambiguity);
  if (negative) rows.push(negative);
  rows.sort((a, b) => Number(a.sentenceId) - Number(b.sentenceId));
  return { rows, scanned, targetCount: chosenTargets.size, hasAmbiguity: !!ambiguity, hasNegative: !!negative };
}

function rowText(row) {
  const page = `https://tatoeba.org/en/sentences/show/${row.sentenceId}`;
  return [row.locale, row.lang, row.sentenceId, row.author, row.added, row.modified,
    "CC-BY-2.0-FR", page, row.role, row.domain, row.expected, row.surface,
    row.ids.length, row.ids.join(","), "human-contributed-unreviewed", row.rowHash,
    row.sentence].join("\t");
}

const labels = readLabels();
const manifest = ["locale\ttatoeba_lang\tretrieved_utc\tarchive_sha256\tarchive_url\tlicense\tlicense_url"];
const allRows = [];
const reports = [];
for (const [locale, lang, expectedHash] of locales) {
  const archive = resolve(archiveDir, `${lang}.tsv.bz2`);
  const observedHash = await shaFile(archive);
  if (observedHash !== expectedHash) throw new Error(`${lang} archive hash ${observedHash} != ${expectedHash}`);
  const url = `https://downloads.tatoeba.org/exports/per_language/${lang}/${lang}_sentences_detailed.tsv.bz2`;
  manifest.push([locale, lang, "2026-07-18T06:52:00Z", expectedHash, url,
    "CC-BY-2.0-FR", "https://creativecommons.org/licenses/by/2.0/fr/"].join("\t"));
  const report = await selectLocale(locale, lang, labels.get(locale));
  allRows.push(...report.rows);
  reports.push({ locale, ...report });
}
const header = "locale\ttatoeba_lang\tsentence_id\tauthor\tadded_utc\tmodified_utc\tlicense\tsentence_url\trole\tdomain\texpected_concept_id\tmatched_surface\tdetected_concept_count\tdetected_concept_ids\treview_state\tsource_row_sha256\tsentence";
const snapshot = `${header}\n${allRows.map(rowText).join("\n")}\n`;
const manifestText = `${manifest.join("\n")}\n`;
const offsets = [Buffer.byteLength(`${header}\n`)];
for (const row of allRows) offsets.push(offsets.at(-1) + Buffer.byteLength(`${rowText(row)}\n`));
const offsetsText = `; concept-human-corpus-13-offsets.fk -- generated byte index over selected source rows.\n; witnessed: 2026-07-18 -> ${allRows.length} attributed rows indexed\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn hcnl13-offsets () (list\n${offsets.map((n) => `    ${n}`).join("\n")}))\n)\n`;
const uniqueConcepts = new Set(allRows.flatMap((row) => row.ids));
const countRole = (role) => allRows.filter((row) => row.role === role).length;
const localeStats = locales.map(([locale]) => {
  const rows = allRows.filter((row) => row.locale === locale);
  return [locale, rows.length, rows.filter((row) => row.role === "domain").length,
    rows.filter((row) => row.role === "ambiguity").length,
    rows.filter((row) => row.role === "negative").length];
});
const metadataText = `; concept-human-corpus-13-metadata.fk -- exact bounded public-corpus evidence.\n; Source: Tatoeba detailed exports, retrieved 2026-07-18; CC BY 2.0 FR.\n; Human-contributed does not imply native-speaker or human-reviewed; reviewed count is zero.\n; witnessed: 2026-07-18 -> ${allRows.length} source rows, ${uniqueConcepts.size} detected concepts, 13/13 negative rows\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn hcnl13-row-count () ${allRows.length})\n  (defn hcnl13-locale-count () 13)\n  (defn hcnl13-domain-count () ${countRole("domain")})\n  (defn hcnl13-ambiguity-count () ${countRole("ambiguity")})\n  (defn hcnl13-negative-count () ${countRole("negative")})\n  (defn hcnl13-detection-count () ${allRows.reduce((n, row) => n + row.ids.length, 0)})\n  (defn hcnl13-unique-concept-count () ${uniqueConcepts.size})\n  (defn hcnl13-attributed-count () ${allRows.length})\n  (defn hcnl13-human-reviewed-count () 0)\n  (defn hcnl13-data-path () "cognition/fixtures/human-corpus-13/tatoeba-human-sentences.tsv")\n  (defn hcnl13-manifest-path () "cognition/fixtures/human-corpus-13/ARCHIVES.tsv")\n  (defn hcnl13-data-sha256 () "${digest(snapshot)}")\n  (defn hcnl13-manifest-sha256 () "${digest(manifestText)}")\n  (defn hcnl13-locale-stats () (list\n${localeStats.map((row) => `    (list "${row[0]}" ${row.slice(1).join(" ")})`).join("\n")}))\n)\n`;
if (verifyOnly) {
  if (readFileSync(outputPath, "utf8") !== snapshot) throw new Error("selected sentence snapshot differs");
  if (readFileSync(manifestPath, "utf8") !== manifestText) throw new Error("archive manifest differs");
  if (readFileSync(offsetsPath, "utf8") !== offsetsText) throw new Error("Form offsets differ");
  if (readFileSync(metadataPath, "utf8") !== metadataText) throw new Error("Form metadata differs");
} else {
  writeFileSync(outputPath, snapshot);
  writeFileSync(manifestPath, manifestText);
  writeFileSync(offsetsPath, offsetsText);
  writeFileSync(metadataPath, metadataText);
}
process.stdout.write(`${JSON.stringify({ snapshotSha256: digest(snapshot), rows: allRows.length, reports }, null, 2)}\n`);
