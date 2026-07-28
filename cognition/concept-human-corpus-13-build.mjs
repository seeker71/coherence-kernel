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
  ["en", "eng", "cfed73b192687e58394630d769cc787e28c7640c68aa2d3d3abc108555d90eb7"],
  ["id", "ind", "514d1b821d36e96b9e04c2b4b283c27081482e03e6a10c257a1c1e16d7f0e77c"],
  ["es", "spa", "751c14000eaf62527b5a8530abebacd770880d2fb0afd7d0f61133efeb4b6f3b"],
  ["fr", "fra", "1e68b07ea01f9989a2330a11101dff0182652b59f742566a67cae60e33e2c412"],
  ["pt-br", "por", "327d317eb1dcb9ae84a2799e5d624d2342a388757cf87a9978414c140d524baf"],
  ["sw", "swh", "9625be94a36d430366a9f82705e75bed93d93d7cf141254eab3b951bad9331c2"],
  ["de", "deu", "67fa43ae8269f145342b4206b7cedcc226e5e5e9a24116f2a7eb07225dc7fe55"],
  ["ru", "rus", "623413d0d61bfbce731f6a21791203178953745e452c8b3c1837bc90acfbaf4e"],
  ["zh", "cmn", "b66b656fc8c1c8e7a7e9dd0cc910854a1ce03888246f5a9eeb6cb83841d5c8a2"],
  ["ja", "jpn", "695821b54d26dbc339de6f7721d3810adde8410d2579ce0a3633768a0999de49"],
  ["ar", "ara", "03173588adc369158756c8fe18804440a0a0e7a75358bbeae0f126c5d07a90d5"],
  ["hi", "hin", "1a66416dcad9f2e222ada2c5d3e74e74881e59b788625960e329182b4833234f"],
  ["tr", "tur", "b599da7d1848ecaa8b2624383fa23e8390cba0869258ac982aacc6bdef28b4fc"],
];
const rowsPerLocale = 100;
// Concepts were looked up by exact English anchor in the committed 10k table.
// They are selection strata, never prompts passed to detection.
const targets = [
  [79, "time"], [122, "love"], [150, "work"], [167, "home"],
  [185, "money"], [212, "house"], [248, "car"], [259, "friend"],
  [270, "family"], [327, "school"], [332, "phone"], [349, "police"],
  [365, "music"], [370, "doctor"], [377, "water"], [454, "fire"],
  [468, "child"], [487, "city"], [493, "office"], [532, "food"],
  [537, "dog"], [571, "book"], [628, "hospital"], [683, "road"],
  [724, "law"], [786, "train"], [893, "government"], [901, "sea"],
  [1040, "cat"], [1045, "village"], [1063, "plane"], [1098, "computer"],
  [1102, "bus"], [1123, "river"], [1338, "animal"], [1357, "rain"],
  [1432, "restaurant"], [1501, "market"], [1595, "mountain"],
  [1772, "weather"], [1992, "airport"], [2346, "internet"],
  [3116, "electricity"], [3255, "sport"], [3710, "parent"],
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
  const ambiguityByAuthor = new Map();
  const negativeByAuthor = new Map();
  const openByConcept = new Map();
  const openByAuthor = new Map();
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
    if (found.ids.length === 0 && !negativeByAuthor.has(author)) {
      negativeByAuthor.set(author,
        { ...base, role: "negative", domain: "none", expected: -1, surface: "" });
    }
    for (const [surface, ids] of found.surfaces) {
      if (ids.length > 1 && !ambiguityByAuthor.has(author)) {
        ambiguityByAuthor.set(author,
          { ...base, role: "ambiguity", domain: "collision", expected: ids[0], surface });
        break;
      }
    }
    for (const [id, domain] of targets) {
      if (chosenTargets.has(id) || used.has(sentenceId) || !found.ids.includes(id)) continue;
      const surface = [...labels].find(([candidate, ids]) => ids.includes(id) && found.surfaces.has(candidate))?.[0] || "";
      chosenTargets.set(id, { ...base, role: "domain", domain, expected: id, surface });
      used.add(sentenceId);
    }
    if (found.ids.length > 0) {
      const expected = found.ids[0];
      const surface = [...found.surfaces]
        .find(([, ids]) => ids.includes(expected))?.[0] || "";
      const candidate = { ...base, role: "open", domain: "open-lexical",
        expected, surface };
      if (!openByConcept.has(expected)) openByConcept.set(expected, candidate);
      if (!openByAuthor.has(author)) openByAuthor.set(author, candidate);
    }
  }
  const code = await new Promise((accept) => child.on("close", accept));
  if (code !== 0) throw new Error(`bzip2 failed for ${archive}: ${code}`);
  const rows = [...chosenTargets.values()];
  const selectedAuthors = new Set(rows.map((row) => row.author));
  const selectedConcepts = new Set(rows.flatMap((row) => row.ids));
  const addUnique = (row) => {
    if (!row || rows.length >= rowsPerLocale || used.has(row.sentenceId)) return false;
    rows.push(row);
    used.add(row.sentenceId);
    selectedAuthors.add(row.author);
    row.ids.forEach((id) => selectedConcepts.add(id));
    return true;
  };
  [...negativeByAuthor.values()].slice(0, 4).forEach(addUnique);
  [...ambiguityByAuthor.values()].slice(0, 4).forEach(addUnique);
  const poolById = new Map();
  [...openByAuthor.values(), ...openByConcept.values()].forEach((row) =>
    poolById.set(row.sentenceId, row));
  const pool = [...poolById.values()].sort((a, b) =>
    a.rowHash.localeCompare(b.rowHash));
  pool.filter((row) => !selectedAuthors.has(row.author) &&
    row.ids.some((id) => !selectedConcepts.has(id))).forEach(addUnique);
  pool.filter((row) => row.ids.some((id) => !selectedConcepts.has(id))).forEach(addUnique);
  pool.forEach(addUnique);
  if (rows.length !== rowsPerLocale) {
    throw new Error(`${locale} selected ${rows.length}/${rowsPerLocale} rows`);
  }
  rows.sort((a, b) => Number(a.sentenceId) - Number(b.sentenceId));
  return { rows, scanned, targetCount: chosenTargets.size,
    ambiguityCount: rows.filter((row) => row.role === "ambiguity").length,
    negativeCount: rows.filter((row) => row.role === "negative").length,
    openCount: rows.filter((row) => row.role === "open").length,
    contributorCount: new Set(rows.map((row) => row.author)).size };
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
  manifest.push([locale, lang, "2026-07-18T09:01:06Z", expectedHash, url,
    "CC-BY-2.0-FR", "https://creativecommons.org/licenses/by/2.0/fr/"].join("\t"));
  const report = await selectLocale(locale, lang, labels.get(locale));
  allRows.push(...report.rows);
  const { rows: selectedRows, ...summary } = report;
  reports.push({ locale, ...summary });
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
    rows.filter((row) => row.role === "open").length,
    rows.filter((row) => row.role === "ambiguity").length,
    rows.filter((row) => row.role === "negative").length,
    new Set(rows.map((row) => row.author)).size];
});
const metadataText = `; concept-human-corpus-13-metadata.fk -- exact bounded public-corpus evidence.\n; Source: Tatoeba detailed exports, retrieved 2026-07-18; CC BY 2.0 FR.\n; Human-contributed does not imply native-speaker or human-reviewed; reviewed count is zero.\n; witnessed: 2026-07-18 -> ${allRows.length} source rows, ${uniqueConcepts.size} detected concepts, 13/13 negative rows\n; preludes: form/form-stdlib/core.fk\n(do\n  (defn hcnl13-row-count () ${allRows.length})\n  (defn hcnl13-locale-count () 13)\n  (defn hcnl13-domain-count () ${countRole("domain")})\n  (defn hcnl13-ambiguity-count () ${countRole("ambiguity")})\n  (defn hcnl13-negative-count () ${countRole("negative")})\n  (defn hcnl13-detection-count () ${allRows.reduce((n, row) => n + row.ids.length, 0)})\n  (defn hcnl13-unique-concept-count () ${uniqueConcepts.size})\n  (defn hcnl13-attributed-count () ${allRows.length})\n  (defn hcnl13-human-reviewed-count () 0)\n  (defn hcnl13-data-path () "cognition/fixtures/human-corpus-13/tatoeba-human-sentences.tsv")\n  (defn hcnl13-manifest-path () "cognition/fixtures/human-corpus-13/ARCHIVES.tsv")\n  (defn hcnl13-data-sha256 () "${digest(snapshot)}")\n  (defn hcnl13-manifest-sha256 () "${digest(manifestText)}")\n  (defn hcnl13-locale-stats () (list\n${localeStats.map((row) => `    (list "${row[0]}" ${row.slice(1).join(" ")})`).join("\n")}))\n)\n`;
const completedMetadataText = metadataText
  .replace("13/13 negative rows", `${countRole("negative")} retained negative rows`)
  .replace("  (defn hcnl13-ambiguity-count",
    `  (defn hcnl13-open-count () ${countRole("open")})\n  (defn hcnl13-ambiguity-count`)
  .replace("  (defn hcnl13-locale-stats",
    `  (defn hcnl13-hash-valid? (path expected)\n    (if (str_eq (host-exec (str_concat "/usr/bin/shasum -a 256 "\n      (str_concat path (str_concat " | /usr/bin/grep -q '^"\n        (str_concat expected "  ' && printf 1")))) "") "1") 1 0))\n  (defn hcnl13-artifacts-valid? ()\n    (and (eq (hcnl13-hash-valid? (hcnl13-data-path)\n               (hcnl13-data-sha256)) 1)\n         (eq (hcnl13-hash-valid? (hcnl13-manifest-path)\n               (hcnl13-manifest-sha256)) 1)))\n  (defn hcnl13-locale-stats`);
if (verifyOnly) {
  if (readFileSync(outputPath, "utf8") !== snapshot) throw new Error("selected sentence snapshot differs");
  if (readFileSync(manifestPath, "utf8") !== manifestText) throw new Error("archive manifest differs");
  if (readFileSync(offsetsPath, "utf8") !== offsetsText) throw new Error("Form offsets differ");
  if (readFileSync(metadataPath, "utf8") !== completedMetadataText) throw new Error("Form metadata differs");
} else {
  writeFileSync(outputPath, snapshot);
  writeFileSync(manifestPath, manifestText);
  writeFileSync(offsetsPath, offsetsText);
  writeFileSync(metadataPath, completedMetadataText);
}
process.stdout.write(`${JSON.stringify({ snapshotSha256: digest(snapshot), rows: allRows.length, reports }, null, 2)}\n`);
